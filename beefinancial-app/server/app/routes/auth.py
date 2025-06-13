from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import BaseModel, EmailStr
from app.core.authenticate import Authenticate
from app.db.database import get_db
from app.autogen.models.users import Users
from app.core.password_utils import hash_password, verify_password
from app.core.logging import logging
import uuid
from typing import Optional, Dict, Any, cast

router = APIRouter()
auth = Authenticate()

# --------- Request Models ---------
class SignupRequest(BaseModel):
    email: EmailStr
    name: str
    password: str

class LoginRequest(BaseModel):
    email: EmailStr
    password: str

class TokenRefreshRequest(BaseModel):
    refresh_token: str

# --------- Endpoints ---------

@router.post("/signup")
def signup(req: SignupRequest, db: Session = Depends(get_db)) -> Dict[str, Any]:
    logging.info(f"Attempting to sign up user: {req.email}")
    existing: Optional[Users] = db.query(Users).filter(Users.__table__.c.email == req.email).first()
    if existing is not None:
        raise HTTPException(status_code=status.HTTP_409_CONFLICT, detail="User already exists")

    new_user = Users(
        id=uuid.uuid4(),
        email=req.email,
        name=req.name,
        password_hash=hash_password(req.password)
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
def login(req: LoginRequest, db: Session = Depends(get_db)) -> Dict[str, Any]:
    user: Optional[Users] = db.query(Users).filter(Users.__table__.c.email == req.email).first()
    if user is None:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    password_hash_value = cast(Optional[str], user.password_hash)

    if password_hash_value is None or password_hash_value.strip() == "":
        user.password_hash = hash_password(req.password)
        db.commit()
        return {
            "message": "Password set successfully",
            "tokens": auth.generate_tokens(req.email),
            "groupId": user.Id
        }

    if not verify_password(req.password, password_hash_value):
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid password")

    return {
        "message": "Login successful",
        "tokens": auth.generate_tokens(req.email),
        "groupId": user.Id
    }

@router.post("/refresh")
def refresh(req: TokenRefreshRequest, db: Session = Depends(get_db)) -> Dict[str, Any]:
    try:
        payload = auth.decode_refresh_token(req.refresh_token)
        if payload.get("type") != "refresh":
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid token type")

        email = payload.get("sub")
        if not isinstance(email, str):
            raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail="Invalid email in token")

        user: Optional[Users] = db.query(Users).filter(Users.__table__.c.email == email).first()
        if user is None:
            raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

        return {
            "message": "Token refreshed",
            "tokens": auth.generate_tokens(email),
            "groupId": user.Id
        }

    except Exception as e:
        raise HTTPException(status_code=status.HTTP_401_UNAUTHORIZED, detail=str(e))