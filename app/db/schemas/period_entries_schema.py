import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class PeriodEntriesSchema(BaseModel):
    id: Optional[uuid.UUID]
    group_id: Optional[uuid.UUID]
    start_date: Optional[datetime.date]
    end_date: Optional[datetime.date]
    notes: Optional[str]
    is_ended: Optional[bool]

    class Config:
        orm_mode = True
