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

class BlogVideos(Base):
    __tablename__ = 'blog_videos'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    blog_id = Column(UUID, ForeignKey("blogs.id"))
    video_id = Column(String, ForeignKey("youtube.video_id"))
    blog = relationship('Blogs', uselist=False, back_populates='blogVideos', foreign_keys=[blog_id])
    video = relationship('Youtube', uselist=False, back_populates='blogVideos', foreign_keys=[video_id])

    @property
    def Id(self) -> uuid.UUID:
        return cast(uuid.UUID, self.id)

    @Id.setter
    def Id(self, value: Optional[uuid.UUID]) -> None:
        self.id = value

    @property
    def BlogId(self) -> uuid.UUID:
        return cast(uuid.UUID, self.blog_id)

    @BlogId.setter
    def BlogId(self, value: Optional[uuid.UUID]) -> None:
        self.blog_id = value

    @property
    def VideoId(self) -> str:
        return cast(str, self.video_id)

    @VideoId.setter
    def VideoId(self, value: Optional[str]) -> None:
        self.video_id = value

    def to_schema(self):
        from app.autogen.schemas.blog_videos_schema import BlogVideosSchema
        return BlogVideosSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
