#!/bin/bash

# All Projects
CWD=$(pwd)
CURIOUS_BYTES_WEBSITE_DIR="$CWD/curiousbytes-website"
BEETRACKER_APP_DIR="$CWD/beetracker-app"
BEEFINANCIAL_APP_DIR="$CWD/beefinancial-app"
UTILITIES_DIR="$CWD/utilities"
GENERATORS_MODULE="generators"
SCRIPTS_DIR="$CWD/scripts"
source /home/curious_bytes/Documents/curious_bytes/bin/activate

rotate_backups() {
    local base_file="$1"
    local max_backups=5

    for ((i=max_backups; i>=1; i--)); do
        if [ -f "${base_file}_${i}.json" ]; then
            mv "${base_file}_${i}.json" "${base_file}_$((i+1)).json"
        fi
    done

    if [ -f "${base_file}.json" ]; then
        mv "${base_file}.json" "${base_file}_1.json"
    fi
}

download_youtube_data() {
    echo "Downloading YouTube data..."
    
    # Load env temporarily
    set -a
    source "$CURIOUS_BYTES_WEBSITE_DIR/.env"
    set +a

    rotate_backups "$CURIOUS_BYTES_WEBSITE_DIR/data/youtube_data"

    "$SCRIPTS_DIR/youtube_script.sh" download \
        --output "$CURIOUS_BYTES_WEBSITE_DIR/data/youtube_data.json" \
        --limit 5

    local status=$?

    # Clean up env variables
    unset YOUTUBE_API_KEY
    unset YOUTUBE_CHANNEL_ID

    if [ $status -ne 0 ]; then
        echo "❌ Failed to download YouTube data"
        return 1
    fi

    echo "✅ YouTube data downloaded successfully"
    return 0
}

update_youtube_data() {
    echo "Updating YouTube data..."

    "$SCRIPTS_DIR/youtube_script.sh" upload \
        --input "$CURIOUS_BYTES_WEBSITE_DIR/data/youtube_data.json"

    if [ $? -ne 0 ]; then
        echo "❌ Failed to update YouTube data"
        return 1
    fi

    echo "✅ YouTube data updated successfully"
    return 0
}

download_and_update_youtube_data() {
    echo "Downloading and updating YouTube data..."
    download_youtube_data || return 1
    update_youtube_data || return 1
    echo "✅ YouTube data downloaded and updated successfully"
}

# Calling Generators
generate_models() {
    project_name=$1
    echo "Generating models for $project_name..."
    case $project_name in
        curiousbytes)
            project_dir="$CURIOUS_BYTES_WEBSITE_DIR"
            project_name="CuriousBytes"
            ;;
        beetracker)
            project_dir="$BEETRACKER_APP_DIR"
            project_name="BeeTracker"
            ;;
        beefinancial)
            project_dir="$BEEFINANCIAL_APP_DIR"
            project_name="BeeFinancial"
            ;;
        *)
            echo "Unknown project name: $project_name"
            return 1
            ;;
    esac
    model_type=$2
    bash "$SCRIPTS_DIR/generate_models.sh" "$project_dir" "$model_type" "$project_name"
    if [ $? -ne 0 ]; then
        echo "Failed to generate models for $project_name with type $model_type"
        return 1
    fi
    echo "Models generated successfully for $project_name with type $model_type"
    return 0
}

run_alembic_migrations() {
    project_name=$1
    echo "Running Alembic migrations for $project_name..."
    case $project_name in
        curiousbytes)
            project_dir="$CURIOUS_BYTES_WEBSITE_DIR"
            ;;
        beetracker)
            project_dir="$BEETRACKER_APP_DIR"
            ;;
        beefinancial)
            project_dir="$BEEFINANCIAL_APP_DIR"
            ;;
        *)
            echo "Unknown project name: $project_name"
            return 1
            ;;
    esac
    bash "$SCRIPTS_DIR/run_alembic_migrations.sh" "$project_dir"
    if [ $? -ne 0 ]; then
        echo "Failed to run Alembic migrations for $project_name"
        return 1
    fi
    echo "Alembic migrations completed successfully for $project_name"
    return 0
}



# Curious Bytes
