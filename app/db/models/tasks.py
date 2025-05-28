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

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def title(self, value: str = None) -> str:
        if value is None:
            return self._title
        self._title = value

    def notes(self, value: str = None) -> str:
        if value is None:
            return self._notes
        self._notes = value

    def deadline(self, value: datetime.datetime = None) -> datetime.datetime:
        if value is None:
            return self._deadline
        self._deadline = value

    def interactionStyle(self, value: str = None) -> str:
        if value is None:
            return self._interaction_style
        self._interaction_style = value

    def presencePreference(self, value: str = None) -> str:
        if value is None:
            return self._presence_preference
        self._presence_preference = value

    def primaryDoerUserId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._primary_doer_user_id
        self._primary_doer_user_id = value

    def isCompleted(self, value: bool = None) -> bool:
        if value is None:
            return self._is_completed
        self._is_completed = value

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._group_id
        self._group_id = value

    def to_schema(self):
        from app.db.schemas.tasks_schema import TasksSchema
        return TasksSchema.from_orm(self)
