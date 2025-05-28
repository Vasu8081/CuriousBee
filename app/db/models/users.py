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
    _apple_device_token = Column(String)
    _password_hash = Column(String)
    _group_id = Column(UUID, ForeignKey("groups._id"))

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self._name
        self._name = value

    def email(self, value: str = None) -> str:
        if value is None:
            return self._email
        self._email = value

    def appleDeviceToken(self, value: str = None) -> str:
        if value is None:
            return self._apple_device_token
        self._apple_device_token = value

    def passwordHash(self, value: str = None) -> str:
        if value is None:
            return self._password_hash
        self._password_hash = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._group_id
        self._group_id = value

    def to_schema(self):
        from app.db.schemas.users_schema import UsersSchema
        return UsersSchema.from_orm(self)
