import uuid
import datetime
from pydantic import BaseModel, Field, ConfigDict
from app.autogen.models.enums import *
from typing import Optional, List, ForwardRef

TagLinksSchema = ForwardRef('TagLinksSchema')

class TagsSchema(BaseModel):
    id: Optional[uuid.UUID] = Field(default=None, alias='id')
    name: Optional[str] = Field(default=None, alias='name')
    tagLinks: List['TagLinksSchema'] = []

    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

    def to_model(self) -> 'Tags':
        from app.autogen.models.tags import Tags
        model = Tags(
            id = self.id,
            name = self.name
        )
        from app.autogen.models.tag_links import TagLinks
        if self.tagLinks is not None:
            model.tagLinks = [obj.to_model() for obj in self.tagLinks]
        return model
