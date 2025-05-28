import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class ProductInfos(Base):
    __tablename__ = 'product_infos'

    _id = Column(UUID, primary_key=True)
    _name = Column(String)
    _category_id = Column(UUID, ForeignKey("product_categories._id"))
    _reminder_enabled = Column(Boolean)
    _days_per_quantity = Column(Float)
    _quantity_unit_size = Column(String)

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.id
        self.id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self.name
        self.name = value

    def categoryId(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self.category_id
        self.category_id = value

    def reminderEnabled(self, value: bool = None) -> bool:
        if value is None:
            return self.reminder_enabled
        self.reminder_enabled = value

    def daysPerQuantity(self, value: float = None) -> float:
        if value is None:
            return self.days_per_quantity
        self.days_per_quantity = value

    def quantityUnitSize(self, value: str = None) -> str:
        if value is None:
            return self.quantity_unit_size
        self.quantity_unit_size = value

    def to_schema(self):
        from app.db.schemas.product_infos_schema import ProductInfosSchema
        return ProductInfosSchema.from_orm(self)
