import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductStatusSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _name: Optional[str]

    class Config:
        orm_mode = True
