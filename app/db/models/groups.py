import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Groups(Base):
    __tablename__ = 'groups'

    _id = Column(UUID, primary_key=True)
    _name = Column(String)
    users = relationship('Users', uselist=True, back_populates='group')
    tasks = relationship('Tasks', uselist=True, back_populates='group')
    period_info = relationship('PeriodInfos', uselist=False, back_populates='group')
    period_entries = relationship('PeriodEntries', uselist=True, back_populates='group')

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self._name
        self._name = value

    def to_schema(self):
        from app.db.schemas.groups_schema import GroupsSchema
        return GroupsSchema.from_orm(self)
