import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class ProductInfos(Base):
    __tablename__ = 'product_infos'

    id = Column(UUID, primary_key=True)
    name = Column(String)
    category_id = Column(UUID, ForeignKey("product_categories.id"))
    reminder_enabled = Column(Boolean)
    days_per_quantity = Column(Float)
    quantity_unit_size = Column(String)

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
        from app.api.schemas.product_infos_schema import ProductInfosSchema
        return ProductInfosSchema.from_orm(self)
