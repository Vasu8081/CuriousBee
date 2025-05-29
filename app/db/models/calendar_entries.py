import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class CalendarEntries(Base):
    __tablename__ = 'calendar_entries'

    _id = Column(UUID, primary_key=True)
    _user_id = Column(UUID, ForeignKey("users._id"))
    _title = Column(String)
    _notes = Column(String)
    _date = Column(Date)
    _start_time = Column(Time)
    _end_time = Column(Time)
    _task_id = Column(UUID, ForeignKey("tasks._id"))
    user = relationship('Users', uselist=False, back_populates='calendar_entries')
    task = relationship('Tasks', uselist=False, back_populates='calendar_entrie')

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def userId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._user_id
        self._user_id = value

    def title(self, value: str = None) -> str:
        if value is None:
            return self._title
        self._title = value

    def notes(self, value: str = None) -> str:
        if value is None:
            return self._notes
        self._notes = value

    def date(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self._date
        self._date = value

    def startTime(self, value: datetime.time = None) -> datetime.time:
        if value is None:
            return self._start_time
        self._start_time = value

    def endTime(self, value: datetime.time = None) -> datetime.time:
        if value is None:
            return self._end_time
        self._end_time = value

    def taskId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._task_id
        self._task_id = value

    def to_schema(self):
        from app.db.schemas.calendar_entries_schema import CalendarEntriesSchema
        return CalendarEntriesSchema.from_orm(self)
