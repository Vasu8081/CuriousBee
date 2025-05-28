import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class PeriodEntries(Base):
    __tablename__ = 'period_entries'

    _id = Column(UUID, primary_key=True)
    _group_id = Column(UUID, ForeignKey("groups._id"))
    _start_date = Column(Date)
    _end_date = Column(Date)
    _notes = Column(String)
    _is_ended = Column(Boolean)

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.id
        self.id = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.group_id
        self.group_id = value

    def startDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.start_date
        self.start_date = value

    def endDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.end_date
        self.end_date = value

    def notes(self, value: str = None) -> str:
        if value is None:
            return self.notes
        self.notes = value

    def isEnded(self, value: bool = None) -> bool:
        if value is None:
            return self.is_ended
        self.is_ended = value

    def to_schema(self):
        from app.db.schemas.period_entries_schema import PeriodEntriesSchema
        return PeriodEntriesSchema.from_orm(self)
