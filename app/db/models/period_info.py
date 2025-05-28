import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class PeriodInfo(Base):
    __tablename__ = 'period_info'

    group_id = Column(UUID, ForeignKey("groups.id"), primary_key=True)
    average_cycle_length = Column(Integer)
    active_period_start_date = Column(Date)
    predicted_next_period_date = Column(Date)

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
