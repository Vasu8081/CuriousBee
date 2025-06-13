from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
import uuid
from app.autogen.models.users import Users
from app.autogen.schemas.users_schema import UsersSchema
from app.autogen.schemas.accounts_schema import AccountsSchema


router = APIRouter()
@router.get("/me")
def get_current_user(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    return user.to_schema()

@router.get("/all")
def get_users(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Admin user not found")
    if not user.IsAdmin:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="You do not have permission to view users")
    
    users = db.query(Users).all()
    return [user.to_schema() for user in users]

@router.get("/accounts")
def get_user_accounts(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    accounts = user.accounts
    if not accounts:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="No accounts found for user")
    return [account.to_schema() for account in accounts]

@router.post("/device_token")
def set_device_token(
    req: UsersSchema,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    if req.apple_device_token is not None:
        user.AppleDeviceToken = req.apple_device_token
        db.commit()
        db.refresh(user)
        return {"message": "Device token updated successfully"}
    else:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail="Device token must be a string")


