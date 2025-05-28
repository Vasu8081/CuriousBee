import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class PeriodInfo(Base):
    __tablename__ = 'period_info'

    _group_id = Column(UUID, ForeignKey("groups._id"), primary_key=True)
    _average_cycle_length = Column(Integer)
    _active_period_start_date = Column(Date)
    _predicted_next_period_date = Column(Date)

    def groupId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.group_id
        self.group_id = value

    def averageCycleLength(self, value: int = None) -> int:
        if value is None:
            return self.average_cycle_length
        self.average_cycle_length = value

    def activePeriodStartDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.active_period_start_date
        self.active_period_start_date = value

    def predictedNextPeriodDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.predicted_next_period_date
        self.predicted_next_period_date = value

    def to_schema(self):
        from app.api.schemas.period_info_schema import PeriodInfoSchema
        return PeriodInfoSchema.from_orm(self)
