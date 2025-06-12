from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from pydantic import EmailStr
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
from app.autogen.models.period_entries import PeriodEntries
from app.autogen.schemas.period_entries_schema import PeriodEntriesSchema
import uuid

router = APIRouter()

@router.post("/{group_id}")
def add_or_update_period_entry(
    group_id: str,
    period_entries: PeriodEntriesSchema,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users
    from app.autogen.models.groups import Groups

    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    group = db.query(Groups).filter(Groups.__table__.c.id == group_id).first()
    if not group:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Group not found")
    
    print("Period Entries:", period_entries)

    existing_entry = db.query(PeriodEntries).filter(PeriodEntries.__table__.c.id == period_entries.id).first()

    if existing_entry:
        updated = period_entries.to_model()
        updated.id = existing_entry.Id
        updated = db.merge(updated)
    else:
        new_entry = period_entries.to_model()
        new_entry.id = period_entries.id or uuid.uuid4()
        db.add(new_entry)

    db.commit()
    db.refresh(new_entry if not existing_entry else updated)

    return (new_entry if not existing_entry else updated).to_schema()


@router.delete("/{period_entry_id}")
def delete_period_entry(
    period_entry_id: str,
    email: EmailStr = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    from app.autogen.models.users import Users

    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    entry = db.query(PeriodEntries).filter(PeriodEntries.__table__.c.id == period_entry_id).first()
    if not entry:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Period entry not found")

    db.delete(entry)
    db.commit()

    return {"detail": "Period entry deleted successfully"}