
import sqlite3
import time

sql_con: sqlite3.Connection
sql_cur: sqlite3.Cursor

start_time_seconds: int
last_commit_time: int

test_id = 0 # the current test id

def loadDatabase(dbFile):
    global sql_con
    global sql_cur
    global last_commit_time
    sql_con = sqlite3.connect(dbFile)

    sql_cur = sql_con.cursor()
    sql_cur.execute("CREATE TABLE loadTest(time integer, value float)")
    sql_cur.execute("CREATE TABLE pulseTests(pulseid integer, value float)")
    sql_cur.execute("CREATE TABLE pulseMetadata("
                    "    pulseid integer, "
                    "    time integer, "
                    "    pre_dur_us integer, "
                    "    dur_us integer, "
                    "    post_dur_us integer"
                    ")")
    sql_con.commit()

    last_commit_time = round(time.time())
    resetVoltageTime()


def resetVoltageTime():
    global start_time_seconds
    start_time_seconds = round(time.time())


def putStartTest(dur_us: int, pre_dur_us: int, post_dur_us: int):
    global test_id
    test_id += 1

    sql_cur.execute("INSERT INTO pulseMetadata"
                    "(pulseid, time, dur_us, pre_dur_us, post_dur_us) VALUES ("
                    + str(test_id) + ", "
                    + str(round(time.time())) + ", "
                    + str(dur_us) + ", "
                    + str(pre_dur_us) + ", "
                    + str(post_dur_us) + ")")
    sql_con.commit()


def putTestComplete():
    # don't need to do much
    sql_con.commit()


def putVoltage(voltage: float):
    global last_commit_time

    t = time.time()
    time_delta = start_time_seconds - round(t)

    sql_cur.execute("INSERT INTO loadTest (time, value) VALUES (" + str(time_delta) + ", " + str(voltage) + ")")

    # commit every 3 seconds
    if t - last_commit_time > 3:
        sql_con.commit()
        last_commit_time = round(t)

def putTestVoltage(voltage: float):
    sql_cur.execute("INSERT INTO loadTest (pulseid, value) VALUES (" + str(test_id) + ", " + str(voltage) + ")")

def close():
    sql_con.commit()
    sql_con.close()
