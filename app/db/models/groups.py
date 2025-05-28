import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Groups(Base):
    __tablename__ = 'groups'

    id = Column(UUID, primary_key=True)
    name = Column(String)

    def name(self, value: str = None) -> str:
        if value is None:
            return self.name
        self.name = value

    def to_schema(self):
        from app.api.schemas.groups_schema import GroupsSchema
        return GroupsSchema.from_orm(self)
