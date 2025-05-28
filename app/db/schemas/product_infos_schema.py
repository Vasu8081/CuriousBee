import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductInfosSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _name: Optional[str]
    _category_id: Optional[uuid.UUID]
    _reminder_enabled: Optional[bool]
    _days_per_quantity: Optional[float]
    _quantity_unit_size: Optional[str]

    class Config:
        orm_mode = True
