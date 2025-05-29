import uuid
import datetime
from sqlalchemy import Column, ForeignKey
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import UUID
from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean
from app.db.base import Base

class ProductCategories(Base):
    __tablename__ = 'product_categories'

    _id = Column(UUID, primary_key=True)
    _name = Column(String)
    product_infos = relationship('ProductInfos', uselist=True, back_populates='category')

    def id(self, value: uuid.UUID = None) -> uuid.UUID:
        if value is None:
            return self._id
        self._id = value

    def name(self, value: str = None) -> str:
        if value is None:
            return self._name
        self._name = value

    def to_schema(self):
        from app.db.schemas.product_categories_schema import ProductCategoriesSchema
        return ProductCategoriesSchema.from_orm(self)
