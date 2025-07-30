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

class Resources(Base):
    __tablename__ = 'resources'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    title = Column(String)
    url = Column(String)
    description = Column(String)
    type = Column(String)
    is_featured = Column(Boolean)
    created_at = Column(DateTime)
    scheduled_at = Column(DateTime)
    resourceVideos = relationship('ResourceVideos', uselist=True, back_populates='resource', foreign_keys='ResourceVideos.resource_id')

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
    def Url(self) -> str:
        return cast(str, self.url)

    @Url.setter
    def Url(self, value: Optional[str]) -> None:
        self.url = value

    @property
    def Description(self) -> str:
        return cast(str, self.description)

    @Description.setter
    def Description(self, value: Optional[str]) -> None:
        self.description = value

    @property
    def Type(self) -> str:
        return cast(str, self.type)

    @Type.setter
    def Type(self, value: Optional[str]) -> None:
        self.type = value

    @property
    def IsFeatured(self) -> bool:
        return cast(bool, self.is_featured)

    @IsFeatured.setter
    def IsFeatured(self, value: Optional[bool]) -> None:
        self.is_featured = value

    @property
    def CreatedAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.created_at)

    @CreatedAt.setter
    def CreatedAt(self, value: Optional[datetime.datetime]) -> None:
        self.created_at = value

    @property
    def ScheduledAt(self) -> datetime.datetime:
        return cast(datetime.datetime, self.scheduled_at)

    @ScheduledAt.setter
    def ScheduledAt(self, value: Optional[datetime.datetime]) -> None:
        self.scheduled_at = value

    def to_schema(self):
        from app.autogen.schemas.resources_schema import ResourcesSchema
        return ResourcesSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
