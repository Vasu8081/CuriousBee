import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class UsersSchema(BaseModel):
    id: Optional[uuid.UUID]
    name: Optional[str]
    email: Optional[str]
    group_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
