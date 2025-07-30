import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

BlogVideosSchema = ForwardRef('BlogVideosSchema')
ResourceVideosSchema = ForwardRef('ResourceVideosSchema')

class YoutubeSchema(BaseModel):
    video_id: Optional[str] = Field(default=None, alias='video_id')
    title: Optional[str] = Field(default=None, alias='title')
    description: Optional[str] = Field(default=None, alias='description')
    published_at: Optional[datetime.datetime] = Field(default=None, alias='published_at')
    thumbnails: Optional[dict] = Field(default=None, alias='thumbnails')
    localized: Optional[dict] = Field(default=None, alias='localized')
    channel_id: Optional[str] = Field(default=None, alias='channel_id')
    channel_title: Optional[str] = Field(default=None, alias='channel_title')
    raw_snippet: Optional[dict] = Field(default=None, alias='raw_snippet')
    is_video: Optional[bool] = Field(default=None, alias='is_video')
    video_type: Optional[VideoTypes] = Field(default=None, alias='video_type')
    blogVideos: List['BlogVideosSchema'] = []
    resourceVideos: List['ResourceVideosSchema'] = []

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'Youtube':
        from app.autogen.models.youtube import Youtube
        model = Youtube(
            video_id = self.video_id,
            title = self.title,
            description = self.description,
            published_at = self.published_at,
            thumbnails = self.thumbnails,
            localized = self.localized,
            channel_id = self.channel_id,
            channel_title = self.channel_title,
            raw_snippet = self.raw_snippet,
            is_video = self.is_video,
            video_type = self.video_type
        )
        from app.autogen.models.blog_videos import BlogVideos
        if self.blogVideos is not None:
            model.blogVideos = [obj.to_model() for obj in self.blogVideos]
        from app.autogen.models.resource_videos import ResourceVideos
        if self.resourceVideos is not None:
            model.resourceVideos = [obj.to_model() for obj in self.resourceVideos]
        return model
