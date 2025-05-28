import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class UsersSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _name: Optional[str]
    _email: Optional[str]
    _apple_device_token: Optional[str]
    _password_hash: Optional[str]
    _group_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
