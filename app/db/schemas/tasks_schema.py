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
    primary_doer_user_id: Optional[uuid.UUID] = Field(alias='_primary_doer_user_id')
    other_users_presence_necessary: Optional[bool] = Field(alias='_other_users_presence_necessary')
    is_completed: Optional[bool] = Field(alias='_is_completed')
    group_id: Optional[uuid.UUID] = Field(alias='_group_id')
    calendar_entrie: Optional['CalendarEntriesSchema'] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

