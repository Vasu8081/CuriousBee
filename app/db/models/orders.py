import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Orders(Base):
    __tablename__ = 'orders'

    _id = Column(UUID, primary_key=True)
    _product_id = Column(UUID, ForeignKey("products._id", use_alter=True))
    _ordered_date = Column(Date)
    _expected_delivery_date = Column(Date)
    _received_date = Column(Date)

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.id
        self.id = value

    def productId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.product_id
        self.product_id = value

    def orderedDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.ordered_date
        self.ordered_date = value

    def expectedDeliveryDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.expected_delivery_date
        self.expected_delivery_date = value

    def receivedDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self.received_date
        self.received_date = value

    def to_schema(self):
        from app.db.schemas.orders_schema import OrdersSchema
        return OrdersSchema.from_orm(self)
