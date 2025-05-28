import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductsSchema(BaseModel):
    id: Optional[uuid.UUID]
    product_info_id: Optional[uuid.UUID]
    quantity: Optional[float]
    started_using_date: Optional[datetime.date]
    product_completed_date: Optional[datetime.date]
    price: Optional[float]
    status_id: Optional[uuid.UUID]
    order_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
