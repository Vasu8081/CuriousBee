import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class TasksSchema(BaseModel):
    id: Optional[uuid.UUID]
    title: Optional[str]
    notes: Optional[str]
    deadline: Optional[datetime.datetime]
    interaction_style: Optional[str]
    presence_preference: Optional[str]
    primary_doer_user_id: Optional[uuid.UUID]
    is_completed: Optional[bool]
    group_id: Optional[uuid.UUID]
    calendar_entry_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
