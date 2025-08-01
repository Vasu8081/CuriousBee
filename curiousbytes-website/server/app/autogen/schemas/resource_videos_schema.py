import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

ResourcesSchema = ForwardRef('ResourcesSchema')
YoutubeSchema = ForwardRef('YoutubeSchema')

class ResourceVideosSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    resource_id: Optional[uuid.UUID] = Field(default=None, alias='resource_id')
    video_id: Optional[str] = Field(default=None, alias='video_id')

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'ResourceVideos':
        from app.autogen.models.resource_videos import ResourceVideos
        model = ResourceVideos(
            id = self.id,
            resource_id = self.resource_id,
            video_id = self.video_id
        )
        return model
