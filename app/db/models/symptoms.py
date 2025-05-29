import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Symptoms(Base):
    __tablename__ = 'symptoms'

    _symptom_id = Column(UUID, primary_key=True)
    _name = Column(String)
    period_symptoms = relationship('PeriodSymptoms', uselist=True, back_populates='symptom')

    def symptomId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._symptom_id
        self._symptom_id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self._name
        self._name = value

    def to_schema(self):
        from app.db.schemas.symptoms_schema import SymptomsSchema
        return SymptomsSchema.from_orm(self)
