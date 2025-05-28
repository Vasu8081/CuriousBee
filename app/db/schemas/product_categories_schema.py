import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductCategoriesSchema(BaseModel):
    id: Optional[uuid.UUID]
    name: Optional[str]

    class Config:
        orm_mode = True
