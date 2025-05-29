import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

OrdersSchema = ForwardRef('OrdersSchema')
ProductInfosSchema = ForwardRef('ProductInfosSchema')
ProductStatusSchema = ForwardRef('ProductStatusSchema')

class ProductsSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    product_info_id: Optional[uuid.UUID] = Field(alias='_product_info_id')
    quantity: Optional[float] = Field(alias='_quantity')
    started_using_date: Optional[datetime.date] = Field(alias='_started_using_date')
    product_completed_date: Optional[datetime.date] = Field(alias='_product_completed_date')
    price: Optional[float] = Field(alias='_price')
    status_id: Optional[uuid.UUID] = Field(alias='_status_id')
    order: Optional['OrdersSchema'] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

