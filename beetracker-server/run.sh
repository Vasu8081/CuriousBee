#!/bin/sh

echo "🔄 Checking database connection..."
python -m app.utils.check_db_connection
python -c "import os; import sqlalchemy; sqlalchemy.create_engine(os.environ['DATABASE_URL'])"
echo "✅ Database connection verified."
echo "🔄 Setting up database..."
echo "🔄 Running database migrations..."
alembic -c alembic.ini revision --autogenerate -m "Initial migration"
echo "✅ Migration script created."
echo "🔄 Applying database migrations..."
alembic -c alembic.ini upgrade head
echo "✅ Database migration complete."

if [ "$LOAD_TEST_DATA" = "true" ]; then
    echo "📊 Loading dummy data..."
    python -m app.utils.load_dummy_data
    echo "✅ Dummy data loaded successfully."
else
    echo "🚫 Skipping dummy data loading."
fi

uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
echo "🚀 Starting BeeTracker Server with Uvicorn..."

# Prevent container from exiting
echo "✅ Setup complete. Container will keep running."
tail -f /dev/null