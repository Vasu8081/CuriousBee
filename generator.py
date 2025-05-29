import os
import re
from pathlib import Path
from collections import defaultdict, deque

SCHEMA_FILE = "app.schema"
OUTPUT_DIR = "app/db/models"
PYDANTIC_OUTPUT_DIR = "app/db/schemas"

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(PYDANTIC_OUTPUT_DIR, exist_ok=True)

with open(SCHEMA_FILE, "r") as f:
    schema = f.read()

tables = {}
fks = defaultdict(list)
reverse_fks = defaultdict(list)
current_table = None

for line in schema.splitlines():
    line = line.strip()
    if line.startswith("Table"):
        current_table = line.split()[1]
        tables[current_table] = []
    elif line.startswith("FK"):
        fk_parts = re.match(r"FK (\w+)\.(\w+) -> (\w+)\.(\w+)", line)
        if fk_parts:
            src_table, src_col, tgt_table, tgt_col = fk_parts.groups()
            fks[src_table].append((src_col, f"{tgt_table}.{tgt_col}"))
            reverse_fks[tgt_table].append(src_table)
    elif line.startswith("}"):
        current_table = None
    elif current_table and line:
        parts = line.split()
        col_name = parts[0]
        col_type = parts[1]
        modifiers = parts[2:] if len(parts) > 2 else []
        tables[current_table].append((col_name, col_type, modifiers))

# Detect circular FKs using graph traversal
dep_graph = defaultdict(set)
for src, refs in fks.items():
    for _, target in refs:
        tgt_table = target.split(".")[0]
        dep_graph[src].add(tgt_table)

circular_deps = set()
visited = set()
def visit(node, path):
    if node in path:
        for p in path[path.index(node):]:
            circular_deps.add((p, node))
        return
    if node in visited:
        return
    visited.add(node)
    for neighbor in dep_graph[node]:
        visit(neighbor, path + [node])
for t in list(dep_graph):
    visit(t, [])

# Data type mapping
sqlalchemy_types = {
    "UUID": "UUID",
    "TEXT": "String",
    "DATE": "Date",
    "TIME": "Time",
    "TIMESTAMP": "DateTime",
    "INTEGER": "Integer",
    "FLOAT": "Float",
    "BOOLEAN": "Boolean",
}

python_types = {
    "UUID": "uuid.UUID",
    "TEXT": "str",
    "DATE": "datetime.date",
    "TIME": "datetime.time",
    "TIMESTAMP": "datetime.datetime",
    "INTEGER": "int",
    "FLOAT": "float",
    "BOOLEAN": "bool",
}

def to_pascal_case(name):
    return ''.join(word.capitalize() for word in name.split('_'))

def to_camel_case(name):
    parts = name.split('_')
    return parts[0] + ''.join(word.capitalize() for word in parts[1:])

for table, columns in tables.items():
    model_path = os.path.join(OUTPUT_DIR, f"{table}.py")
    pydantic_path = os.path.join(PYDANTIC_OUTPUT_DIR, f"{table}_schema.py")
    
    class_name = to_pascal_case(table)

    with open(model_path, "w") as f:
        f.write("import uuid\n")
        f.write("import datetime\n")
        f.write("from sqlalchemy import Column, ForeignKey\n")
        f.write("from sqlalchemy.dialects.postgresql import UUID\n")
        f.write("from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean\n")
        f.write("from app.db.base import Base\n\n")
        f.write(f"class {class_name}(Base):\n")
        f.write(f"    __tablename__ = '{table}'\n\n")

        for col_name, col_type, modifiers in columns:
            sqla_type = sqlalchemy_types.get(col_type.upper(), "String")
            is_pk = any("primary" in m.lower() for m in modifiers)
            fk_target = None
            for src_col, target in fks.get(table, []):
                if src_col == col_name:
                    fk_target = target

            col_def = f"Column({sqla_type}"
            if fk_target:
                tgt_table = fk_target.split('.')[0]
                is_circular = (table, tgt_table) in circular_deps or (tgt_table, table) in circular_deps
                fk_args = f"\"{fk_target}\""
                if is_circular:
                    fk_args += ", use_alter=True"
                col_def += f", ForeignKey({fk_args})"
            if is_pk:
                col_def += ", primary_key=True"
            col_def += ")"
            f.write(f"    {col_name} = {col_def}\n")

        f.write("\n")
        for col_name, col_type, _ in columns:
            new_col_name = col_name[1:] if col_name.startswith("_") else col_name
            camel = to_camel_case(new_col_name)
            py_type = python_types.get(col_type.upper(), "str")
            f.write(f"    def {camel}(self, value: {py_type} = None) -> {py_type}:\n")
            f.write(f"        if value is None:\n")
            f.write(f"            return self.{col_name}\n")
            f.write(f"        self.{col_name} = value\n\n")

        f.write(f"    def to_schema(self):\n")
        f.write(f"        from app.db.schemas.{table}_schema import {class_name}Schema\n")
        f.write(f"        return {class_name}Schema.from_orm(self)\n")

    with open(pydantic_path, "w") as f:
        f.write("import uuid\n")
        f.write("import datetime\n")
        f.write("from pydantic import BaseModel, Field, ConfigDict\n")
        f.write("from typing import Optional\n\n")
        f.write(f"class {class_name}Schema(BaseModel):\n")
        for col_name, col_type, _ in columns:
            py_type = python_types.get(col_type.upper(), "str")
            f.write(f"    {col_name[1:]}: Optional[{py_type}] = Field(alias='{col_name}')\n")
        f.write("\n")
        f.write("    model_config = ConfigDict(\n")
        f.write("        from_attributes=True,\n")
        f.write("        populate_by_name=True\n")
        f.write("    )\n")

init_path = os.path.join(OUTPUT_DIR, "__init__.py")
with open(init_path, "w") as f:
    for table in tables:
        f.write(f"from .{table} import {to_pascal_case(table)}\n")