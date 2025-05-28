import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class PeriodInfoSchema(BaseModel):
    _group_id: Optional[uuid.UUID]
    _average_cycle_length: Optional[int]
    _active_period_start_date: Optional[datetime.date]
    _predicted_next_period_date: Optional[datetime.date]

    class Config:
        orm_mode = True
