#!/bin/bash

# All Projects
CWD=$(pwd)
CURIOUS_BYTES_WEBSITE_DIR="$CWD/curiousbytes-website"
BEETRACKER_APP_DIR="$CWD/beetracker-app"
BEEFINANCIAL_APP_DIR="$CWD/beefinancial-app"
UTILITIES_DIR="$CWD/utilities"
GENERATORS_MODULE="generators"
SCRIPTS_DIR="$CWD/scripts"


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


# Curious Bytes
