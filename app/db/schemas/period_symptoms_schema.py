import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

PeriodEntriesSchema = ForwardRef('PeriodEntriesSchema')
SymptomsSchema = ForwardRef('SymptomsSchema')

class PeriodSymptomsSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    period_entry_id: Optional[uuid.UUID] = Field(alias='_period_entry_id')
    symptom_id: Optional[uuid.UUID] = Field(alias='_symptom_id')
    _symptom_id: Optional['SymptomsSchema'] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

