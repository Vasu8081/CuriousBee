import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class PeriodEntriesSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _group_id: Optional[uuid.UUID]
    _start_date: Optional[datetime.date]
    _end_date: Optional[datetime.date]
    _notes: Optional[str]
    _is_ended: Optional[bool]

    class Config:
        orm_mode = True
