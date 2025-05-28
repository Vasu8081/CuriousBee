import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductInfosSchema(BaseModel):
    id: Optional[uuid.UUID]
    name: Optional[str]
    category_id: Optional[uuid.UUID]
    reminder_enabled: Optional[bool]
    days_per_quantity: Optional[float]
    quantity_unit_size: Optional[str]

    class Config:
        orm_mode = True
