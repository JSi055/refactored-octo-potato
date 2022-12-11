
import sqlite3

sql_con: sqlite3.Connection

def loadDatabase(dbFile) -> sqlite3.Connection:
    global sql_con
    sql_con = sqlite3.connect(dbFile)

    cursor = sql_con.cursor()
    cursor.execute("CREATE TABLE loadTest()")

