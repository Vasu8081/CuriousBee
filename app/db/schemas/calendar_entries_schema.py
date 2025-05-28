import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class CalendarEntriesSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _user_id: Optional[uuid.UUID]
    _title: Optional[str]
    _notes: Optional[str]
    _date: Optional[datetime.date]
    _start_time: Optional[datetime.time]
    _end_time: Optional[datetime.time]
    _task_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
