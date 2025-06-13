from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
import uuid
from app.autogen.models.users import Users
from app.autogen.models.accounts import Accounts
from app.autogen.schemas.users_schema import UsersSchema
from app.autogen.schemas.accounts_schema import AccountsSchema


router = APIRouter()

@router.get("/{user_id}", response_model=list[AccountsSchema])
def get_account(
    user_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    print(f"Fetching account for user ID: {user_id}")
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    accounts = db.query(Accounts).filter(Accounts.__table__.c.user_id == user_id).all()
    
    return [account.to_schema() for account in accounts]

@router.post("/{user_id}")
def create_account(
    user_id: uuid.UUID,
    accountsSchema: AccountsSchema,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    print(f"Creating account with ID: {user_id}")
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    existing_account = db.query(Users).filter(Users.__table__.c.id == user_id).first()
    if existing_account:
        updated = accountsSchema.to_model()
        updated.id = existing_account.Id
        updated = db.merge(updated)
    else:
        new_account = accountsSchema.to_model()
        new_account.id = new_account.id or uuid.uuid4()
        new_account.user_id = user.Id
        db.add(new_account)
    
    db.commit()
    db.refresh(new_account if not existing_account else updated)
    
    return (new_account if not existing_account else updated).to_schema()