import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

BlogVideosSchema = ForwardRef('BlogVideosSchema')

class BlogsSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    title: Optional[str] = Field(default=None, alias='title')
    slug: Optional[str] = Field(default=None, alias='slug')
    author_name: Optional[str] = Field(default=None, alias='author_name')
    content: Optional[str] = Field(default=None, alias='content')
    cover_image: Optional[str] = Field(default=None, alias='cover_image')
    is_published: Optional[bool] = Field(default=None, alias='is_published')
    is_featured: Optional[bool] = Field(default=None, alias='is_featured')
    published_at: Optional[datetime.datetime] = Field(default=None, alias='published_at')
    created_at: Optional[datetime.datetime] = Field(default=None, alias='created_at')
    updated_at: Optional[datetime.datetime] = Field(default=None, alias='updated_at')
    scheduled_at: Optional[datetime.datetime] = Field(default=None, alias='scheduled_at')
    blogVideos: List['BlogVideosSchema'] = []

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'Blogs':
        from app.autogen.models.blogs import Blogs
        model = Blogs(
            id = self.id,
            title = self.title,
            slug = self.slug,
            author_name = self.author_name,
            content = self.content,
            cover_image = self.cover_image,
            is_published = self.is_published,
            is_featured = self.is_featured,
            published_at = self.published_at,
            created_at = self.created_at,
            updated_at = self.updated_at,
            scheduled_at = self.scheduled_at
        )
        from app.autogen.models.blog_videos import BlogVideos
        if self.blogVideos is not None:
            model.blogVideos = [obj.to_model() for obj in self.blogVideos]
        return model
