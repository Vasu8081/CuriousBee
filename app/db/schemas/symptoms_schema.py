import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

PeriodSymptomsSchema = ForwardRef('PeriodSymptomsSchema')

class SymptomsSchema(BaseModel):
    symptom_id: Optional[uuid.UUID] = Field(alias='_symptom_id')
    name: Optional[str] = Field(alias='_name')

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

