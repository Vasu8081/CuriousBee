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

    resource_id = Column(UUID, primary_key=True, default=uuid.uuid4)
    url = Column(String)

    @property
    def ResourceId(self) -> uuid.UUID:
        return cast(uuid.UUID, self.resource_id)

    @ResourceId.setter
    def ResourceId(self, value: Optional[uuid.UUID]) -> None:
        self.resource_id = value

    @property
    def Url(self) -> str:
        return cast(str, self.url)

    @Url.setter
    def Url(self, value: Optional[str]) -> None:
        self.url = value

    def to_schema(self):
        from app.autogen.schemas.resources_schema import ResourcesSchema
        return ResourcesSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
