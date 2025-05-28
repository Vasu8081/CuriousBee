import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class ProductCategories(Base):
    __tablename__ = 'product_categories'

    id = Column(UUID, primary_key=True)
    name = Column(String)

    def name(self, value: str = None) -> str:
        if value is None:
            return self.name
        self.name = value

    def to_schema(self):
        from app.api.schemas.product_categories_schema import ProductCategoriesSchema
        return ProductCategoriesSchema.from_orm(self)
