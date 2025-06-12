#!/bin/bash

# This script runs alembic revision (if needed) and migrations for a specified project,
# ensuring the database schema is up to date.

if [ -z "$1" ]; then
    echo "Usage: $0 <project_directory>"
    exit 1
fi

project_dir="$1"/server

if [ ! -d "$project_dir" ]; then
    echo "Project directory $project_dir does not exist."
    exit 1
fi

cd "$project_dir" || exit 1

echo "Checking for model changes and generating Alembic revision if needed..."

# Attempt to generate revision with --autogenerate
output=$(alembic revision --autogenerate -m "Auto revision" 2>&1)

# Check if Alembic actually created a new file
if echo "$output" | grep -q "Generating"; then
    echo "New revision generated."
else
    echo "No changes detected in models. No new revision created."
fi

echo "Running Alembic migrations in $project_dir..."
alembic upgrade head

if [ $? -ne 0 ]; then
    echo "Alembic migrations failed."
    exit 1
fi

echo "Alembic migrations completed successfully."
exit 0
# End of script