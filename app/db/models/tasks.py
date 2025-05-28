import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Tasks(Base):
    __tablename__ = 'tasks'

    _id = Column(UUID, primary_key=True)
    _title = Column(String)
    _notes = Column(String)
    _deadline = Column(DateTime)
    _interaction_style = Column(String)
    _presence_preference = Column(String)
    _primary_doer_user_id = Column(UUID, ForeignKey("users._id"))
    _is_completed = Column(Boolean)
    _group_id = Column(UUID, ForeignKey("groups._id"))
    _calendar_entry_id = Column(UUID, ForeignKey("calendar_entries._id", use_alter=True))

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.id
        self.id = value

    def title(self, value: str = None) -> str:
        if value is None:
            return self.title
        self.title = value

    def notes(self, value: str = None) -> str:
        if value is None:
            return self.notes
        self.notes = value

    def deadline(self, value: datetime.datetime = None) -> datetime.datetime:
        if value is None:
            return self.deadline
        self.deadline = value

    def interactionStyle(self, value: str = None) -> str:
        if value is None:
            return self.interaction_style
        self.interaction_style = value

    def presencePreference(self, value: str = None) -> str:
        if value is None:
            return self.presence_preference
        self.presence_preference = value

    def primaryDoerUserId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.primary_doer_user_id
        self.primary_doer_user_id = value

    def isCompleted(self, value: bool = None) -> bool:
        if value is None:
            return self.is_completed
        self.is_completed = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.group_id
        self.group_id = value

    def calendarEntryId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.calendar_entry_id
        self.calendar_entry_id = value

    def to_schema(self):
        from app.db.schemas.tasks_schema import TasksSchema
        return TasksSchema.from_orm(self)
