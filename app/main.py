from fastapi import FastAPI
from app.db.database import get_db
from app.db.models.users import Users
from app.db.models.groups import Groups

app = FastAPI()

# app.include_router(auth.router, prefix="/auth")
# app.include_router(users.router, prefix="/users")

db = get_db()

users = db.query(Users).all()

group = users[0].group

for user in group.users:
    print(f"User: {user.name()}, Email: {user.email()}")
