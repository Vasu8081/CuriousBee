from .groups_schema import GroupsSchema
from .users_schema import UsersSchema
from .tasks_schema import TasksSchema
from .calendar_entries_schema import CalendarEntriesSchema
from .period_infos_schema import PeriodInfosSchema
from .period_entries_schema import PeriodEntriesSchema
from .product_infos_schema import ProductInfosSchema
from .product_categories_schema import ProductCategoriesSchema
from .product_status_schema import ProductStatusSchema
from .products_schema import ProductsSchema
from .orders_schema import OrdersSchema
GroupsSchema.model_rebuild(_types_namespace=globals())
UsersSchema.model_rebuild(_types_namespace=globals())
TasksSchema.model_rebuild(_types_namespace=globals())
CalendarEntriesSchema.model_rebuild(_types_namespace=globals())
PeriodInfosSchema.model_rebuild(_types_namespace=globals())
PeriodEntriesSchema.model_rebuild(_types_namespace=globals())
ProductInfosSchema.model_rebuild(_types_namespace=globals())
ProductCategoriesSchema.model_rebuild(_types_namespace=globals())
ProductStatusSchema.model_rebuild(_types_namespace=globals())
ProductsSchema.model_rebuild(_types_namespace=globals())
OrdersSchema.model_rebuild(_types_namespace=globals())
