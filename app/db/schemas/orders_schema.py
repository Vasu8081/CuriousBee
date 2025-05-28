import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class OrdersSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _product_info_id: Optional[uuid.UUID]
    _ordered_date: Optional[datetime.date]
    _expected_delivery_date: Optional[datetime.date]
    _received_date: Optional[datetime.date]

    class Config:
        orm_mode = True
