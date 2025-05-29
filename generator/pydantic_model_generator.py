import os
import re
from pathlib import Path
from collections import defaultdict

SCHEMA_FILE = "generator/app.schema"
PYDANTIC_OUTPUT_DIR = "app/db/autogen/schemas"

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
    pydantic_path = os.path.join(PYDANTIC_OUTPUT_DIR, f"{table}_schema.py")
    class_name = to_pascal_case(table)
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

init_path_pydantic = os.path.join(PYDANTIC_OUTPUT_DIR, "__init__.py")
with open(init_path_pydantic, "w") as f:
    for table in tables:
        f.write(f"from .{table}_schema import {to_pascal_case(table)}Schema\n")
    for table in tables:
        f.write(f"{to_pascal_case(table)}Schema.model_rebuild(_types_namespace=globals())\n")