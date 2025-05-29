from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import BaseModel, EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.db.models.groups import Groups
from app.db.models.users import Users

router = APIRouter()

@router.get("/me")
def get_current_user(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    from app.db.models.users import Users
    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    return user.to_schema()

@router.get("/all")
def get_users(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    from app.db.models.users import Users
    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Admin user not found")
    if not user._is_admin:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="You do not have permission to view users")
    
    users = db.query(Users).all()
    return [
        user.to_schema() for user in users
    ]

@router.get("/group")
def get_group(email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    from app.db.models.users import Users
    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    if not user._group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User does not belong to any group")
    
    group = db.query(Groups).filter(Groups._id == user._group).first()
    if not group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Group not found")
    
    return group.to_schema()

@router.post("/device_token")
def set_device_token(req: Users, email: str = Depends(get_current_user_email), db: Session = Depends(get_db)):
    from app.db.models.users import Users
    user = db.query(Users).filter(Users._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    user._device_token = req._device_token
    db.commit()
    db.refresh(user)
    
    return {"message": "Device token updated successfully"}