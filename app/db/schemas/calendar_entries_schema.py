import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class CalendarEntriesSchema(BaseModel):
    id: Optional[uuid.UUID]
    user_id: Optional[uuid.UUID]
    title: Optional[str]
    notes: Optional[str]
    date: Optional[datetime.date]
    start_time: Optional[datetime.time]
    end_time: Optional[datetime.time]
    task_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
