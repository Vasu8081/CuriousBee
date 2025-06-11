from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.core.logging import logging
from app.autogen.models.groups import Groups
from app.autogen.models.users import Users
from app.autogen.schemas.tasks_schema import TasksSchema
import uuid

router = APIRouter()

@router.get("/{group_id}")
def get_group_tasks(
    group_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.tasks import Tasks

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    if user.GroupId != group_id:
        raise HTTPException(status_code=status.HTTP_403_FORBIDDEN, detail="You do not have permission to view this group's period entries")
    
    tasks = db.query(Tasks).filter(Tasks.__table__.c._group_id == group_id).all()

    return [task.to_schema() for task in tasks]

@router.post("/{group_id}")
def add_or_update_task(
    group_id: uuid.UUID,
    task: TasksSchema,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.tasks import Tasks

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    existing_entry = db.query(Tasks).filter(Tasks.__table__.c._id == task.id).first()

    if existing_entry:
        updated = task.to_model()
        updated._id = existing_entry.Id
        updated = db.merge(updated)
    else:
        new_entry = task.to_model()
        new_entry._id = task.id or uuid.uuid4()
        db.add(new_entry)

    db.commit()
    db.refresh(new_entry if not existing_entry else updated)

    return (new_entry if not existing_entry else updated).to_schema()


@router.delete("/{task_id}")
def delete_group_period_entry(
    task_id: str,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users
    from app.autogen.models.groups import Groups
    from app.autogen.models.tasks import Tasks

    user = db.query(Users).filter(Users.__table__.c._email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    entry = db.query(Tasks).filter(Tasks.__table__.c._id == task_id).first()
    if not entry:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Task not found")

    db.delete(entry)
    db.commit()

    return {"detail": "Task deleted successfully"}