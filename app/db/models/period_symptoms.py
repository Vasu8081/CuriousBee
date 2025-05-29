import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class PeriodSymptoms(Base):
    __tablename__ = 'period_symptoms'

    _id = Column(UUID, primary_key=True)
    _period_entry_id = Column(UUID, ForeignKey("period_entries._id"))
    _symptom_id = Column(UUID, ForeignKey("symptoms._symptom_id"))
    period_entry = relationship('PeriodEntries', uselist=False, back_populates='period_symptoms')
    symptom = relationship('Symptoms', uselist=False, back_populates='period_symptoms')

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def periodEntryId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._period_entry_id
        self._period_entry_id = value

    def symptomId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._symptom_id
        self._symptom_id = value

    def to_schema(self):
        from app.db.schemas.period_symptoms_schema import PeriodSymptomsSchema
        return PeriodSymptomsSchema.from_orm(self)
