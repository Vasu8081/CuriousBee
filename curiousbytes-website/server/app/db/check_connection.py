import os
import time
import psycopg2
from psycopg2 import OperationalError

DB_URL = os.getenv("DATABASE_URL")

if not DB_URL:
    print("❌ DATABASE_URL not set")
    exit(1)

print(f"🔄 Waiting for PostgreSQL to be ready at {DB_URL}...")

while True:
    try:
        conn = psycopg2.connect(DB_URL)
        conn.close()
        print("✅ PostgreSQL is ready.")
        break
    except OperationalError as e:
        print(f"⏳ PostgreSQL not ready yet: {e}")
        time.sleep(2)