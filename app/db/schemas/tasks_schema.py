import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

CalendarEntriesSchema = ForwardRef('CalendarEntriesSchema')
GroupsSchema = ForwardRef('GroupsSchema')
UsersSchema = ForwardRef('UsersSchema')

class TasksSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    title: Optional[str] = Field(alias='_title')
    notes: Optional[str] = Field(alias='_notes')
    deadline: Optional[datetime.datetime] = Field(alias='_deadline')
    interaction_style: Optional[str] = Field(alias='_interaction_style')
    presence_preference: Optional[str] = Field(alias='_presence_preference')
    primary_doer_user_id: Optional[uuid.UUID] = Field(alias='_primary_doer_user_id')
    is_completed: Optional[bool] = Field(alias='_is_completed')
    group_id: Optional[uuid.UUID] = Field(alias='_group_id')
    group: Optional[GroupsSchema] = None
    user: Optional[UsersSchema] = None
    calendar_entrie: Optional[CalendarEntriesSchema] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

TasksSchema.model_rebuild()
