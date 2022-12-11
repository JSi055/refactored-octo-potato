
import sqlite3
import time

sql_con: sqlite3.Connection
sql_cur: sqlite3.Cursor

start_time_seconds: int
last_commit_time: int

def loadDatabase(dbFile) -> sqlite3.Connection:
    global sql_con
    global sql_cur
    sql_con = sqlite3.connect(dbFile)

    sql_cur = sql_con.cursor()
    sql_cur.execute("CREATE TABLE loadTest(time integer, value float)")
    sql_con.commit()

    last_commit_time = time.time() * 1000
    resetVoltageTime()

def resetVoltageTime():
    start_time_seconds = round(time.time())

def putVoltage(voltage: float):
    global last_commit_time

    t = time.time()
    timeDelta = start_time_seconds - round(t)

    sql_cur.execute("INSERT INTO loadTest (time, value) VALUES (" + timeDelta + ", " + voltage + ")")

    # commit every 3 seconds
    if t - last_commit_time > 3:
        sql_con.commit()
        last_commit_time = round(t)


def close():

    sql_con.commit()
    sql_con.close()