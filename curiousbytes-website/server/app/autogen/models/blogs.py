import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, TEXT, Float, Date, Time, DateTime, Boolean, LargeBinary, JSON
from app.db.base import Base
from typing import Optional, cast
from sqlalchemy import Enum as SQLEnum
from app.autogen.models.enums import *

class Blogs(Base):
    __tablename__ = 'blogs'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    title = Column(String)
    slug = Column(String)
    author_name = Column(String)
    content = Column(String)
    cover_image = Column(String)
    is_published = Column(Boolean)
    is_featured = Column(Boolean)
    published_at = Column(DateTime)
    created_at = Column(DateTime)
    updated_at = Column(DateTime)
    scheduled_at = Column(DateTime)
    blogVideos = relationship('BlogVideos', uselist=True, back_populates='blog', foreign_keys='BlogVideos.blog_id')

    @property
    def Id(self) -> uuid.UUID:
        return cast(uuid.UUID, self.id)

    @Id.setter
    def Id(self, value: Optional[uuid.UUID]) -> None:
        self.id = value

    @property
    def Title(self) -> str:
        return cast(str, self.title)

    @Title.setter
    def Title(self, value: Optional[str]) -> None:
        self.title = value

    @property
    def Slug(self) -> str:
        return cast(str, self.slug)

    @Slug.setter
    def Slug(self, value: Optional[str]) -> None:
        self.slug = value

    @property
    def AuthorName(self) -> str:
        return cast(str, self.author_name)

    @AuthorName.setter
    def AuthorName(self, value: Optional[str]) -> None:
        self.author_name = value

    @property
    def Content(self) -> str:
        return cast(str, self.content)

    @Content.setter
    def Content(self, value: Optional[str]) -> None:
        self.content = value

    @property
    def CoverImage(self) -> str:
        return cast(str, self.cover_image)

    @CoverImage.setter
    def CoverImage(self, value: Optional[str]) -> None:
        self.cover_image = value

    @property
    def IsPublished(self) -> bool:
        return cast(bool, self.is_published)

    @IsPublished.setter
    def IsPublished(self, value: Optional[bool]) -> None:
        self.is_published = value

    @property
    def IsFeatured(self) -> bool:
        return cast(bool, self.is_featured)

    @IsFeatured.setter
    def IsFeatured(self, value: Optional[bool]) -> None:
        self.is_featured = value

    @property
    def PublishedAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.published_at)

    @PublishedAt.setter
    def PublishedAt(self, value: Optional[datetime.datetime]) -> None:
        self.published_at = value

    @property
    def CreatedAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.created_at)

    @CreatedAt.setter
    def CreatedAt(self, value: Optional[datetime.datetime]) -> None:
        self.created_at = value

    @property
    def UpdatedAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.updated_at)

    @UpdatedAt.setter
    def UpdatedAt(self, value: Optional[datetime.datetime]) -> None:
        self.updated_at = value

    @property
    def ScheduledAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.scheduled_at)

    @ScheduledAt.setter
    def ScheduledAt(self, value: Optional[datetime.datetime]) -> None:
        self.scheduled_at = value

    def to_schema(self):
        from app.autogen.schemas.blogs_schema import BlogsSchema
        return BlogsSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
