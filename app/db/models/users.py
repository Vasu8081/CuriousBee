import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Users(Base):
    __tablename__ = 'users'

    _id = Column(UUID, primary_key=True)
    _name = Column(String)
    _email = Column(String)
    _group_id = Column(UUID, ForeignKey("groups._id"))

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.id
        self.id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self.name
        self.name = value

    def email(self, value: str = None) -> str:
        if value is None:
            return self.email
        self.email = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.group_id
        self.group_id = value

    def to_schema(self):
        from app.db.schemas.users_schema import UsersSchema
        return UsersSchema.from_orm(self)
