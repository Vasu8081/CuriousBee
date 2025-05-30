from pydantic import BaseModel, Field, ConfigDict
import uuid

class TestSchema(BaseModel):
    id: uuid.UUID = Field(alias='_id')
    model_config = ConfigDict(from_attributes=True, populate_by_name=True)

class Dummy:
    def __init__(self):
        self._id = uuid.uuid4()

print(TestSchema.model_validate(Dummy()))