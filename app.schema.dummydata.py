from uuid import uuid4
import datetime
from app.db.database import get_db
from app.db.models import (
    users, groups, tasks, calendar_entries,
    period_infos, period_entries, product_infos,
    product_categories, product_status, products, orders
)

def dummy():
    db = get_db()

    # Groups
    group1 = groups.Groups(_id=uuid4(), _name="Team Alpha")
    group2 = groups.Groups(_id=uuid4(), _name="Team Beta")
    db.add_all([group1, group2])
    db.commit()

    # Users
    user1 = users.Users(_id=uuid4(), _name="Alice", _email="alice@example.com", _apple_device_token="token1",
                        _password_hash="hash1", _group_id=group1._id, _is_admin=True)
    user2 = users.Users(_id=uuid4(), _name="Bob", _email="bob@example.com", _apple_device_token="token2",
                        _password_hash="hash2", _group_id=group1._id, _is_admin=False)
    db.add_all([user1, user2])
    db.commit()

    # Tasks
    task1 = tasks.Tasks(_id=uuid4(), _title="Buy Groceries", _notes="Milk, Bread", _deadline=datetime.datetime.now() + datetime.timedelta(days=1),
                        _interaction_style="in_person", _presence_preference="together", _primary_doer_user_id=user1._id,
                        _is_completed=False, _group_id=group1._id)
    task2 = tasks.Tasks(_id=uuid4(), _title="Plan Trip", _notes="Weekend trip", _deadline=datetime.datetime.now() + datetime.timedelta(days=7),
                        _interaction_style="remote", _presence_preference="either", _primary_doer_user_id=user2._id,
                        _is_completed=False, _group_id=group1._id)
    db.add_all([task1, task2])
    db.commit()

    # Calendar Entries
    entry1 = calendar_entries.CalendarEntries(_id=uuid4(), _user_id=user1._id, _title="Meeting",
                                              _notes="Discuss goals", _date=datetime.date.today(),
                                              _start_time=datetime.time(10, 0), _end_time=datetime.time(11, 0),
                                              _task_id=task1._id)
    db.add(entry1)
    db.commit()

    # Period Info
    period_info = period_infos.PeriodInfos(_group_id=group1._id, _average_cycle_length=28,
                                           _active_period_start_date=datetime.date.today(),
                                           _predicted_next_period_date=datetime.date.today() + datetime.timedelta(days=28))
    db.add(period_info)
    db.commit()

    # Period Entries
    period_entry = period_entries.PeriodEntries(_id=uuid4(), _group_id=group1._id, _start_date=datetime.date.today() - datetime.timedelta(days=5),
                                                _end_date=datetime.date.today(), _notes="Regular", _is_ended=True)
    db.add(period_entry)
    db.commit()

    # Product Category
    cat1 = product_categories.ProductCategories(_id=uuid4(), _name="Skincare")
    db.add(cat1)
    db.commit()

    # Product Info
    prod_info = product_infos.ProductInfos(_id=uuid4(), _name="Sunscreen", _category_id=cat1._id,
                                           _reminder_enabled=True, _days_per_quantity=30.0,
                                           _quantity_unit_size="ml")
    db.add(prod_info)
    db.commit()

    # Product Status
    status = product_status.ProductStatus(_id=uuid4(), _name="In Use")
    db.add(status)
    db.commit()

    # Order
    order = orders.Orders(_id=uuid4(), _ordered_date=datetime.date.today() - datetime.timedelta(days=3),
                          _expected_delivery_date=datetime.date.today() + datetime.timedelta(days=4),
                          _received_date=None)
    db.add(order)
    db.commit()

    # Product
    product = products.Products(_id=uuid4(), _product_info_id=prod_info._id, _quantity=100.0,
                                _started_using_date=datetime.date.today(),
                                _product_completed_date=None,
                                _price=499.0, _status_id=status._id, _order_id=order._id)
    db.add(product)
    db.commit()

    print("Dummy data successfully loaded.")

if __name__ == "__main__":
    dummy()
    print("Dummy data generation complete.")