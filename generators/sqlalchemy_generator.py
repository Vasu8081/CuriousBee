import os
import json
from pathlib import Path

SQLA_TYPES = {
    "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Time", "LARGE_TEXT": "TEXT",
    "TIMESTAMP": "DateTime", "INTEGER": "Integer", "FLOAT": "Float", "BOOLEAN": "Boolean",
    "BYTEA": "LargeBinary", "JSONB": "String", "JSON": "JSON", "ARRAY": "String"
}
PY_TYPES = {
    "UUID": "uuid.UUID", "TEXT": "str", "DATE": "datetime.date", "TIME": "datetime.time",
    "TIMESTAMP": "datetime.datetime", "INTEGER": "int", "FLOAT": "float", "BOOLEAN": "bool",
    "BYTEA": "bytes", "JSONB": "dict", "JSON": "dict", "ARRAY": "list"
}

def to_pascal(name):
    return ''.join(w.capitalize() for w in name.split('_'))

def to_camel(name):
    return name.split('_')[0] + ''.join(w.capitalize() for w in name.split('_')[1:])

def to_enum_name(name):
    name = name.split('_')
    name = [part.lower().capitalize() for part in name]
    return ''.join(name)

def generate_alembic_models(build_dir: str, output_dir: str):
    build_path = Path(build_dir)
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    tables = json.loads((build_path / "tables.json").read_text())
    enums = json.loads((build_path / "enums.json").read_text())
    fks = json.loads((build_path / "foreign_keys.json").read_text())
    reverse_fks = json.loads((build_path / "reverse_fks.json").read_text())
    circular_deps = set(map(tuple, json.loads((build_path / "circular_deps.json").read_text())))

    enums_file = output_path / "enums.py"
    with enums_file.open("w") as f:
        f.write("from enum import Enum\n\n")
        for enum in enums:
            enum_name = to_pascal(enum)
            f.write(f"class {enum_name}(str, Enum):\n")
            for value in enums[enum]:
                f.write(f"    {to_enum_name(value[0])} = '{value[0]}'\n")
            f.write("\n")
    print(f"Generated enums file: {enums_file}")
    print("✅ Enums file generated successfully.")

    for table, columns in tables.items():
        model_file = output_path / f"{table}.py"
        class_name = to_pascal(table)

        with open(model_file, "w") as f:
            f.write("import uuid\nimport datetime\n")
            f.write("from sqlalchemy import Column, ForeignKey\n")
            f.write("from sqlalchemy.orm import relationship\n")
            f.write("from sqlalchemy.dialects.postgresql import UUID\n")
            f.write("from sqlalchemy.types import String, Integer, TEXT, Float, Date, Time, DateTime, Boolean, LargeBinary, JSON\n")
            f.write("from app.db.base import Base\n")
            f.write("from typing import Optional, cast\n")
            f.write("from sqlalchemy import Enum as SQLEnum\n")
            f.write("from app.autogen.models.enums import *\n\n")
            f.write(f"class {class_name}(Base):\n")
            f.write(f"    __tablename__ = '{table}'\n\n")

            for col_name, col_type, modifiers in columns:
                sqla_type = SQLA_TYPES.get(col_type.upper(), "String")
                if sqla_type == "String":
                    if col_type in enums.keys():
                        default_value = enums[col_type]
                        for value in default_value:
                            if len(value) >= 2:
                                default_value = value[0]
                                break

                        sqla_type = f"SQLEnum({to_pascal(col_type)}, name='{col_type}')"
                        col_def = f"Column({sqla_type}, nullable=False, default={to_pascal(col_type)}.{to_enum_name(default_value)})"
                        f.write(f"    {col_name} = {col_def}\n")
                        continue
                is_pk = any("primary" in m.lower() for m in modifiers)
                fk_target = None
                for fk in fks.get(table, []):
                    if fk["src_col"] == col_name:
                        fk_target = f"{fk['tgt_table']}.{fk['tgt_col']}"
                        break

                col_def = f"Column({sqla_type}"
                if fk_target:
                    tgt = fk_target.split('.')[0]
                    is_circular = (table, tgt) in circular_deps or (tgt, table) in circular_deps
                    col_def += f", ForeignKey(\"{fk_target}\""
                    if is_circular:
                        col_def += ", use_alter=True"
                    col_def += ")"
                if is_pk:
                    col_def += ", primary_key=True, default=uuid.uuid4"
                col_def += ")"
                f.write(f"    {col_name} = {col_def}\n")

            for fk in fks.get(table, []):
                rel_type = fk["type"]
                tgt_class = to_pascal(fk["tgt_table"])
                field_name = fk["left_field"]
                if rel_type == "many_to_many":
                    uselist = "uselist=True"
                elif rel_type == "many_to_one":
                    uselist = "uselist=False"
                elif rel_type == "one_to_one":
                    uselist = "uselist=False"
                else:
                    uselist = "uselist=True"
                back_pop = f"back_populates='{fk['right_field']}'"
                f.write(f"    {field_name} = relationship('{tgt_class}', {uselist}, {back_pop})\n")

            for rfk in reverse_fks.get(table, []):
                rel_type = rfk["type"]
                src_class = to_pascal(rfk["src_table"])
                field_name = rfk["right_field"]
                if rel_type == "many_to_many":
                    uselist = "uselist=True"
                elif rel_type == "many_to_one":
                    uselist = "uselist=True"
                elif rel_type == "one_to_one":
                    uselist = "uselist=False"
                else:
                    uselist = "uselist=True"
                back_pop = f"back_populates='{rfk['left_field']}'"
                f.write(f"    {field_name} = relationship('{src_class}', {uselist}, {back_pop})\n")

            f.write("\n")
            for col_name, col_type, _ in columns:
                method = to_pascal(col_name.lstrip("_"))
                py_type = PY_TYPES.get(col_type.upper(), "str")
                if py_type == "str" and col_type in enums.keys():
                    py_type = to_pascal(col_type)

                f.write(f"    @property\n")
                f.write(f"    def {method}(self) -> {py_type}:\n")
                f.write(f"        return cast({py_type}, self.{col_name})\n\n")

                f.write(f"    @{method}.setter\n")
                f.write(f"    def {method}(self, value: Optional[{py_type}]) -> None:\n")
                f.write(f"        self.{col_name} = value\n\n")

            f.write("    def to_schema(self):\n")
            f.write(f"        from app.autogen.schemas.{table}_schema import {class_name}Schema\n")
            f.write(f"        return {class_name}Schema.model_validate(self)\n\n")
            f.write("    def to_dict(self):\n")
            f.write("        return self.to_schema().model_dump(by_alias=False)\n")

    with open(output_path / "__init__.py", "w") as init_f:
        for table in tables:
            init_f.write(f"from .{table} import {to_pascal(table)}\n")

    print("✅ Alembic models generated successfully.")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate SQLAlchemy models from build directory.")
    parser.add_argument("--build-dir", type=str, help="Path to the build directory containing JSON files.")
    parser.add_argument("--output-dir", type=str, help="Path to the output directory for generated models.")

    args = parser.parse_args()
    print(f"Generating models from {args.build_dir} to {args.output_dir}...")
    generate_alembic_models(args.build_dir, args.output_dir)
    print("Models generation completed.")