import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

BlogsSchema = ForwardRef('BlogsSchema')
YoutubeSchema = ForwardRef('YoutubeSchema')

class BlogVideosSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    blog_id: Optional[uuid.UUID] = Field(default=None, alias='blog_id')
    video_id: Optional[str] = Field(default=None, alias='video_id')

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'BlogVideos':
        from app.autogen.models.blog_videos import BlogVideos
        model = BlogVideos(
            id = self.id,
            blog_id = self.blog_id,
            video_id = self.video_id
        )
        return model
