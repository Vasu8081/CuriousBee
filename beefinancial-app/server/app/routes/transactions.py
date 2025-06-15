from fastapi import APIRouter, Depends, HTTPException, status
from sqlalchemy.orm import Session
from app.core.dependencies import get_current_user_email
from app.db.database import get_db
import uuid
from app.autogen.models.users import Users
from app.autogen.models.accounts import Accounts
from app.autogen.schemas.users_schema import UsersSchema
from app.autogen.schemas.accounts_schema import AccountsSchema
from app.autogen.models.transactions import Transactions
from app.autogen.schemas.transactions_schema import TransactionsSchema


router = APIRouter()

@router.get("/{user_id}", response_model=list[TransactionsSchema])
def get_transactions(
    user_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    print(f"Fetching transactions for user ID: {user_id}")
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    transactions = db.query(Transactions).filter(Transactions.__table__.c.user_id == user_id).all()
    
    return [transaction.to_schema() for transaction in transactions]

@router.post("/{user_id}")
def create_transaction(
    user_id: uuid.UUID,
    transactionsSchema: TransactionsSchema,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    print(f"Creating transaction for user ID: {user_id}, transactionsSchema: {transactionsSchema}")
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    existing_transaction = db.query(Transactions).filter(Transactions.__table__.c.id == transactionsSchema.id).first()
    if existing_transaction:
        updated = transactionsSchema.to_model()
        updated.id = existing_transaction.id
        updated = db.merge(updated)
    else:
        new_transaction = transactionsSchema.to_model()
        new_transaction.id = new_transaction.id or uuid.uuid4()
        new_transaction.user_id = user.id
        db.add(new_transaction)
    
    db.commit()
    db.refresh(new_transaction if not existing_transaction else updated)
    
    return (new_transaction if not existing_transaction else updated).to_schema()

@router.delete("/{transaction_id}")
def delete_transaction(
    transaction_id: uuid.UUID,
    email: str = Depends(get_current_user_email),
    db: Session = Depends(get_db)
):
    print(f"Deleting transaction with ID: {transaction_id}")
    user = db.query(Users).filter(Users.__table__.c.email == email).first()
    if not user:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="User not found")

    transaction = db.query(Transactions).filter(Transactions.__table__.c.id == transaction_id, Transactions.__table__.c.user_id == user.id).first()
    if not transaction:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Transaction not found")

    db.delete(transaction)
    db.commit()
    
    return {"detail": "Transaction deleted successfully"}

