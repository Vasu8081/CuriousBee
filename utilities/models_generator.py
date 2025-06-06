from utilities.swift_generator import generate_swift_models
from utilities.swift_viewmodel_generator import generate_swift_viewmodels
from utilities.swift_endpoints_generator import generate_swift_endpoints
from utilities.alembic_generator import generate_alembic_models
from utilities.pydantic_generator import generate_pydantic_schemas
from utilities.parse_schema import generate_parsed_schema_files

from pathlib import Path
import argparse

def get_paths(project_name: str):
    """
    Given a project name (e.g., 'BeeTracker'), constructs paths for schema input,
    build output, and autogen directories for both Swift and Python.

    For project 'BeeTracker', outputs are:
    - schema_file:            CuriousBee/beetracker-app/server/app.schema
                              📥 Input schema file to parse.

    - build_dir:              CuriousBee/utilities/build/beetracker-app
                              🛠️  Stores intermediate JSON files (tables.json, etc.)

    Swift Outputs (in iOS app):
    - swift_model_dir:        CuriousBee/beetracker-app/iosapp/BeeTracker/Autogen/Models
                              🍎 Contains Swift Codable structs

    - swift_viewmodel_dir:    CuriousBee/beetracker-app/iosapp/BeeTracker/Autogen/ViewModels
                              👀 Contains Swift ObservableObject classes

    - swift_endpoint_dir:     CuriousBee/beetracker-app/iosapp/BeeTracker/Autogen/Endpoints
                              🌐 Contains Swift API networking code

    Python Outputs (in FastAPI backend):
    - alembic_model_dir:      CuriousBee/beetracker-app/server/app/autogen/models
                              🧠 Contains SQLAlchemy ORM models

    - pydantic_schema_dir:    CuriousBee/beetracker-app/server/app/autogen/schemas
                              📦 Contains Pydantic validation schemas
    """
    curiousbee_root = Path(__file__).parent.parent.resolve()
    subproject_folder = f"{project_name.lower()}-app"
    project_root = curiousbee_root / subproject_folder

    schema_file = project_root / "server" / "app.schema"
    build_dir = Path(__file__).parent / "build" / subproject_folder

    swift_model_dir = project_root / "iosapp" / project_name / "Autogen" / "Models"
    swift_viewmodel_dir = project_root / "iosapp" / project_name / "Autogen" / "ViewModels"
    swift_endpoint_dir = project_root / "iosapp" / project_name / "Autogen" / "Endpoints"

    alembic_model_dir = project_root / "server" / "app" / "autogen" / "models"
    pydantic_schema_dir = project_root / "server" / "app" / "autogen" / "schemas"

    return {
        "schema_file": schema_file,
        "build_dir": build_dir,
        "swift_model_dir": swift_model_dir,
        "swift_viewmodel_dir": swift_viewmodel_dir,
        "swift_endpoint_dir": swift_endpoint_dir,
        "alembic_model_dir": alembic_model_dir,
        "pydantic_schema_dir": pydantic_schema_dir
    }

def generate_swift(project_name: str):
    paths = get_paths(project_name)
    build = str(paths["build_dir"])

    generate_swift_models(build, str(paths["swift_model_dir"]))
    generate_swift_viewmodels(build, str(paths["swift_viewmodel_dir"]))
    generate_swift_endpoints(build, str(paths["swift_endpoint_dir"]))

def generate_python(project_name: str):
    paths = get_paths(project_name)
    schema = paths["schema_file"]
    build = paths["build_dir"]

    generate_parsed_schema_files(schema, build)
    generate_alembic_models(str(build), str(paths["alembic_model_dir"]))
    generate_pydantic_schemas(str(build), str(paths["pydantic_schema_dir"]))

def main():
    parser = argparse.ArgumentParser(description="Generate Swift and Python models from schema")
    parser.add_argument("--project", required=True, help="Project name, e.g., BeeTracker")
    parser.add_argument("--target", choices=["all", "swift", "python"], default="all", help="What to generate")
    args = parser.parse_args()

    if args.target in ("all", "python"):
        print(f"⚙️  Generating Python models for {args.project}...")
        generate_python(args.project)

    if args.target in ("all", "swift"):
        print(f"⚙️  Generating Swift models for {args.project}...")
        generate_swift(args.project)

    print("✅ All done!")

if __name__ == "__main__":
    main()