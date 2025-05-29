import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from typing import Optional

class ProductCategoriesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(alias='_id')
    name: Optional[str] = Field(alias='_name')

    model_config = ConfigDict(
        from_attributes=True,
        populate_by_name=True
    )
