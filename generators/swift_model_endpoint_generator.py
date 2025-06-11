import json
from pathlib import Path

def to_pascal(name): return ''.join(w.capitalize() for w in name.split('_'))

def generate_swift_endpoints(build_dir: str, output_dir: str):
    Path(output_dir).mkdir(parents=True, exist_ok=True)

    tables = json.loads((Path(build_dir) / "tables.json").read_text())

    for table, cols in tables.items():
        struct_name = to_pascal(table)
        endpoint_file = Path(output_dir) / f"{struct_name}Endpoint.swift"

        id_field = next((col[0] for col in cols if len(col) == 3 and "primary" in [m.lower() for m in col[2]]), None)
        if not id_field:
            print(f"Skipping {struct_name}: No primary key found.")
            continue

        with open(endpoint_file, "w") as f:
            f.write("import Foundation\n\n")
            f.write(f"extension ServerEndPoints {{\n\n")

            # GET by ID
            f.write(f"    func get{struct_name}(id: String, completion: @escaping (Result<{struct_name}, Error>) -> Void) {{\n")
            f.write("        guard let token = AuthenticateViewModel.shared.getToken() else {\n")
            f.write("            completion(.failure(ServerError.missingToken))\n            return\n        }\n")
            f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/get/\\(id)\") else {{\n")
            f.write("            completion(.failure(ServerError.invalidURL))\n            return\n        }\n")
            f.write("        var request = URLRequest(url: url)\n")
            f.write("        request.httpMethod = \"GET\"\n")
            f.write("        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
            f.write(f"        send(request, expecting: {struct_name}.self, completion: completion)\n")
            f.write("    }\n\n")

            # POST
            f.write(f"    func addOrUpdate{struct_name}(_ obj: {struct_name}, completion: @escaping (Result<{struct_name}, Error>) -> Void) {{\n")
            f.write("        guard let token = AuthenticateViewModel.shared.getToken() else {\n")
            f.write("            completion(.failure(ServerError.missingToken))\n            return\n        }\n")
            f.write("        let id = AuthenticateViewModel.shared.getGroupId()\n")
            f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id)\") else {{\n")
            f.write("            completion(.failure(ServerError.invalidURL))\n            return\n        }\n")
            f.write("        guard let body = try? encodeModel(obj) else {\n")
            f.write("            completion(.failure(ServerError.noData))\n            return\n        }\n")
            f.write("        var request = URLRequest(url: url)\n")
            f.write("        request.httpMethod = \"POST\"\n")
            f.write("        request.httpBody = body\n")
            f.write("        request.setValue(\"application/json\", forHTTPHeaderField: \"Content-Type\")\n")
            f.write("        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
            f.write(f"        send(request, expecting: {struct_name}.self, completion: completion)\n")
            f.write("    }\n\n")

            # GET All
            f.write(f"    func getAll{struct_name}(completion: @escaping (Result<[{struct_name}], Error>) -> Void) {{\n")
            f.write("        guard let token = AuthenticateViewModel.shared.getToken() else {\n")
            f.write("            completion(.failure(ServerError.missingToken))\n            return\n        }\n")
            f.write("        let id = AuthenticateViewModel.shared.getGroupId()\n")
            f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id)\") else {{\n")
            f.write("            completion(.failure(ServerError.invalidURL))\n            return\n        }\n")
            f.write("        var request = URLRequest(url: url)\n")
            f.write("        request.httpMethod = \"GET\"\n")
            f.write("        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
            f.write(f"        send(request, expecting: [{struct_name}].self, completion: completion)\n")
            f.write("    }\n\n")

            # DELETE
            f.write(f"    func delete{struct_name}(id: UUID, completion: @escaping (Result<String, Error>) -> Void) {{\n")
            f.write("        guard let token = AuthenticateViewModel.shared.getToken() else {\n")
            f.write("            completion(.failure(ServerError.missingToken))\n            return\n        }\n")
            f.write(f"        guard let url = URL(string: \"\\(baseURL){table}/\\(id.uuidString)\") else {{\n")
            f.write("            completion(.failure(ServerError.invalidURL))\n            return\n        }\n")
            f.write("        var request = URLRequest(url: url)\n")
            f.write("        request.httpMethod = \"DELETE\"\n")
            f.write("        request.setValue(\"Bearer \\(token)\", forHTTPHeaderField: \"Authorization\")\n")
            f.write("        send(request, expecting: String.self, completion: completion)\n")
            f.write("    }\n")

            f.write("}\n")

        print(f"✅ Endpoint written: {endpoint_file}")

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate Swift endpoints from database schema.")
    parser.add_argument("--build-dir", type=str, required=True, help="Path to the build directory containing tables.json.")
    parser.add_argument("--output-dir", type=str, required=True, help="Path to the output directory for generated Swift endpoints.")

    args = parser.parse_args()
    generate_swift_endpoints(args.build_dir, args.output_dir)
    print("✅ All Swift endpoints generated successfully.")