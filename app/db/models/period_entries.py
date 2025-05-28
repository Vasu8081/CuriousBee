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
            return self._id
        self._id = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._group_id
        self._group_id = value

    def startDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self._start_date
        self._start_date = value

    def endDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self._end_date
        self._end_date = value

    def notes(self, value: str = None) -> str:
        if value is None:
            return self._notes
        self._notes = value

    def isEnded(self, value: bool = None) -> bool:
        if value is None:
            return self._is_ended
        self._is_ended = value

    def to_schema(self):
        from app.db.schemas.period_entries_schema import PeriodEntriesSchema
        return PeriodEntriesSchema.from_orm(self)
