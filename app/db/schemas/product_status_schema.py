import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductStatusSchema(BaseModel):
    id: Optional[uuid.UUID]
    name: Optional[str]

    class Config:
        orm_mode = True
