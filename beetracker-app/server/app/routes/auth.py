from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import BaseModel, EmailStr
from app.core.authenticate import Authenticate
from app.db.database import get_db
from app.db.autogen.models.users import Users  # Your SQLAlchemy Users model
from app.core.password_utils import hash_password, verify_password
from app.core.logging import logging
import uuid

router = APIRouter()
auth = Authenticate()

# --------- Request Models ---------
class SignupRequest(BaseModel):
    email: EmailStr
    name: str
    password: str  # Include password

class LoginRequest(BaseModel):
    email: EmailStr
    password: str  # Include password

class TokenRefreshRequest(BaseModel):
    refresh_token: str

# --------- Endpoints ---------

from app.core.password_utils import hash_password

@router.post("/signup")
def signup(req: SignupRequest, db: Session = Depends(get_db)):
    logging.info(f"Attempting to sign up user: {req.email}")
    existing = db.query(Users).filter(Users._email == req.email).first()
    if existing:
        raise HTTPException(status_code=status.HTTP_409_CONFLICT, detail="User already exists")

    password_hash = hash_password(req.password)
    new_user = Users(
        _id=uuid.uuid4(),
        _email=req.email,
        _name=req.name,
        _password_hash=password_hash
    )
    db.add(new_user)
    db.commit()
    db.refresh(new_user)
    logging.info(f"User {req.email} created successfully")
    tokens = auth.generate_tokens(req.email)
    return {
        "message": "User created successfully",
        "tokens": tokens
    }


@router.post("/login")
def login(req: LoginRequest, db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users._email == req.email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    if user.passwordHash() is None or user.passwordHash() == "":
        user._password_hash = hash_password(req.password)
        db.commit()
        return {
            "message": "Password set successfully",
            "tokens": auth.generate_tokens(req.email),
            "groupId": user._group_id
        }
    if not verify_password(req.password, user._password_hash):
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid password")

    tokens = auth.generate_tokens(req.email)
    return {
        "message": "Login successful",
        "tokens": tokens,
        "groupId": user._group_id
    }

@router.post("/refresh")
def refresh(req: TokenRefreshRequest, db: Session = Depends(get_db)):
    try:
        payload = auth.decode_refresh_token(req.refresh_token)
        if payload.get("type") != "refresh":
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid token type")
        email = payload.get("sub")
        user = db.query(Users).filter(Users._email == email).first()
        if not user:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
        
        tokens = auth.generate_tokens(email)
        return {
            "message": "Token refreshed",
            "tokens": tokens,
            "groupId": user._group_id
        }
    except Exception as e:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail=str(e))