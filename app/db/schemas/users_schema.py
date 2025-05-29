import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional

class UsersSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    name: Optional[str] = Field(alias='_name')
    email: Optional[str] = Field(alias='_email')
    apple_device_token: Optional[str] = Field(alias='_apple_device_token')
    group_id: Optional[uuid.UUID] = Field(alias='_group_id')
    is_admin: Optional[bool] = Field(alias='_is_admin')

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )
