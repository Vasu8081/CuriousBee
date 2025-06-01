import os
import re
from pathlib import Path
from collections import defaultdict

SCHEMA_FILE = "app/app.schema"
SWIFT_OUTPUT_DIR = "BeeTracker/BeeTracker/Autogen/Models"
os.makedirs(SWIFT_OUTPUT_DIR, exist_ok=True)
SWIFT_VIEW_MODEL_OUTPUT_DIR = "BeeTracker/BeeTracker/Autogen/ViewModels"
os.makedirs(SWIFT_VIEW_MODEL_OUTPUT_DIR, exist_ok=True)
SWIFT_ENDPOINTS_DIR = "BeeTracker/BeeTracker/Autogen/Endpoints"
os.makedirs(SWIFT_ENDPOINTS_DIR, exist_ok=True)

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
        import shlex

        parts = shlex.split(line)
        col_name = parts[0]
        col_type = parts[1]
        raw_modifiers = parts[2:] if len(parts) > 2 else []

        # Flatten and clean modifier tags like [primary key] or [hidden]
        modifiers = []
        for mod in raw_modifiers:
            mod = mod.strip("[]").lower()
            modifiers.extend(mod.split())
        # parts = line.split()
        # col_name = parts[0]
        # col_type = parts[1]
        # modifiers = parts[2:] if len(parts) > 2 else []
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
    return col_name
    # return col_name.lstrip("_")

for table, cols in tables.items():
    struct_name = to_swift_struct_name(table)
    file_path = os.path.join(SWIFT_OUTPUT_DIR, f"{struct_name}.swift")

    with open(file_path, "w") as f:
        f.write("import Foundation\n\n")
        f.write(f"struct {struct_name}: Codable, Identifiable, Hashable, BeeCodableModel {{\n")

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
                field_name = fk["src_col"][1:-3]
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
                    f.write(f"    var {field_name.rstrip('s')}: {src_struct}?\n\n")


       # Regular fields
        f.write(f"    static var serverTypeMap: [String:String] {{\n")
        f.write("        return [\n")
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
            f.write(f"            \"{swift_field}\": \"{col_type.upper()}\",\n")
        f.write("        ]\n")
        f.write("    }\n\n")

        f.write("    var id: UUID {\n")
        f.write("        return _id ?? UUID()\n")
        f.write("    }\n\n")

        f.write("}\n")

    print(f"Generated Swift struct for {table} at {file_path}")
    # --- ViewModel generation ---
    vm_name = f"{struct_name}ViewModel"
    vm_path = os.path.join(SWIFT_VIEW_MODEL_OUTPUT_DIR, f"{vm_name}.swift")
    with open(vm_path, "w") as vf:
        vf.write("import Foundation\n")
        vf.write("import SwiftUI\n\n")
        vf.write(f"class {vm_name}: ObservableObject, Identifiable, Hashable {{\n")
        
        # Base columns
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
            swift_type = swift_type_map.get(col_type.upper(), "String")
            field_name = to_swift_field_name(col_name)
            vf.write(f"    @Published var {field_name}: {swift_type}?\n")

        # Foreign keys (refer_left)
        for fk in fks.get(table, []):
            if fk.get("pydantic_refer") == "refer_left":
                tgt_struct = to_swift_struct_name(fk["tgt_table"])
                rel_type = fk["type"]
                field_name = fk["src_col"][1:-3]
                if rel_type in {"many_to_one", "one_to_one"}:
                    vf.write(f"    @Published var {field_name}: {tgt_struct}ViewModel?\n")
                elif rel_type in {"one_to_many", "many_to_many"}:
                    vf.write(f"    @Published var {field_name}: [{tgt_struct}ViewModel] = []\n")

        # Reverse FKs (refer_right)
        for rfk in reverse_fks.get(table, []):
            if rfk.get("pydantic_refer") == "refer_right":
                src_struct = to_swift_struct_name(rfk["src_table"])
                rel_type = rfk["type"]
                field_name = rfk["src_table"]
                if rel_type in {"many_to_one", "many_to_many"}:
                    vf.write(f"    @Published var {field_name}: [{src_struct}ViewModel] = []\n")
                elif rel_type in {"one_to_one", "one_to_many"}:
                    vf.write(f"    @Published var {field_name.rstrip('s')}: {src_struct}ViewModel?\n")

        # Initializer
        vf.write(f"\n    init(model: {struct_name}) {{\n")
        vf.write(f"        fromModel(model)\n")
        vf.write("    }\n")

        # fromModel method
        vf.write(f"\n    func fromModel(_ model: {struct_name}) {{\n")
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
            field_name = to_swift_field_name(col_name)
            vf.write(f"        self.{field_name} = model.{field_name}\n")

        for fk in fks.get(table, []):
            if fk.get("pydantic_refer") == "refer_left":
                tgt_struct = to_swift_struct_name(fk["tgt_table"])
                rel_type = fk["type"]
                field_name = fk["src_col"][1:-3]
                if rel_type in {"many_to_one", "one_to_one"}:
                    vf.write(f"        if let value = model.{field_name} {{ self.{field_name} = {tgt_struct}ViewModel(model: value) }}\n")
                elif rel_type in {"one_to_many", "many_to_many"}:
                    vf.write(f"        self.{field_name} = model.{field_name}.map {{ {tgt_struct}ViewModel(model: $0) }}\n")

        for rfk in reverse_fks.get(table, []):
            if rfk.get("pydantic_refer") == "refer_right":
                src_struct = to_swift_struct_name(rfk["src_table"])
                rel_type = rfk["type"]
                field_name = rfk["src_table"]
                if rel_type in {"many_to_one", "many_to_many"}:
                    vf.write(f"        self.{field_name} = model.{field_name}.map {{ {src_struct}ViewModel(model: $0) }}\n")
                elif rel_type in {"one_to_one", "one_to_many"}:
                    vf.write(f"        if let value = model.{field_name.rstrip('s')} {{ self.{field_name.rstrip('s')} = {src_struct}ViewModel(model: value) }}\n")
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
            field_name = to_swift_field_name(col_name)
            model_fields.append(f"{field_name}: {field_name}")
        for fk in fks.get(table, []):
            if fk.get("pydantic_refer") == "refer_left":
                rel_type = fk["type"]
                field_name = fk["src_col"][1:-3]
                if rel_type in {"many_to_one", "one_to_one"}:
                    model_fields.append(f"{field_name}: {field_name}?.toModel")
                elif rel_type in {"one_to_many", "many_to_many"}:
                    model_fields.append(f"{field_name}: {field_name}.map {{ $0.toModel }}")
        for rfk in reverse_fks.get(table, []):
            if rfk.get("pydantic_refer") == "refer_right":
                rel_type = rfk["type"]
                field_name = rfk["src_table"]
                if rel_type in {"many_to_one", "many_to_many"}:
                    model_fields.append(f"{field_name}: {field_name}.map {{ $0.toModel }}")
                elif rel_type in {"one_to_one", "one_to_many"}:
                    model_fields.append(f"{field_name.rstrip('s')}: {field_name.rstrip('s')}?.toModel")
        vf.write("            " + ",\n            ".join(model_fields) + "\n")
        vf.write("        )\n")
        vf.write("    }\n")

        # id
        vf.write("\n    var id: UUID {\n")
        vf.write("        return _id ?? UUID()\n")
        vf.write("    }\n")

        # Hashable
        vf.write(f"\n    static func == (lhs: {vm_name}, rhs: {vm_name}) -> Bool {{\n")
        vf.write("        return lhs.id == rhs.id\n")
        vf.write("    }\n")
        vf.write("    func hash(into hasher: inout Hasher) {\n")
        vf.write("        hasher.combine(id)\n")
        vf.write("    }\n\n")

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
for table, cols in tables.items():
    struct_name = to_swift_struct_name(table)
    endpoint_file = os.path.join(SWIFT_ENDPOINTS_DIR, f"{struct_name}Endpoint.swift")

    id_field = None
    for col in cols:
        if "primary" in [m.lower() for m in col[2]]:
            id_field = to_swift_field_name(col[0])
            break
    if not id_field:
        print(f"Skipping {struct_name}: No primary key found.")
        continue

    with open(endpoint_file, "w") as f:
        f.write("import Foundation\n\n")
        f.write(f"extension ServerEndPoints {{\n\n")

        # GET by ID
        f.write(f"    func get{struct_name}(id: String, completion: @escaping (Result<{struct_name}, Error>) -> Void) {{\n")
        f.write(f"        guard let token = AuthenticateViewModel.shared.getToken() else {{\n")
        f.write(f"            completion(.failure(ServerError.missingToken))\n            return\n        }}\n")
        f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/get/\\(id)\") else {{\n")
        f.write(f"            completion(.failure(ServerError.invalidURL))\n            return\n        }}\n")
        f.write(f"        var request = URLRequest(url: url)\n")
        f.write(f"        request.httpMethod = \"GET\"\n")
        f.write(f"        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
        f.write(f"        send(request, expecting: {struct_name}.self, completion: completion)\n")
        f.write(f"    }}\n\n")

        # POST data
        f.write(f"    func addOrUpdate{struct_name}(_ obj: {struct_name}, completion: @escaping (Result<{struct_name}, Error>) -> Void) {{\n")
        f.write(f"        guard let token = AuthenticateViewModel.shared.getToken() else {{\n")
        f.write(f"            completion(.failure(ServerError.missingToken))\n            return\n        }}\n")
        f.write(f"        let id = AuthenticateViewModel.shared.getGroupId()\n")
        f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id)\") else {{\n")
        f.write(f"            completion(.failure(ServerError.invalidURL))\n            return\n        }}\n")
        f.write(f"        guard let body = try? encodeModel(obj) else {{\n")
        f.write(f"            completion(.failure(ServerError.noData))\n            return\n        }}\n")
        f.write(f"        var request = URLRequest(url: url)\n")
        f.write(f"        request.httpMethod = \"POST\"\n")
        f.write(f"        request.httpBody = body\n")
        f.write(f"        request.setValue(\"application/json\", forHTTPHeaderField: \"Content-Type\")\n")
        f.write(f"        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
        f.write(f"        send(request, expecting: {struct_name}.self, completion: completion)\n")
        f.write(f"    }}\n\n")

        # GET all
        f.write(f"    func getAll{struct_name}(completion: @escaping (Result<[{struct_name}], Error>) -> Void) {{\n")
        f.write(f"        guard let token = AuthenticateViewModel.shared.getToken() else {{\n")
        f.write(f"            completion(.failure(ServerError.missingToken))\n            return\n        }}\n")
        f.write(f"        let id = AuthenticateViewModel.shared.getGroupId()\n")
        f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id)\") else {{\n")
        f.write(f"            completion(.failure(ServerError.invalidURL))\n            return\n        }}\n")
        f.write(f"        var request = URLRequest(url: url)\n")
        f.write(f"        request.httpMethod = \"GET\"\n")
        f.write(f"        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
        f.write(f"        send(request, expecting: [{struct_name}].self, completion: completion)\n")
        f.write(f"    }}\n\n")

        # DELETE by ID
        f.write(f"    func delete{struct_name}(id: UUID, completion: @escaping (Result<String, Error>) -> Void) {{\n")
        f.write(f"        guard let token = AuthenticateViewModel.shared.getToken() else {{\n")
        f.write(f"            completion(.failure(ServerError.missingToken))\n            return\n        }}\n")
        f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id.uuidString)\") else {{\n")
        f.write(f"            completion(.failure(ServerError.invalidURL))\n            return\n        }}\n")
        f.write(f"        var request = URLRequest(url: url)\n")
        f.write(f"        request.httpMethod = \"DELETE\"\n")
        f.write(f"        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
        f.write(f"        send(request, expecting: String.self, completion: completion)\n")
        f.write(f"    }}\n")

        f.write("}\n")

    print(f"Generated endpoint for {table} -> {endpoint_file}")


    print(f"Generated ViewModel for {table} at {vm_path}")
    print(f"Generated Swift struct for {table} at {file_path}")