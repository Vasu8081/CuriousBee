import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

TagsSchema = ForwardRef('TagsSchema')

class TagLinksSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    tag_id: Optional[uuid.UUID] = Field(default=None, alias='tag_id')
    ref_id: Optional[str] = Field(default=None, alias='ref_id')
    ref_type: Optional[TagRefType] = Field(default=None, alias='ref_type')

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'TagLinks':
        from app.autogen.models.tag_links import TagLinks
        model = TagLinks(
            id = self.id,
            tag_id = self.tag_id,
            ref_id = self.ref_id,
            ref_type = self.ref_type
        )
        return model
