
import sqlite3
import threading
import time

sql_con: sqlite3.Connection
sql_cur: sqlite3.Cursor
polling_cur: sqlite3.Cursor

lock = threading.Lock()

start_time_seconds: int
last_commit_time: int

test_id = 0 # the current test id


def loadDatabase(dbFile):
    global sql_con
    global sql_cur
    global polling_cur
    global last_commit_time
    with lock:
        sql_con = sqlite3.connect(dbFile, check_same_thread=False)
        sql_cur = sql_con.cursor()
        polling_cur = sql_con.cursor()

        sql_cur.execute(''' SELECT count(name) FROM sqlite_master WHERE type='table' AND name='loadTest' ''')

        # if the count is 1, then table exists
        if sql_cur.fetchone()[0] == 1:
            print('warning: sql file is not new')
        else:
            sql_cur.execute("CREATE TABLE loadTest(time integer, value float)")
            sql_cur.execute("CREATE TABLE pulseTests(pulseid integer, value float)")
            sql_cur.execute("CREATE TABLE pulseMetadata("
                            "    pulseid integer, "
                            "    time integer, "
                            "    pre_dur_us integer, "
                            "    dur_us integer, "
                            "    post_dur_us integer, "
                            "    current float"
                            ")")
            sql_con.commit()

        last_commit_time = round(time.time())
        resetVoltageTime()


def resetVoltageTime():
    global start_time_seconds
    start_time_seconds = round(time.time())


def putStartTest(dur_us: int, pre_dur_us: int, post_dur_us: int, current: float):
    global test_id
    test_id += 1

    with lock:
        sql_cur.execute("INSERT INTO pulseMetadata"
                        "(pulseid, time, dur_us, pre_dur_us, post_dur_us, current) VALUES ("
                        + str(test_id) + ", "
                        + str(round(time.time())) + ", "
                        + str(dur_us) + ", "
                        + str(pre_dur_us) + ", "
                        + str(post_dur_us) + ", "
                        + str(current) + ")")
        sql_con.commit()


def putTestComplete() -> int:
    # don't need to do much
    with lock:
        sql_con.commit()
    return test_id


def putVoltage(voltage: float):
    global last_commit_time
    global start_time_seconds

    t = time.time()
    time_delta = round(t) - start_time_seconds

    with lock:
        sql_cur.execute("INSERT INTO loadTest (time, value) VALUES (" + str(time_delta) + ", " + str(voltage) + ")")

        # commit every 3 seconds
        if t - last_commit_time > 3:
            sql_con.commit()
            last_commit_time = round(t)

def putTestVoltage(voltage: float):
    with lock:
        sql_cur.execute("INSERT INTO pulseTests (pulseid, value) VALUES (" + str(test_id) + ", " + str(voltage) + ")")


# returns {id: [time,voltage][values]}
def get_pulse_tests() -> dict:
    global polling_cur
    all_tests = {}
    with lock:
        polling_cur.execute("SELECT pulseid, value FROM pulseTests")
        i = 0
        last_pulseid = -1;
        for (pulseid, value) in polling_cur.fetchall():
            test = []
            if pulseid not in all_tests:
                test = [[], []]
                all_tests[pulseid] = test
            else:
                test = all_tests[pulseid]
            if last_pulseid != pulseid:
                i = 0
                last_pulseid = pulseid
            else:
                i += 1
            test[0].append(i)
            test[1].append(value)
    return all_tests


def close():
    with lock:
        sql_con.commit()
        sql_con.close()
