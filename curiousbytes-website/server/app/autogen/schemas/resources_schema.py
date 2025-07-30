import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

ResourceVideosSchema = ForwardRef('ResourceVideosSchema')

class ResourcesSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    title: Optional[str] = Field(default=None, alias='title')
    url: Optional[str] = Field(default=None, alias='url')
    description: Optional[str] = Field(default=None, alias='description')
    type: Optional[str] = Field(default=None, alias='type')
    is_featured: Optional[bool] = Field(default=None, alias='is_featured')
    created_at: Optional[datetime.datetime] = Field(default=None, alias='created_at')
    scheduled_at: Optional[datetime.datetime] = Field(default=None, alias='scheduled_at')
    resourceVideos: List['ResourceVideosSchema'] = []

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'Resources':
        from app.autogen.models.resources import Resources
        model = Resources(
            id = self.id,
            title = self.title,
            url = self.url,
            description = self.description,
            type = self.type,
            is_featured = self.is_featured,
            created_at = self.created_at,
            scheduled_at = self.scheduled_at
        )
        from app.autogen.models.resource_videos import ResourceVideos
        if self.resourceVideos is not None:
            model.resourceVideos = [obj.to_model() for obj in self.resourceVideos]
        return model
