import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef


class ResourcesSchema(BaseModel):
    resource_id: Optional[uuid.UUID] = Field(default=None, alias='resource_id')
    url: Optional[str] = Field(default=None, alias='url')

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'Resources':
        from app.autogen.models.resources import Resources
        model = Resources(
            resource_id = self.resource_id,
            url = self.url
        )
        return model
