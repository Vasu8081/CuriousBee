import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

TasksSchema = ForwardRef('TasksSchema')
UsersSchema = ForwardRef('UsersSchema')

class CalendarEntriesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    user_id: Optional[uuid.UUID] = Field(alias='_user_id')
    title: Optional[str] = Field(alias='_title')
    notes: Optional[str] = Field(alias='_notes')
    date: Optional[datetime.date] = Field(alias='_date')
    start_time: Optional[datetime.time] = Field(alias='_start_time')
    end_time: Optional[datetime.time] = Field(alias='_end_time')
    task_id: Optional[uuid.UUID] = Field(alias='_task_id')
    user: Optional[UsersSchema] = None
    task: Optional[TasksSchema] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

CalendarEntriesSchema.model_rebuild()
