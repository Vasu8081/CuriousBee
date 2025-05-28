import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class OrdersSchema(BaseModel):
    id: Optional[uuid.UUID]
    product_info_id: Optional[uuid.UUID]
    ordered_date: Optional[datetime.date]
    expected_delivery_date: Optional[datetime.date]
    received_date: Optional[datetime.date]

    class Config:
        orm_mode = True
