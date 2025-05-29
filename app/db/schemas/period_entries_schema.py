import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

GroupsSchema = ForwardRef('GroupsSchema')

class PeriodEntriesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    group_id: Optional[uuid.UUID] = Field(alias='_group_id')
    start_date: Optional[datetime.date] = Field(alias='_start_date')
    end_date: Optional[datetime.date] = Field(alias='_end_date')
    notes: Optional[str] = Field(alias='_notes')
    is_ended: Optional[bool] = Field(alias='_is_ended')
    group: Optional['GroupsSchema'] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

