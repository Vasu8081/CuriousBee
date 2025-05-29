import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional, List, ForwardRef

ProductInfosSchema = ForwardRef('ProductInfosSchema')

class ProductCategoriesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    name: Optional[str] = Field(alias='_name')
    product_infos: List['ProductInfosSchema'] = []

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )

