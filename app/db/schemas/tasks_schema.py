import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class TasksSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _title: Optional[str]
    _notes: Optional[str]
    _deadline: Optional[datetime.datetime]
    _interaction_style: Optional[str]
    _presence_preference: Optional[str]
    _primary_doer_user_id: Optional[uuid.UUID]
    _is_completed: Optional[bool]
    _group_id: Optional[uuid.UUID]
    _calendar_entry_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
