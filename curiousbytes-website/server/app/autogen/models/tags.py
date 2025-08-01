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

class Tags(Base):
    __tablename__ = 'tags'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    name = Column(String)
    tagLinks = relationship('TagLinks', uselist=True, back_populates='tag', foreign_keys='TagLinks.tag_id')

    @property
    def Id(self) -> uuid.UUID:
        return cast(uuid.UUID, self.id)

    @Id.setter
    def Id(self, value: Optional[uuid.UUID]) -> None:
        self.id = value

    @property
    def Name(self) -> str:
        return cast(str, self.name)

    @Name.setter
    def Name(self, value: Optional[str]) -> None:
        self.name = value

    def to_schema(self):
        from app.autogen.schemas.tags_schema import TagsSchema
        return TagsSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
