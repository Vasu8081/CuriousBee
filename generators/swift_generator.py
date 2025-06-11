# utilities/generate_swift_models.py
import os
import json
from pathlib import Path

swift_type_map = {
    "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Date",
    "TIMESTAMP": "Date", "INTEGER": "Int", "FLOAT": "Double", "BOOLEAN": "Bool"
}

def to_pascal(name): return ''.join(w.capitalize() for w in name.split('_'))

def generate_swift_models(build_dir: str, output_dir: str):
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    tables = json.loads((Path(build_dir) / "tables.json").read_text())
    fks = json.loads((Path(build_dir) / "foreign_keys.json").read_text())
    reverse_fks = json.loads((Path(build_dir) / "reverse_fks.json").read_text())

    for table, cols in tables.items():
        struct_name = to_pascal(table)
        file_path = Path(output_dir) / f"{struct_name}.swift"

        with open(file_path, "w") as f:
            f.write("import Foundation\n\n")
            f.write(f"struct {struct_name}: Codable, Identifiable, Hashable, BeeCodableModel {{\n")

            for col in cols:
                name, typ, mods = col if len(col) == 3 else (*col, [])
                if "HIDDEN" in [m.upper() for m in mods]: continue
                swift_type = swift_type_map.get(typ.upper(), "String")
                f.write(f"    var {name}: {swift_type}?\n")

            for fk in fks.get(table, []):
                if fk["pydantic_refer"] == "refer_left":
                    rel = to_pascal(fk["tgt_table"])
                    name = fk["left_field"]
                    f.write(f"    var {name}: {rel}?\n")

            for rfk in reverse_fks.get(table, []):
                if rfk["pydantic_refer"] == "refer_right":
                    rel = to_pascal(rfk["src_table"])
                    name = rfk["right_field"]
                    f.write(f"    var {name}: [{rel}] = []\n")

            f.write("\n    static var serverTypeMap: [String:String] {\n")
            f.write("        return [\n")
            for col in cols:
                name, typ, mods = col if len(col) == 3 else (*col, [])
                if "HIDDEN" in [m.upper() for m in mods]: continue
                f.write(f"            \"{name}\": \"{typ.upper()}\",\n")
            f.write("        ]\n    }\n")

            f.write("\n    var id: UUID {\n")
            f.write("        return _id ?? UUID()\n    }\n")
            f.write("}\n")

        print(f"✅ Generated Swift model: {file_path}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Swift models from database schema.")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing tables.json, foreign_keys.json, and reverse_fks.json.")
    parser.add_argument("--output-dir", type=str, required=True, help="Path to the output directory for generated Swift models.")

    args = parser.parse_args()
    generate_swift_models(args.build_dir, args.output_dir)
    print("✅ All Swift models generated successfully.")