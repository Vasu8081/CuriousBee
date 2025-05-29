import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

PeriodInfosSchema = ForwardRef('PeriodInfosSchema')
ProductInfosSchema = ForwardRef('ProductInfosSchema')
TasksSchema = ForwardRef('TasksSchema')
UsersSchema = ForwardRef('UsersSchema')

class GroupsSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    name: Optional[str] = Field(alias='_name')
    users: List['UsersSchema'] = []
    tasks: List['TasksSchema'] = []
    period_info: Optional['PeriodInfosSchema'] = None
    product_infos: List['ProductInfosSchema'] = []

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

