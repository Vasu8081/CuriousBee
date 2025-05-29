import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

ProductCategoriesSchema = ForwardRef('ProductCategoriesSchema')
ProductsSchema = ForwardRef('ProductsSchema')

class ProductInfosSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    name: Optional[str] = Field(alias='_name')
    category_id: Optional[uuid.UUID] = Field(alias='_category_id')
    reminder_enabled: Optional[bool] = Field(alias='_reminder_enabled')
    days_per_quantity: Optional[float] = Field(alias='_days_per_quantity')
    quantity_unit_size: Optional[str] = Field(alias='_quantity_unit_size')
    products: List['ProductsSchema'] = []

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

