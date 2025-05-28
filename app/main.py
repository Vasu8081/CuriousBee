# app/main.py
import uuid
from fastapi import FastAPI
from app.db.database import get_db
from app.db.models import Groups

app = FastAPI()

@app.get("/")
def health():
    return {"status": "ok"}


db = get_db()

groups = db.query(Groups).all()

for group in groups:
    print(f"Group ID: {group.id()}, Name: {group.name()}")

db.add(group)
db.commit()
db.refresh(group)
db.close()