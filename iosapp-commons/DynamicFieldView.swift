import SwiftUI
import Foundation

struct DisplayFieldInfo {
    let label: String
    let type: String
}

protocol DisplayableModel: Identifiable, Codable, Hashable {
    func getDisplayNames() -> [String: DisplayFieldInfo]
}

struct GenericFormBuilder<T: DisplayableModel>: View {
    @Binding var model: T
    @Binding var storage: [String: Any]
    private var displayNames: [String: DisplayFieldInfo] = [:]

    init(model: Binding<T>, storage: Binding<[String: Any]>) {
        self._model = model
        self._storage = storage
        self.displayNames = model.wrappedValue.getDisplayNames()
    }

    var body: some View {
        Form {
            ForEach(Array(displayNames.keys), id: \.self) { key in
                if let field = displayNames[key], let value = storage[key] {
                    DynamicFieldView(label: field.label, value: value, storage: $storage, key: key)
                }
            }
        }
        .onAppear {
            initializeStorage()
        }
    }

    private func initializeStorage() {
        guard storage.isEmpty else { return }  // prevent overwriting existing storage
        guard let data = try? JSONEncoder().encode(model),
              let dict = try? JSONSerialization.jsonObject(with: data) as? [String: Any] else {
            print("❌ Failed to encode model")
            return
        }
        storage = dict
    }
}

struct DynamicFieldView: View {
    let label: String
    let value: Any
    @Binding var storage: [String: Any]
    let key: String

    var body: some View {
        HStack {
            Text(label.capitalized)
                .frame(maxWidth: .infinity, alignment: .leading)

            fieldForValue()
                .frame(maxWidth: .infinity, alignment: .trailing)
        }
        .padding(.vertical, 4)
    }

    @ViewBuilder
    private func fieldForValue() -> some View {
        if let stringValue = value as? String {
            TextField("", text: Binding(
                get: { storage[key] as? String ?? stringValue },
                set: { storage[key] = $0 }
            ))
            .textFieldStyle(RoundedBorderTextFieldStyle())
        } else if let intValue = value as? Int {
            TextField("", text: Binding(
                get: { String(storage[key] as? Int ?? intValue) },
                set: { storage[key] = Int($0) ?? intValue }
            ))
            .keyboardType(.numberPad)
            .textFieldStyle(RoundedBorderTextFieldStyle())
        } else if let doubleValue = value as? Double {
            TextField("", text: Binding(
                get: { String(storage[key] as? Double ?? doubleValue) },
                set: { storage[key] = Double($0) ?? doubleValue }
            ))
            .keyboardType(.decimalPad)
            .textFieldStyle(RoundedBorderTextFieldStyle())
        } else if let boolValue = value as? Bool {
            Toggle("", isOn: Binding(
                get: { storage[key] as? Bool ?? boolValue },
                set: { storage[key] = $0 }
            ))
        } else {
            Text("N/A")
                .foregroundColor(.gray)
        }
    }
}
