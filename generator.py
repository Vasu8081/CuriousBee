import os
import re
from pathlib import Path
from collections import defaultdict

SCHEMA_FILE = "app.schema"
OUTPUT_DIR = "app/db/models"
PYDANTIC_OUTPUT_DIR = "app/db/schemas"

os.makedirs(OUTPUT_DIR, exist_ok=True)
os.makedirs(PYDANTIC_OUTPUT_DIR, exist_ok=True)

with open(SCHEMA_FILE, "r") as f:
    schema = f.read()

def print_relationships_dict(title, relationships, reverse=False):
    print(f"\n{title}:")
    for table, refs in relationships.items():
        print(f"  {table}:")
        for ref in refs:
            if reverse:
                # reverse_fks: from src_table.src_col -> table.tgt_col
                print(f"    - {ref['src_table']}.{ref['src_col']} -> {table}.{ref['tgt_col']} [{ref['type']}]")
            else:
                # fks: from src_col -> tgt_table.tgt_col
                print(f"    - {ref['src_col']} -> {ref['tgt_table']}.{ref['tgt_col']} [{ref['type']}]")
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
        fk_parts = re.match(r"FK (\w+)\.(\w+) -> (\w+)\.(\w+)(?: \[(\w+_\w+)\]? \[(\w+_\w+)\])?", line)
        if fk_parts:
            src_table, src_col, tgt_table, tgt_col, rel_type, pydantic_refer = fk_parts.groups()
            relationship_type = rel_type or "many_to_one"  # Default fallback
            pydantic_refer = pydantic_refer or "refer_right"
            fks[src_table].append({
                "src_col": src_col,
                "src_table": src_table,
                "tgt_table": tgt_table,
                "tgt_col": tgt_col,
                "type": relationship_type,
                "pydantic_refer": pydantic_refer
            })
            reverse_fks[tgt_table].append({
                "src_table": src_table,
                "src_col": src_col,
                "tgt_table": tgt_table,
                "tgt_col": tgt_col,
                "type": relationship_type,
                "pydantic_refer": pydantic_refer
            })
    elif line.startswith("}"):
        current_table = None
    elif current_table and line:
        parts = line.split()
        col_name = parts[0]
        col_type = parts[1]
        modifiers = parts[2:] if len(parts) > 2 else []
        tables[current_table].append((col_name, col_type, modifiers))

# print_relationships_dict("Foreign Keys", fks, reverse=False)
# print_relationships_dict("Reverse Foreign Keys", reverse_fks, reverse=True)

# Detect circular FKs
dep_graph = defaultdict(set)
for src_table, fk_list in fks.items():
    for fk in fk_list:
        tgt_table = fk["tgt_table"]
        dep_graph[src_table].add(tgt_table)

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

sqlalchemy_types = {
    "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Time",
    "TIMESTAMP": "DateTime", "INTEGER": "Integer", "FLOAT": "Float", "BOOLEAN": "Boolean"
}
python_types = {
    "UUID": "uuid.UUID", "TEXT": "str", "DATE": "datetime.date", "TIME": "datetime.time",
    "TIMESTAMP": "datetime.datetime", "INTEGER": "int", "FLOAT": "float", "BOOLEAN": "bool"
}

def to_pascal_case(name): return ''.join(word.capitalize() for word in name.split('_'))
def to_camel_case(name): parts = name.split('_'); return parts[0] + ''.join(word.capitalize() for word in parts[1:])

for table, columns in tables.items():
    model_path = os.path.join(OUTPUT_DIR, f"{table}.py")
    pydantic_path = os.path.join(PYDANTIC_OUTPUT_DIR, f"{table}_schema.py")
    class_name = to_pascal_case(table)

    # --- SQLAlchemy Model ---
    with open(model_path, "w") as f:
        f.write("import uuid\nimport datetime\n")
        f.write("from sqlalchemy import Column, ForeignKey\n")
        f.write("from sqlalchemy.orm import relationship\n")
        f.write("from sqlalchemy.dialects.postgresql import UUID\n")
        f.write("from sqlalchemy.types import String, Integer, Float, Date, Time, DateTime, Boolean\n")
        f.write("from app.db.base import Base\n\n")
        f.write(f"class {class_name}(Base):\n")
        f.write(f"    __tablename__ = '{table}'\n\n")

        for col_name, col_type, modifiers in columns:
            sqla_type = sqlalchemy_types.get(col_type.upper(), "String")
            is_pk = any("primary" in m.lower() for m in modifiers)
            fk_target = None
            for fk in fks.get(table, []):
                src_col = fk["src_col"]
                tgt_table = fk["tgt_table"]
                tgt_col = fk["tgt_col"]
                rel_type = fk["type"]
                target = f"{tgt_table}.{tgt_col}"
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

        for fk in fks.get(table.lower(), []):
            src_col = fk["src_col"]
            tgt_table = fk["tgt_table"]
            tgt_col = fk["tgt_col"]
            rel_type = fk["type"]
            tgt_class = to_pascal_case(tgt_table)
            if rel_type == "many_to_one":
                fld_name = src_col[1:-3]  # Remove leading '_' and last '_id' for singular
                backpopulates = f"back_populates='{table.lower()}'"
                uselist = f"uselist=False"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "one_to_many":
                fld_name = src_table.lower()           
                backpopulates = f"back_populates='{src_col[1:-3]}'"  # Remove last 's' for singular
                uselist = f"uselist=True"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "one_to_one":
                fld_name = tgt_table.lower()[:-1]                 
                backpopulates = f"back_populates='{table.lower()[:-1]}'"  # Remove last 's' for singular
                uselist = f"uselist=False"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "many_to_many":
                fld_name = tgt_table.lower()
                backpopulates = f"back_populates='{table.lower()}'"
                uselist = f"uselist=True"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            else:
                raise ValueError(f"Unknown relationship type: {rel_type}")

            
        for reverse_fk in reverse_fks.get(table, []):
            src_table = reverse_fk["src_table"]
            src_col = reverse_fk["src_col"]
            tgt_col = reverse_fk["tgt_col"]
            rel_type = reverse_fk["type"]
            tgt_class = to_pascal_case(src_table)
            if rel_type == "many_to_one":
                fld_name = src_table.lower()           
                backpopulates = f"back_populates='{src_col[1:-3]}'"  # Remove last 's' for singular
                uselist = f"uselist=True"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "one_to_many":
                fld_name = src_col[1:-3]  # Remove leading '_' and last '_id' for singular
                backpopulates = f"back_populates='{table.lower()}'"
                uselist = f"uselist=False"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "one_to_one":
                fld_name = src_table.lower()[:-1]  # Remove last 's' for singular
                backpopulates = f"back_populates='{table.lower()[:-1]}'"  # Remove last 's' for singular
                uselist = f"uselist=False"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            elif rel_type == "many_to_many":
                fld_name = src_table.lower()
                backpopulates = f"back_populates='{table.lower()}'"
                uselist = f"uselist=True"
                f.write(f"    {fld_name} = relationship('{tgt_class}', {uselist}, {backpopulates})\n")
            else:
                raise ValueError(f"Unknown relationship type: {rel_type}")
            
                

        f.write("\n")
        for col_name, col_type, _ in columns:
            method_name = to_camel_case(col_name.lstrip("_"))
            py_type = python_types.get(col_type.upper(), "str")
            f.write(f"    def {method_name}(self, value: {py_type} = None) -> {py_type}:\n")
            f.write(f"        if value is None:\n")
            f.write(f"            return self.{col_name}\n")
            f.write(f"        self.{col_name} = value\n\n")

        f.write(f"    def to_schema(self):\n")
        f.write(f"        from app.db.schemas.{table}_schema import {class_name}Schema\n")
        f.write(f"        return {class_name}Schema.from_orm(self)\n")

        # --- Pydantic Schema ---
    with open(pydantic_path, "w") as f:
        f.write("import uuid\nimport datetime\n")
        f.write("from pydantic import BaseModel, Field, ConfigDict\n")
        f.write("from typing import Optional, List, ForwardRef\n\n")

        # Collect related schema class names
        related_classes = set()
        for fk in fks.get(table, []):
            related_classes.add(to_pascal_case(fk["tgt_table"]) + "Schema")
        for rfk in reverse_fks.get(table, []):
            related_classes.add(to_pascal_case(rfk["src_table"]) + "Schema")

        # Declare ForwardRefs
        for rc in sorted(related_classes):
            f.write(f"{rc} = ForwardRef('{rc}')\n")
        f.write("\n")

        # Schema class definition
        f.write(f"class {class_name}Schema(BaseModel):\n")
        for col_name, col_type, modifiers in columns:
            if "HIDDEN" in [m.upper() for m in modifiers]:
                continue
            py_type = python_types.get(col_type.upper(), "str")
            alias = col_name
            field_name = col_name.lstrip("_")
            f.write(f"    {field_name}: Optional[{py_type}] = Field(alias='{alias}')\n")

        # Foreign key relationships
        for fk in fks.get(table, []):
            tgt_class = to_pascal_case(fk["tgt_table"]) + "Schema"
            rel_type = fk["type"]
            field_name = fk["tgt_col"]
            pydantic_refer = fk["pydantic_refer"]
            if pydantic_refer == "refer_left":
                if rel_type in {"many_to_one", "one_to_one"}:
                    f.write(f"    {field_name}: Optional['{tgt_class}'] = None\n")
                elif rel_type in {"one_to_many", "many_to_many"}:
                    f.write(f"    {field_name}: List['{tgt_class}'] = []\n")
                else:
                    raise ValueError(f"Unknown relationship type: {rel_type}")

        # Reverse foreign key relationships
        for rfk in reverse_fks.get(table, []):
            src_class = to_pascal_case(rfk["src_table"]) + "Schema"
            rel_type = rfk["type"]
            field_name = rfk["src_table"]  # plural for collections
            if rfk["pydantic_refer"] == "refer_right":
                if rel_type in {"many_to_one", "many_to_many"}:
                    f.write(f"    {field_name}: List['{src_class}'] = []\n")
                elif rel_type in {"one_to_one", "one_to_many"}:
                    f.write(f"    {field_name.rstrip('s')}: Optional['{src_class}'] = None\n")

        # Config and model rebuild
        f.write("\n")
        f.write("    model_config = ConfigDict(\n")
        f.write("        from_attributes=True,\n")
        f.write("        populate_by_name=True\n")
        f.write("    )\n\n")

        # f.write(f"{class_name}Schema.model_rebuild(_types_namespace=globals())\n")
# Init file for models
init_path = os.path.join(OUTPUT_DIR, "__init__.py")
init_path_pydantic = os.path.join(PYDANTIC_OUTPUT_DIR, "__init__.py")
with open(init_path_pydantic, "w") as f:
    for table in tables:
        f.write(f"from .{table}_schema import {to_pascal_case(table)}Schema\n")
    for table in tables:
        f.write(f"{to_pascal_case(table)}Schema.model_rebuild(_types_namespace=globals())\n")
with open(init_path, "w") as f:
    for table in tables:
        f.write(f"from .{table} import {to_pascal_case(table)}\n")
        