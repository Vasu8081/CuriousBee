import time
import psycopg2
from psycopg2 import OperationalError

DB_HOST = "db"
DB_PORT = 5432
DB_NAME = "beetracker"
DB_USER = "vkandula"
DB_PASSWORD = "vkandula"

MAX_RETRIES = 20
RETRY_DELAY = 2

for i in range(MAX_RETRIES):
    try:
        conn = psycopg2.connect(
            host=DB_HOST, port=DB_PORT,
            dbname=DB_NAME, user=DB_USER, password=DB_PASSWORD
        )
        conn.close()
        print("✅ PostgreSQL is available")
        break
    except OperationalError as e:
        print(f"⏳ Waiting for PostgreSQL... attempt {i+1}")
        time.sleep(RETRY_DELAY)
else:
    raise Exception("❌ PostgreSQL not available after max retries.")