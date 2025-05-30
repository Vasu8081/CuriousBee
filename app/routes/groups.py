from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.db.autogen.models.groups import Groups
from app.db.autogen.models.users import Users
from app.db.autogen.schemas.period_entries_schema import PeriodEntriesSchema
from app.db.autogen.schemas.groups_schema import GroupsSchema
import uuid

router = APIRouter()

@router.get("/periods", response_model=list[PeriodEntriesSchema])
async def get_periods(
    db: Session = Depends(get_db),
    current_user_email: EmailStr = Depends(get_current_user_email)
):
    """
    Retrieve all periods for the current user.
    """
    user = db.query(Users).filter(Users._email == current_user_email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    if not user.group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User group not found")

    return [entry.to_schema() for entry in user.group.period_entries]

@router.get("/group", response_model=GroupsSchema)
async def get_group(
    db: Session = Depends(get_db),
    current_user_email: EmailStr = Depends(get_current_user_email)
):
    """
    Retrieve the group of the current user.
    """
    user = db.query(Users).filter(Users._email == current_user_email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    if not user.group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User group not found")

    return user.group.to_schema()

@router.get("/get/{group_id}", response_model=GroupsSchema)
async def get_group_by_id(
    group_id: uuid.UUID,
    db: Session = Depends(get_db),
    current_user_email: EmailStr = Depends(get_current_user_email)
):
    """
    Retrieve a group by its ID.
    """
    user = db.query(Users).filter(Users._email == current_user_email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")
    
    if not user.group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User group not found")
    if user.group._id != group_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="Access to this group is forbidden")

    schema = GroupsSchema.model_validate(user.group)
    print(f"Retrieved group schema: {schema}")
    if not schema:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Group not found")
    return schema.model_dump(by_alias=False)