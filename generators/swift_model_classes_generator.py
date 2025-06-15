import json
from pathlib import Path
from collections import defaultdict

swift_type_map = {
    "UUID": "UUID", "TEXT": "String", "DATE": "Date", "TIME": "Date",
    "TIMESTAMP": "Date", "INTEGER": "Int", "FLOAT": "Double", "BOOLEAN": "Bool",
    "BYTEA": "Data", "BYTEA[]": "[Data]", "JSON": "String", "JSONB": "String",
}

swift_default_values = {
    "UUID": "UUID()",
    "TEXT": '""',
    "DATE": "Date()",
    "TIME": "Date()",
    "TIMESTAMP": "Date()",
    "INTEGER": "0",
    "FLOAT": "0.0",
    "BOOLEAN": "false",
    "BYTEA": "Data()",
    "BYTEA[]": "[]",
    "JSON": '""',
    "JSONB": '""',
}

def to_pascal(name):
    return ''.join(w.capitalize() for w in name.split('_'))

def to_enum_name(name):
    name = name.split('_')
    name = [part.lower().capitalize() for part in name]
    return ''.join(name)

def generate_protocols(protocols: dict, output_path: Path):
    with open(output_path / "Protocols.swift", 'w') as f:
        f.write("import Foundation\n\n")
        for proto_name, fields in protocols.items():
            proto_type = to_pascal(proto_name)
            f.write(f"protocol {proto_type}: Codable, Hashable {{\n")
            for name, typ in fields:
                swift_type = swift_type_map.get(typ.upper(), "String")
                f.write(f"    var {name}: {swift_type} {{ get }}\n")
            f.write("}\n\n")
        print(f"✅ Generated protocols: {output_path / 'Protocols.swift'}")

def generate_models_and_collect_factory_data(models: dict, output_path: Path):
    protocol_to_models = defaultdict(list)

    for model_key, content in models.items():
        struct_name = to_pascal(model_key)
        parents = content.get("parents", [])
        fields = content.get("fields", [])

        conformances = ", ".join([to_pascal(p) for p in parents] + ["Codable", "Hashable", "DisplayableModel"])
        file_path = output_path / f"{struct_name}.swift"

        # Track models for each protocol
        for parent in parents:
            protocol_to_models[parent].append((struct_name, model_key))

        with open(file_path, 'w') as f:
            f.write("import Foundation\n\n")
            f.write(f"struct {struct_name}: {conformances} {{\n")

            for name, typ, mods in fields:
                if name == "id":
                    f.write("    var id: UUID = UUID()\n")
                    continue
                swift_type = swift_type_map.get(typ.upper(), "String")
                default_value = swift_default_values.get(typ.upper(), "")
                f.write(f"    var {name}: {swift_type} = {default_value}\n")

            f.write("\n")
            f.write("    enum CodingKeys: String, CodingKey {\n")
            for name, _, _ in fields:
                f.write(f"        case {name}\n")
            f.write("    }\n\n")

            f.write("    init(from decoder: Decoder) throws {\n")
            f.write("        let container = try decoder.container(keyedBy: CodingKeys.self)\n")
            for name, typ, _ in fields:
                if name == "id":
                    f.write(f"        self.{name} = try container.decodeIfPresent(UUID.self, forKey: .{name}) ?? UUID()\n")
                    continue
                swift_type = swift_type_map.get(typ.upper(), "String")
                default_value = swift_default_values.get(typ.upper(), "")
                f.write(f"        self.{name} = try container.decodeIfPresent({swift_type}.self, forKey: .{name}) ?? {default_value}\n")
            f.write("    }\n\n")

            f.write("    init() {\n")
            f.write("    }\n\n")

            f.write("    func getDisplayNames() -> [String: DisplayFieldInfo] {\n")
            f.write("        return [\n")
            for name, typ, mods in fields:
                if mods.strip():
                    swift_type = swift_type_map.get(typ.upper(), "String")
                    f.write(f"            \"{name}\": DisplayFieldInfo(label: \"{mods}\", type: \"{swift_type}\"),\n")
            f.write("        ]\n")
            f.write("    }\n")

            f.write("}\n")

        print(f"✅ Generated model: {file_path.name}")

    return protocol_to_models

def generate_protocol_factory(protocol_name, models, output_path: Path):
    factory_name = f"{to_pascal(protocol_name)}Factory"
    enum_name = f"{to_pascal(protocol_name)}Types"
    file_path = output_path / f"{factory_name}.swift"

    with open(file_path, 'w') as f:
        f.write("import Foundation\n\n")
        f.write(f"enum {factory_name} {{\n")
        f.write(f"    static func create(from data: Data, type: {enum_name}, id: UUID) -> (any {to_pascal(protocol_name)})? {{\n")
        f.write("        switch type {\n")

        for struct_name, case_name in models:
            enum_case = to_enum_name(case_name)
            f.write(f"        case .{enum_case}:\n")
            f.write(f"            do {{\n")
            f.write(f"                var account = try JSONDecoder().decode({struct_name}.self, from: data)\n")
            f.write(f"                account.id = id\n")
            f.write(f"                return account\n")
            f.write(f"            }} catch {{\n")
            f.write(f"                print(\"❌ Failed to decode {struct_name}:\", error)\n")
            f.write(f"            }}\n")
            f.write(f"            return nil\n")

        f.write("        }\n")
        f.write("    }\n")
        f.write("}\n")

    print(f"✅ Generated factory: {file_path.name}")

def generate_account_models(build_dir: str, output_dir: str):
    output_path = Path(output_dir)
    output_path.mkdir(parents=True, exist_ok=True)

    models_file = Path(build_dir) / "models.json"
    protocols_file = Path(build_dir) / "protocols.json"

    with open(models_file, 'r') as f:
        models = json.load(f)
    with open(protocols_file, 'r') as f:
        protocols = json.load(f)

    generate_protocols(protocols, output_path)
    protocol_to_models = generate_models_and_collect_factory_data(models, output_path)

    for protocol_name, model_entries in protocol_to_models.items():
        generate_protocol_factory(protocol_name, model_entries, output_path)

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Swift models, protocols, and protocol-specific factories from JSON.")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing models.json and protocols.json.")
    parser.add_argument("--output-dir", type=str, required=True, help="Output directory for Swift files.")

    args = parser.parse_args()
    generate_account_models(args.build_dir, args.output_dir)