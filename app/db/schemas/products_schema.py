import uuid
import datetime
from pydantic import BaseModel
from typing import Optional

class ProductsSchema(BaseModel):
    _id: Optional[uuid.UUID]
    _product_info_id: Optional[uuid.UUID]
    _quantity: Optional[float]
    _started_using_date: Optional[datetime.date]
    _product_completed_date: Optional[datetime.date]
    _price: Optional[float]
    _status_id: Optional[uuid.UUID]
    _order_id: Optional[uuid.UUID]

    class Config:
        orm_mode = True
