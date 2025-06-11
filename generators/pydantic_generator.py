import os
import json
from pathlib import Path

PYTHON_TYPES = {
    "UUID": "uuid.UUID", "TEXT": "str", "DATE": "datetime.date", "TIME": "datetime.time",
    "TIMESTAMP": "datetime.datetime", "INTEGER": "int", "FLOAT": "float", "BOOLEAN": "bool"
}

def to_pascal(name): return ''.join(w.capitalize() for w in name.split('_'))
def to_camel(name): return name.split('_')[0] + ''.join(w.capitalize() for w in name.split('_')[1:])

def generate_pydantic_schemas(build_dir: str, output_dir: str):
    build_path = Path(build_dir)
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    tables = json.loads((build_path / "tables.json").read_text())
    fks = json.loads((build_path / "foreign_keys.json").read_text())
    reverse_fks = json.loads((build_path / "reverse_fks.json").read_text())

    for table, columns in tables.items():
        path = output_path / f"{table}_schema.py"
        class_name = to_pascal(table)

        with path.open("w") as f:
            f.write("import uuid\nimport datetime\n")
            f.write("from pydantic import BaseModel, Field, ConfigDict\n")
            f.write("from typing import Optional, List, ForwardRef\n\n")

            related = set()
            for fk in fks.get(table, []):
                related.add(to_pascal(fk["tgt_table"]) + "Schema")
            for rfk in reverse_fks.get(table, []):
                related.add(to_pascal(rfk["src_table"]) + "Schema")

            for rc in sorted(related):
                f.write(f"{rc} = ForwardRef('{rc}')\n")
            f.write("\n")

            f.write(f"class {class_name}Schema(BaseModel):\n")
            for col_name, col_type, modifiers in columns:
                if "HIDDEN" in [m.upper() for m in modifiers]:
                    continue
                py_type = PYTHON_TYPES.get(col_type.upper(), "str")
                f.write(f"    {col_name.lstrip('_')}: Optional[{py_type}] = Field(default=None, alias='{col_name}')\n")

            # FK relationships
            for fk in fks.get(table, []):
                if fk["pydantic_refer"] == "refer_left":
                    target_class = to_pascal(fk["tgt_table"]) + "Schema"
                    field = fk["right_field"]
                    if fk["type"] in {"many_to_one", "one_to_one"}:
                        f.write(f"    {field}: Optional['{target_class}'] = None\n")
                    else:
                        f.write(f"    {field}: List['{target_class}'] = []\n")

            # Reverse FK relationships
            for rfk in reverse_fks.get(table, []):
                if rfk["pydantic_refer"] == "refer_right":
                    target_class = to_pascal(rfk["src_table"]) + "Schema"
                    field = rfk["right_field"]
                    if rfk["type"] in {"many_to_one", "many_to_many"}:
                        f.write(f"    {field}: List['{target_class}'] = []\n")
                    else:
                        f.write(f"    {field}: Optional['{target_class}'] = None\n")

            # Config + to_model
            f.write("\n")
            f.write("    model_config = ConfigDict(from_attributes=True, populate_by_name=True)\n\n")

            f.write(f"    def to_model(self) -> '{class_name}':\n")
            f.write(f"        from app.autogen.models.{table} import {class_name}\n")
            f.write(f"        model = {class_name}(\n")

            init_fields = [
                f"{col} = self.{col.lstrip('_')}"
                for col, col_type, mods in columns
                if "HIDDEN" not in [m.upper() for m in mods]
            ]
            f.write("            " + ",\n            ".join(init_fields) + "\n        )\n")

            for fk in fks.get(table, []):
                if fk["pydantic_refer"] == "refer_left":
                    tgt_model = to_pascal(fk["tgt_table"])
                    field = fk["right_field"]
                    f.write(f"        from app.autogen.models.{fk['tgt_table']} import {tgt_model}\n")
                    if fk["type"] in {"many_to_one", "one_to_one"}:
                        f.write(f"        if self.{field} is not None:\n")
                        f.write(f"            model.{field} = self.{field}.to_model()\n")
                    else:
                        f.write(f"        model.{field} = [obj.to_model() for obj in self.{field}]\n")

            for rfk in reverse_fks.get(table, []):
                if rfk["pydantic_refer"] == "refer_right":
                    src_model = to_pascal(rfk["src_table"])
                    field = rfk["right_field"]
                    f.write(f"        from app.autogen.models.{rfk['src_table']} import {src_model}\n")
                    if rfk["type"] in {"many_to_one", "many_to_many"}:
                        f.write(f"        model.{field} = [obj.to_model() for obj in self.{field}]\n")
                    else:
                        f.write(f"        model.{field} = self.{field}.to_model()\n")

            f.write("        return model\n")

    # Generate __init__.py
    with open(output_path / "__init__.py", "w") as f:
        for table in tables:
            class_name = to_pascal(table)
            f.write(f"from .{table}_schema import {class_name}Schema\n")
        for table in tables:
            class_name = to_pascal(table)
            f.write(f"{class_name}Schema.model_rebuild(_types_namespace=globals())\n")

    print("✅ Pydantic schemas generated successfully.")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Pydantic schemas from database schema.")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing tables.json and foreign_keys.json.")
    parser.add_argument("--output-dir", type=str, required=True, help="Path to the output directory for generated schemas.")

    args = parser.parse_args()
    print(f"Generating Pydantic schemas in {args.output_dir}...")
    generate_pydantic_schemas(args.build_dir, args.output_dir)
    print("Pydantic schemas generation completed.")