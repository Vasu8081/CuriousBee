import json
from pathlib import Path

def to_pascal(name): return ''.join(w.capitalize() for w in name.split('_'))

def generate_swift_viewmodels(build_dir: str, output_dir: str):
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    swift_type_map = {
        "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Date",
        "TIMESTAMP": "Date", "INTEGER": "Int", "FLOAT": "Double", "BOOLEAN": "Bool",
        "BYTEA": "Data", "BYTEA[]": "[Data]", "JSON": "String", "JSONB": "String"
    }

    tables = json.loads((Path(build_dir) / "tables.json").read_text())
    enums = json.loads((Path(build_dir) / "enums.json").read_text())
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
                if swift_type == "String" and typ in enums.keys():
                    swift_type = f"{to_pascal(typ)}"
                # if name == "id":
                #     vf.write(f"    @Published var id: UUID = UUID()\n")
                # else:
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
                    if field == "id":
                        vf.write(f"    @Published var {field}: UUID = UUID()\n")
                        continue
                    if rfk["type"] in {"many_to_one", "many_to_many"}:
                        vf.write(f"    @Published var {field}: [{tgt}ViewModel] = []\n")
                    else:
                        vf.write(f"    @Published var {field}: {tgt}ViewModel?\n")

            # Initializer
            vf.write(f"\n    init(model: {struct_name}) {{\n")
            vf.write("        fromModel(model)\n    }\n")

            # fromModel
            vf.write(f"\n    func fromModel(_ model: {struct_name}) {{\n")
            for name, typ, *mods in cols:
                mods = mods[0] if mods else []
                swift_type = swift_type_map.get(typ.upper(), "String")
                if "HIDDEN" in [m.upper() for m in mods]: continue
                if swift_type == "String" and typ in enums.keys():
                    swift_type = f"{to_pascal(typ)}"
                if swift_type == "Data":
                    vf.write(f"        self.{name} = model.{name}?.decrypt()\n")
                else:
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


                    # toModel
            vf.write(f"\n    var toModel: {struct_name} {{\n")
            vf.write(f"        return {struct_name}(\n")
            model_fields = []
            for col in cols:
                if len(col) == 3:
                    col_name, col_type, modifiers = col
                elif len(col) == 2:
                    col_name, col_type = col
                    modifiers = []
                else:
                    continue
                if "HIDDEN" in [m.upper() for m in modifiers]:
                    continue
                field_name = col_name
                if col_type.upper() == "BYTEA":
                    model_fields.append(f"{field_name}: {field_name}?.encrypt()")
                else:
                    model_fields.append(f"{field_name}: {field_name}")
            for fk in fks.get(table, []):
                if fk.get("pydantic_refer") == "refer_left":
                    rel_type = fk["type"]
                    field_name = fk["left_field"]
                    if rel_type in {"many_to_one", "one_to_one"}:
                        model_fields.append(f"{field_name}: {field_name}?.toModel")
                    elif rel_type in {"one_to_many", "many_to_many"}:
                        model_fields.append(f"{field_name}: {field_name}.map {{ $0.toModel }}")
            for rfk in reverse_fks.get(table, []):
                if rfk.get("pydantic_refer") == "refer_right":
                    rel_type = rfk["type"]
                    field_name = rfk["right_field"]
                    if rel_type in {"many_to_one", "many_to_many"}:
                        model_fields.append(f"{field_name}: {field_name}.map {{ $0.toModel }}")
                    elif rel_type in {"one_to_one", "one_to_many"}:
                        model_fields.append(f"{field_name.rstrip('s')}: {field_name.rstrip('s')}?.toModel")
            vf.write("            " + ",\n            ".join(model_fields) + "\n")
            vf.write("        )\n")
            vf.write("    }\n")

            vf.write(f"\n    static func == (lhs: {vm_name}, rhs: {vm_name}) -> Bool {{ lhs.id == rhs.id }}\n\n")
            vf.write("    func hash(into hasher: inout Hasher) { hasher.combine(id) }\n\n")

            vf.write(f"    func save() {{\n")
            vf.write(f"        ServerEndPoints.shared.addOrUpdate{struct_name}(self.toModel) {{ result in\n")
            vf.write(f"            switch result {{\n")
            vf.write(f"            case .success(let entry):\n")
            vf.write(f"                print(\"Successfully saved {struct_name}\")\n")
            vf.write(f"                self.fromModel(entry)\n")
            vf.write(f"            case .failure(let error):\n")
            vf.write(f"                print(\"Failed to save {struct_name}: \\(error)\")\n")
            vf.write(f"            }}\n")
            vf.write(f"        }}\n")
            vf.write(f"    }}\n\n")
            vf.write(f"}}\n")

        print(f"✅ ViewModel written: {file_path}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Swift ViewModel classes from database schema")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing schema files")
    parser.add_argument("--output-dir", type=str, required=True, help="Path to output directory for generated ViewModels")

    args = parser.parse_args()
    generate_swift_viewmodels(args.build_dir, args.output_dir)
    print("✅ Swift ViewModels generated successfully.")