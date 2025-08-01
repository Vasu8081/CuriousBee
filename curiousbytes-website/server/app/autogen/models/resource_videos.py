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

class ResourceVideos(Base):
    __tablename__ = 'resource_videos'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    resource_id = Column(UUID, ForeignKey("resources.id"))
    video_id = Column(String, ForeignKey("youtube.video_id"))
    resource = relationship('Resources', uselist=False, back_populates='resourceVideos', foreign_keys=[resource_id])
    video = relationship('Youtube', uselist=False, back_populates='resourceVideos', foreign_keys=[video_id])

    @property
    def Id(self) -> uuid.UUID:
        return cast(uuid.UUID, self.id)

    @Id.setter
    def Id(self, value: Optional[uuid.UUID]) -> None:
        self.id = value

    @property
    def ResourceId(self) -> uuid.UUID:
        return cast(uuid.UUID, self.resource_id)

    @ResourceId.setter
    def ResourceId(self, value: Optional[uuid.UUID]) -> None:
        self.resource_id = value

    @property
    def VideoId(self) -> str:
        return cast(str, self.video_id)

    @VideoId.setter
    def VideoId(self, value: Optional[str]) -> None:
        self.video_id = value

    def to_schema(self):
        from app.autogen.schemas.resource_videos_schema import ResourceVideosSchema
        return ResourceVideosSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
