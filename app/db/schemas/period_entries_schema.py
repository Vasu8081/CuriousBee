import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

PeriodInfosSchema = ForwardRef('PeriodInfosSchema')
PeriodSymptomsSchema = ForwardRef('PeriodSymptomsSchema')

class PeriodEntriesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    period_info_id: Optional[uuid.UUID] = Field(alias='_period_info_id')
    start_date: Optional[datetime.date] = Field(alias='_start_date')
    end_date: Optional[datetime.date] = Field(alias='_end_date')
    notes: Optional[str] = Field(alias='_notes')
    is_ended: Optional[bool] = Field(alias='_is_ended')
    period_symptoms: List['PeriodSymptomsSchema'] = []

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

