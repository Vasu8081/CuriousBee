# app/main.py
import uuid
from fastapi import FastAPI
from app.db.database import get_db
from app.db.models import Users

app = FastAPI()

@app.get("/")
def health():
    return {"status": "ok"}


db = get_db()

new_user = Users(
    _id=uuid.uuid4(),
    _name="Vasudhan Varma Kandula",
    _email="vasudhanvarma@gmail.com",
    _group_id=None
)
db.add(new_user)
db.commit()
db.refresh(new_user)
db.close()