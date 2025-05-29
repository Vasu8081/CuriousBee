import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional

class PeriodInfoSchema(BaseModel):
    group_id: Optional[uuid.UUID] = Field(alias='_group_id')
    average_cycle_length: Optional[int] = Field(alias='_average_cycle_length')
    active_period_start_date: Optional[datetime.date] = Field(alias='_active_period_start_date')
    predicted_next_period_date: Optional[datetime.date] = Field(alias='_predicted_next_period_date')

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )
