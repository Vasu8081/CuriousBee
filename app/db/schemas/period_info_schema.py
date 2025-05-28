import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class PeriodInfoSchema(BaseModel):
    group_id: Optional[uuid.UUID]
    average_cycle_length: Optional[int]
    active_period_start_date: Optional[datetime.date]
    predicted_next_period_date: Optional[datetime.date]

    class Config:
        orm_mode = True
