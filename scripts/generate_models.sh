#!/bin/bash


# This script generates models for project dir passed as an argument and model_type, 
# File structure should be like this:
# project_dir_passed/
#   ├── iosapp/ (mandatory for swift models)
#   |     ├── project_name/
#   |     ├── project_name.xcodeproj
#   ├── server/ (mandatory for sqlalchemy and pydantic models)
#         ├── alembic/
#         ├── app/
#         ├── alembic.ini
#         ├── app.schema
#         ├── requirements.txt
#         └── ...
#
# Current Models Types Supported: pydantic, sqlalchemy, swiftmodels, swiftviewmodels, swiftmodelendpoints
# Usage: ./generate_models.sh <project_dir> <model_type>
# 
# 
# Once ran this script generates models under 
# sqlalchemy: project_dir_passed/server/app/autogen/models
# pydantic: project_dir_passed/server/app/autogen/schemas
# swiftmodels: project_dir_passed/iosapp/project_name/Autogen/Models
# swiftviewmodels: project_dir_passed/iosapp/project_name/Autogen/ViewModels
# swiftmodelendpoints: project_dir_passed/iosapp/project_name/Autogen/Endpoints


generate_intermediate_parsed_models() {
    project_dir=$1
    echo "Generating intermediate parsed models for $project_dir..."
    apps_schema_file_path="$project_dir/server/app.schema"
    if [[ ! -f "$apps_schema_file_path" ]]; then
        echo "Schema file not found at $apps_schema_file_path"
        return 1
    fi
    build_dir_path="$project_dir/intermediate_parsed_models"
    mkdir -p "$build_dir_path"
    python3 -m generators.parse_schema \
        --schema_file "$apps_schema_file_path" \
        --build_dir "$build_dir_path"
}


generate_pydantic_models() {
    project_dir=$1
    echo "Generating Pydantic models for $project_dir..."
    generate_intermediate_parsed_models "$project_dir"
    python3 -m generators.pydantic_generator \
        --build-dir "$project_dir/intermediate_parsed_models" \
        --output-dir "$project_dir/server/app/autogen/schemas"
    echo "Pydantic models generated at $project_dir/server/app/autogen/schemas"
}

generate_sqlalchemy_models() {
    project_dir=$1
    echo "Generating SQLAlchemy models for $project_dir..."
    generate_intermediate_parsed_models "$project_dir" "generators"
    python3 -m generators.sqlalchemy_generator \
        --build-dir "$project_dir/intermediate_parsed_models" \
        --output-dir "$project_dir/server/app/autogen/models"
    echo "SQLAlchemy models generated at $project_dir/server/app/autogen/models"
}

generate_swift_models() {
    project_dir=$1
    echo "Generating Swift models for $project_dir..."
    ios_app_dir="$project_dir/iosapp"
    if [[ ! -d "$ios_app_dir" ]]; then
        echo "iOS app directory not found at $ios_app_dir"
        return 1
    fi
    project_name=$2
    autogen_models_dir="$ios_app_dir/$project_name/Autogen/Models"
    mkdir -p "$autogen_models_dir"
    python3 -m generators.swift_generator \
        --build-dir "$project_dir/intermediate_parsed_models" \
        --output-dir "$autogen_models_dir"
    echo "Swift models generated at $autogen_models_dir"
}

generate_swift_viewmodels() {
    project_dir=$1
    echo "Generating Swift view models for $project_dir..."
    ios_app_dir="$project_dir/iosapp"
    if [[ ! -d "$ios_app_dir" ]]; then
        echo "iOS app directory not found at $ios_app_dir"
        return 1
    fi
    project_name=$2
    autogen_viewmodels_dir="$ios_app_dir/$project_name/Autogen/ViewModels"
    mkdir -p "$autogen_viewmodels_dir"
    python3 -m generators.swift_viewmodel_generator \
        --build-dir "$project_dir/intermediate_parsed_models" \
        --output-dir "$autogen_viewmodels_dir"
    echo "Swift view models generated at $autogen_viewmodels_dir"
}

generate_swift_model_endpoints() {
    project_dir=$1
    echo "Generating Swift model endpoints for $project_dir..."
    ios_app_dir="$project_dir/iosapp"
    if [[ ! -d "$ios_app_dir" ]]; then
        echo "iOS app directory not found at $ios_app_dir"
        return 1
    fi
    project_name=$2
    autogen_endpoints_dir="$ios_app_dir/$project_name/Autogen/Endpoints"
    mkdir -p "$autogen_endpoints_dir"
    python3 -m generators.swift_model_endpoint_generator \
        --build-dir "$project_dir/intermediate_parsed_models" \
        --output-dir "$autogen_endpoints_dir"
    echo "Swift model endpoints generated at $autogen_endpoints_dir"
}

main() {
    if [ "$#" -ne 3 ]; then
        echo "Usage: $0 <project_dir> <model_type> <project_name>"
        exit 1
    fi

    project_dir=$1
    model_type=$2
    project_name=$3

    case $model_type in
        pydantic)
            generate_pydantic_models "$project_dir"
            ;;
        sqlalchemy)
            generate_sqlalchemy_models "$project_dir"
            ;;
        swiftmodels)
            generate_swift_models "$project_dir" "$project_name"
            ;;
        swiftviewmodels)
            generate_swift_viewmodels "$project_dir" "$project_name"
            ;;
        swiftmodelendpoints)
            generate_swift_model_endpoints "$project_dir" "$project_name"
            ;;
        *)
            echo "Unknown model type: $model_type"
            exit 1
            ;;
    esac
}

main "$@"