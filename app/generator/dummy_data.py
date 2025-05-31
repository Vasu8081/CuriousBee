from uuid import uuid4
import datetime
from app.db.database import get_db
from app.db.autogen.models import (
    users, groups, tasks, calendar_entries,
    period_infos, period_entries, period_symptoms,
    symptoms, product_infos, product_categories,
    product_status, products, orders
)

def dummy():
    db = next(get_db())
    today = datetime.date.today()

    # GROUP
    group_id = uuid4()
    group = groups.Groups(_id=group_id, _name="Team Alpha")
    db.add(group)
    db.commit()

    # USERS
    user1 = users.Users(_id=uuid4(), _name="Alice", _email="alice@example.com",
                        _apple_device_token="token1", _password_hash=None,
                        _group_id=group_id, _is_admin=True)

    user2 = users.Users(_id=uuid4(), _name="Bob", _email="bob@example.com",
                        _apple_device_token="token2", _password_hash=None,
                        _group_id=group_id, _is_admin=False)

    db.add_all([user1, user2])
    db.commit()

    # PERIOD INFO (uses group_id as _id due to one-to-one FK)
    period_info = period_infos.PeriodInfos(
        _id=group_id,
        _average_cycle_length=28,
        _average_period_duration=5,
        _active_period_start_date=today - datetime.timedelta(days=5),
        _predicted_next_period_date=today + datetime.timedelta(days=23)
    )
    db.add(period_info)
    db.commit()

    # PERIOD ENTRIES
    period_entries_list = []
    for i in range(5):
        entry = period_entries.PeriodEntries(
            _id=uuid4(), _period_info_id=period_info._id,
            _start_date=today - datetime.timedelta(days=30 * (i + 1)),
            _end_date=today - datetime.timedelta(days=30 * i - 5),
            _notes=f"Cycle {i+1}", _is_ended=True
        )
        period_entries_list.append(entry)
    db.add_all(period_entries_list)
    db.commit()

    # SYMPTOMS
    symptoms_list = []
    for name in ["Cramps", "Headache", "Fatigue", "Back pain", "Mood swings"]:
        symptoms_list.append(symptoms.Symptoms(_id=uuid4(), _name=name))
    db.add_all(symptoms_list)
    db.commit()

    # PERIOD SYMPTOMS
    symptom_records = []
    for entry in period_entries_list:
        for s in symptoms_list[:2]:  # Add 2 symptoms per period entry
            symptom_records.append(period_symptoms.PeriodSymptoms(
                _id=uuid4(),
                _period_entry_id=entry._id,
                _symptom_id=s._id
            ))
    db.add_all(symptom_records)
    db.commit()

    # TASKS
    task_list = []
    for i in range(5):
        task_list.append(tasks.Tasks(
            _id=uuid4(),
            _title=f"Task {i+1}",
            _notes="This is a task note.",
            _deadline=datetime.datetime.now() + datetime.timedelta(days=i),
            _interaction_style="remote" if i % 2 == 0 else "in_person",
            _primary_doer_user_id=user1._id if i % 2 == 0 else user2._id,
            _other_users_presence_necessary=(i % 2 == 0),
            _is_completed=(i % 3 == 0),
            _group_id=group_id
        ))
    db.add_all(task_list)
    db.commit()

    # CALENDAR ENTRIES
    calendar_list = []
    for i in range(5):
        calendar_list.append(calendar_entries.CalendarEntries(
            _id=uuid4(),
            _user_id=user1._id if i % 2 == 0 else user2._id,
            _group_id=group_id,
            _title=f"Meeting {i+1}",
            _notes="Discuss project",
            _date=today + datetime.timedelta(days=i),
            _start_time=datetime.time(9 + i, 0),
            _end_time=datetime.time(10 + i, 0),
            _task_id=task_list[i]._id
        ))
    db.add_all(calendar_list)
    db.commit()

    # PRODUCT CATEGORIES
    skincare = product_categories.ProductCategories(_id=uuid4(), _name="Skincare")
    hygiene = product_categories.ProductCategories(_id=uuid4(), _name="Hygiene")
    db.add_all([skincare, hygiene])
    db.commit()

    # PRODUCT INFO
    prod_info_list = [
        product_infos.ProductInfos(_id=uuid4(), _group_id=group_id, _name="Sunscreen",
                                   _category_id=skincare._id, _reminder_enabled=True,
                                   _days_per_quantity=30.0, _quantity_unit_size="ml"),
        product_infos.ProductInfos(_id=uuid4(), _group_id=group_id, _name="Face Wash",
                                   _category_id=skincare._id, _reminder_enabled=True,
                                   _days_per_quantity=20.0, _quantity_unit_size="ml"),
        product_infos.ProductInfos(_id=uuid4(), _group_id=group_id, _name="Toothpaste",
                                   _category_id=hygiene._id, _reminder_enabled=True,
                                   _days_per_quantity=40.0, _quantity_unit_size="g"),
        product_infos.ProductInfos(_id=uuid4(), _group_id=group_id, _name="Shampoo",
                                   _category_id=hygiene._id, _reminder_enabled=True,
                                   _days_per_quantity=25.0, _quantity_unit_size="ml"),
        product_infos.ProductInfos(_id=uuid4(), _group_id=group_id, _name="Moisturizer",
                                   _category_id=skincare._id, _reminder_enabled=False,
                                   _days_per_quantity=50.0, _quantity_unit_size="ml")
    ]
    db.add_all(prod_info_list)
    db.commit()

    # PRODUCT STATUS
    status_list = []
    for name in ["In Use", "Ordered", "Empty", "Not Started", "Expired"]:
        status_list.append(product_status.ProductStatus(_id=uuid4(), _name=name))
    db.add_all(status_list)
    db.commit()

    # PRODUCTS
    product_list = []
    for i in range(5):
        product_list.append(products.Products(
            _id=uuid4(),
            _product_info_id=prod_info_list[i]._id,
            _quantity=100 + i * 10,
            _started_using_date=today - datetime.timedelta(days=i * 10),
            _product_completed_date=None,
            _price=100 + i * 50,
            _status_id=status_list[i]._id
        ))
    db.add_all(product_list)
    db.commit()

    # ORDERS
    order_list = []
    for i in range(5):
        order_list.append(orders.Orders(
            _id=uuid4(),
            _ordered_date=today - datetime.timedelta(days=i + 3),
            _expected_delivery_date=today + datetime.timedelta(days=7 - i),
            _received_date=None if i % 2 == 0 else today - datetime.timedelta(days=1),
            _product_id=product_list[i]._id
        ))
    db.add_all(order_list)
    db.commit()

    print("\u2705 Dummy data successfully loaded.")

if __name__ == "__main__":
    dummy()