import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class Products(Base):
    __tablename__ = 'products'

    _id = Column(UUID, primary_key=True)
    _product_info_id = Column(UUID, ForeignKey("product_infos._id"))
    _quantity = Column(Float)
    _started_using_date = Column(Date)
    _product_completed_date = Column(Date)
    _price = Column(Float)
    _status_id = Column(UUID, ForeignKey("product_status._id"))
    _order_id = Column(UUID, ForeignKey("orders._id", use_alter=True))

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def productInfoId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._product_info_id
        self._product_info_id = value

    def quantity(self, value: float = None) -> float:
        if value is None:
            return self._quantity
        self._quantity = value

    def startedUsingDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self._started_using_date
        self._started_using_date = value

    def productCompletedDate(self, value: datetime.date = None) -> datetime.date:
        if value is None:
            return self._product_completed_date
        self._product_completed_date = value

    def price(self, value: float = None) -> float:
        if value is None:
            return self._price
        self._price = value

    def statusId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._status_id
        self._status_id = value

    def orderId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._order_id
        self._order_id = value

    def to_schema(self):
        from app.db.schemas.products_schema import ProductsSchema
        return ProductsSchema.from_orm(self)
