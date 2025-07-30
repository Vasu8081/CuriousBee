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

class TagLinks(Base):
    __tablename__ = 'tag_links'

    id = Column(UUID, primary_key=True, default=uuid.uuid4)
    tag_id = Column(UUID, ForeignKey("tags.id"))
    ref_id = Column(String)
    ref_type = Column(SQLEnum(TagRefType, name='tag_ref_type'), nullable=False, default=TagRefType.Video)
    tag = relationship('Tags', uselist=False, back_populates='tagLinks', foreign_keys=[tag_id])

    @property
    def Id(self) -> uuid.UUID:
        return cast(uuid.UUID, self.id)

    @Id.setter
    def Id(self, value: Optional[uuid.UUID]) -> None:
        self.id = value

    @property
    def TagId(self) -> uuid.UUID:
        return cast(uuid.UUID, self.tag_id)

    @TagId.setter
    def TagId(self, value: Optional[uuid.UUID]) -> None:
        self.tag_id = value

    @property
    def RefId(self) -> str:
        return cast(str, self.ref_id)

    @RefId.setter
    def RefId(self, value: Optional[str]) -> None:
        self.ref_id = value

    @property
    def RefType(self) -> TagRefType:
        return cast(TagRefType, self.ref_type)

    @RefType.setter
    def RefType(self, value: Optional[TagRefType]) -> None:
        self.ref_type = value

    def to_schema(self):
        from app.autogen.schemas.tag_links_schema import TagLinksSchema
        return TagLinksSchema.model_validate(self)

    def to_dict(self):
        return self.to_schema().model_dump(by_alias=False)
