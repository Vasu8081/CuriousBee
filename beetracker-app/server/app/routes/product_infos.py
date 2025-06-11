from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.core.logging import logging
from app.autogen.models.groups import Groups
from app.autogen.models.users import Users
from app.autogen.schemas.groups_schema import TasksSchema
import uuid

router = APIRouter()

@router.get("/{group_id}")
def get_product_infos(
    group_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.product_infos import ProductInfos

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    if user.GroupId != group_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="You do not have permission to view this group's period entries")
    
    productInfos = db.query(ProductInfos).filter(ProductInfos.__table__.c._group_id == group_id).all()

    return [productInfo.to_schema() for productInfo in productInfos]