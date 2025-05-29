import os
import re
from pathlib import Path
from collections import defaultdict

SCHEMA_FILE = "generator/app.schema"
SWIFT_OUTPUT_DIR = "BeeTracker/BeeTracker/Models"
os.makedirs(SWIFT_OUTPUT_DIR, exist_ok=True)

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

swift_type_map = {
    "UUID": "UUID",
    "TEXT": "String",
    "DATE": "Date",
    "TIME": "Date",
    "TIMESTAMP": "Date",
    "INTEGER": "Int",
    "FLOAT": "Double",
    "BOOLEAN": "Bool"
}

def to_swift_struct_name(table_name):
    return ''.join(word.capitalize() for word in table_name.split('_'))

def to_swift_field_name(col_name):
    return col_name.lstrip("_")

for table, cols in tables.items():
    struct_name = to_swift_struct_name(table)
    file_path = os.path.join(SWIFT_OUTPUT_DIR, f"{struct_name}.swift")

    with open(file_path, "w") as f:
        f.write("import Foundation\n\n")
        f.write(f"struct {struct_name}: Codable {{\n")

        # Regular fields
        for col in cols:
            if len(col) == 3:
                col_name, col_type, modifiers = col
            elif len(col) == 2:
                col_name, col_type = col
                modifiers = []
            else:
                continue  # Skip if format is unexpected

            if "HIDDEN" in [m.upper() for m in modifiers]:
                continue
            swift_type = swift_type_map.get(col_type.upper(), "String")
            swift_field = to_swift_field_name(col_name)
            f.write(f"    var {swift_field}: {swift_type}?\n")

        # Foreign key relationships (refer_left)
        for fk in fks.get(table, []):
            if fk.get("pydantic_refer") == "refer_left":
                tgt_struct = to_swift_struct_name(fk["tgt_table"])
                rel_type = fk["type"]
                field_name = fk["tgt_col"]
                if rel_type in {"many_to_one", "one_to_one"}:
                    f.write(f"    var {field_name}: {tgt_struct}?\n")
                elif rel_type in {"one_to_many", "many_to_many"}:
                    f.write(f"    var {field_name}: [{tgt_struct}] = []\n")

        # Reverse foreign key relationships (refer_right)
        for rfk in reverse_fks.get(table, []):
            if rfk.get("pydantic_refer") == "refer_right":
                src_struct = to_swift_struct_name(rfk["src_table"])
                rel_type = rfk["type"]
                field_name = rfk["src_table"]
                if rel_type in {"many_to_one", "many_to_many"}:
                    f.write(f"    var {field_name}: [{src_struct}] = []\n")
                elif rel_type in {"one_to_one", "one_to_many"}:
                    f.write(f"    var {field_name.rstrip('s')}: {src_struct}?\n")

        f.write("}\n")
    print(f"Generated Swift struct for {table} at {file_path}")