import json
from pathlib import Path

def to_pascal(name): return ''.join(w.capitalize() for w in name.split('_'))

def generate_swift_viewmodels(build_dir: str, output_dir: str):
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    swift_type_map = {
        "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Date",
        "TIMESTAMP": "Date", "INTEGER": "Int", "FLOAT": "Double", "BOOLEAN": "Bool"
    }

    tables = json.loads((Path(build_dir) / "tables.json").read_text())
    fks = json.loads((Path(build_dir) / "foreign_keys.json").read_text())
    reverse_fks = json.loads((Path(build_dir) / "reverse_fks.json").read_text())

    for table, cols in tables.items():
        struct_name = to_pascal(table)
        vm_name = f"{struct_name}ViewModel"
        file_path = Path(output_dir) / f"{vm_name}.swift"

        with open(file_path, "w") as vf:
            vf.write("import Foundation\nimport SwiftUI\n\n")
            vf.write(f"class {vm_name}: ObservableObject, Identifiable, Hashable {{\n")

            # Base fields
            for name, typ, *mods in cols:
                mods = mods[0] if mods else []
                if "HIDDEN" in [m.upper() for m in mods]: continue
                swift_type = swift_type_map.get(typ.upper(), "String")
                vf.write(f"    @Published var {name}: {swift_type}?\n")

            # Foreign keys
            for fk in fks.get(table, []):
                if fk["pydantic_refer"] == "refer_left":
                    tgt = to_pascal(fk["tgt_table"])
                    field = fk["left_field"]
                    if fk["type"] in {"many_to_one", "one_to_one"}:
                        vf.write(f"    @Published var {field}: {tgt}ViewModel?\n")
                    else:
                        vf.write(f"    @Published var {field}: [{tgt}ViewModel] = []\n")

            # Reverse foreign keys
            for rfk in reverse_fks.get(table, []):
                if rfk["pydantic_refer"] == "refer_right":
                    tgt = to_pascal(rfk["src_table"])
                    field = rfk["right_field"]
                    if rfk["type"] in {"many_to_one", "many_to_many"}:
                        vf.write(f"    @Published var {field}: [{tgt}ViewModel] = []\n")
                    else:
                        vf.write(f"    @Published var {field}: {tgt}ViewModel?\n")

            # Initializer
            vf.write(f"\n    init(model: {struct_name}) {{\n")
            vf.write("        fromModel(model)\n    }\n")

            # fromModel
            vf.write(f"\n    func fromModel(_ model: {struct_name}) {{\n")
            for name, _, *mods in cols:
                mods = mods[0] if mods else []
                if "HIDDEN" in [m.upper() for m in mods]: continue
                vf.write(f"        self.{name} = model.{name}\n")

            for fk in fks.get(table, []):
                if fk["pydantic_refer"] == "refer_left":
                    tgt = to_pascal(fk["tgt_table"])
                    field = fk["left_field"]
                    if fk["type"] in {"many_to_one", "one_to_one"}:
                        vf.write(f"        if let value = model.{field} {{ self.{field} = {tgt}ViewModel(model: value) }}\n")
                    else:
                        vf.write(f"        self.{field} = model.{field}.map {{ {tgt}ViewModel(model: $0) }}\n")

            for rfk in reverse_fks.get(table, []):
                if rfk["pydantic_refer"] == "refer_right":
                    tgt = to_pascal(rfk["src_table"])
                    field = rfk["right_field"]
                    if rfk["type"] in {"many_to_one", "many_to_many"}:
                        vf.write(f"        self.{field} = model.{field}.map {{ {tgt}ViewModel(model: $0) }}\n")
                    else:
                        vf.write(f"        if let value = model.{field} {{ self.{field} = {tgt}ViewModel(model: value) }}\n")
            vf.write("    }\n")

            # Required methods
            vf.write(f"\n    var id: UUID {{ return _id ?? UUID() }}\n")
            vf.write(f"\n    static func == (lhs: {vm_name}, rhs: {vm_name}) -> Bool {{ lhs.id == rhs.id }}\n")
            vf.write("    func hash(into hasher: inout Hasher) { hasher.combine(id) }\n")
            vf.write("}\n")

        print(f"✅ ViewModel written: {file_path}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Swift ViewModel classes from database schema")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing schema files")
    parser.add_argument("--output-dir", type=str, required=True, help="Path to output directory for generated ViewModels")

    args = parser.parse_args()
    generate_swift_viewmodels(args.build_dir, args.output_dir)
    print("✅ Swift ViewModels generated successfully.")