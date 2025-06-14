# utilities/parser_schema.py
import os
import re
import json
from pathlib import Path
from collections import defaultdict

def parse_schema(schema_text):
    tables = {}
    enums = {}
    models = {}
    protocols = {}
    fks = defaultdict(list)
    reverse_fks = defaultdict(list)
    current_table = None
    current_enum = None
    current_model = None
    current_protocol = None

    for line in schema_text.splitlines():
        line = line.strip()
        if not line:
            continue

        if line.startswith("Table"):
            parts = line.split()
            if len(parts) >= 2:
                current_table = parts[1]
                tables[current_table] = []
            continue

        if line.startswith("Enum"):
            parts = line.split()
            if len(parts) >= 2:
                current_enum = parts[1]
                enums[current_enum] = []
            continue

        if line.startswith("Model"):
            parts = line.split()
            if len(parts) >= 2:
                current_model = parts[1]
                if len(parts) > 3:
                    models[current_model] = {"parents": [parts[3]], "fields": []}
                else:
                    models[current_model] = {"parents": [], "fields": []}
            continue

        if line.startswith("Protocol"):
            parts = line.split()
            if len(parts) >= 2:
                current_protocol = parts[1]
                protocols[current_protocol] = []
            continue

        if line.startswith("FK"):
            match = re.match(
                r"FK\s+(\w+)\.(\w+)\s+as\s+(\w+)\s+->\s+(\w+)\.(\w+)\s+as\s+(\w+)(?:\s+\[(\w+_\w+)\])?(?:\s+\[(\w+_\w+)\])?",
                line
            )
            if match:
                src_table, src_col, left_field, tgt_table, tgt_col, right_field, rel_type, refer_dir = match.groups()
                rel_type = rel_type or "many_to_one"
                refer_dir = refer_dir or "refer_right"

                fk_obj = {
                    "src_table": src_table,
                    "src_col": src_col,
                    "tgt_table": tgt_table,
                    "tgt_col": tgt_col,
                    "type": rel_type,
                    "pydantic_refer": refer_dir,
                    "left_field": left_field,
                    "right_field": right_field
                }

                fks[src_table].append(fk_obj)
                reverse_fks[tgt_table].append(fk_obj)
            continue

        if line.startswith("}"):
            current_table = None
            current_enum = None
            current_model = None
            current_protocol = None
            continue

        if current_table:
            parts = line.split()
            col_name = parts[0]
            col_type = parts[1]
            modifiers = parts[2:] if len(parts) > 2 else []
            tables[current_table].append((col_name, col_type, modifiers))
        elif current_enum:
            parts = line.split()
            if len(parts) >= 2:
                enum_value = parts[0]
                enum_alias = parts[1] if len(parts) > 1 else enum_value
                enums[current_enum].append((enum_value, enum_alias))
            else:
                enums[current_enum].append([parts[0]])
        elif current_model:
            parts = line.split(";")
            model_property = parts[0]
            model_type = parts[1]
            if parts[2] == "NOFORM":
                display_name = ""
            else:
                display_name = parts[2]
            models[current_model]["fields"].append((model_property, model_type, display_name))
        elif current_protocol:
            parts = line.split()
            protocol_method = parts[0]
            protocol_return_type = parts[1]
            protocols[current_protocol].append((protocol_method, protocol_return_type))

    return tables, fks, reverse_fks, enums, models, protocols

def detect_circular_deps(fks_dict):
    dep_graph = defaultdict(set)
    for src_table, fk_list in fks_dict.items():
        for fk in fk_list:
            dep_graph[src_table].add(fk["tgt_table"])

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
        for neighbor in dep_graph.get(node, []):
            visit(neighbor, path + [node])

    for tbl in list(dep_graph.keys()):
        visit(tbl, [])

    return circular_deps

def save_json(obj, path: Path):
    with path.open("w") as f:
        json.dump(obj, f, indent=2)

def generate_parsed_schema_files(schema_file: Path, build_dir: Path):
    if not schema_file.exists():
        raise FileNotFoundError(f"Schema file not found: {schema_file}")

    build_dir.mkdir(parents=True, exist_ok=True)
    schema_text = schema_file.read_text()
    tables, fks, reverse_fks, enums, models, protocols = parse_schema(schema_text)
    circular_deps = detect_circular_deps(fks)

    save_json(tables, build_dir / "tables.json")
    save_json(enums, build_dir / "enums.json")
    save_json(models, build_dir / "models.json")
    save_json(protocols, build_dir / "protocols.json")
    save_json({k: v for k, v in fks.items()}, build_dir / "foreign_keys.json")
    save_json({k: v for k, v in reverse_fks.items()}, build_dir / "reverse_fks.json")
    save_json(list(circular_deps), build_dir / "circular_deps.json")

    print(f"✅  Parsed schema and wrote JSON to: {build_dir}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Parse schema and generate JSON files.")
    parser.add_argument("--schema_file", type=Path, help="Path to the schema file")
    parser.add_argument("--build_dir", type=Path, help="Directory to save the generated JSON files")

    args = parser.parse_args()
    generate_parsed_schema_files(args.schema_file, args.build_dir)
    print(f"Parsing schema from {args.schema_file} and saving to {args.build_dir}")
    print("Done.")