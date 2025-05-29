import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

ProductsSchema = ForwardRef('ProductsSchema')

class OrdersSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    ordered_date: Optional[datetime.date] = Field(alias='_ordered_date')
    expected_delivery_date: Optional[datetime.date] = Field(alias='_expected_delivery_date')
    received_date: Optional[datetime.date] = Field(alias='_received_date')
    product: Optional[ProductsSchema] = None

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

OrdersSchema.model_rebuild()
