import SwiftUI

struct AddCategoryFormView: View {
    @Environment(\.dismiss) var dismiss
    var onSave: (Category) -> Void

    @State private var name: String = ""
    @State private var emoji: String = ""
    @State private var type: CategoryType = .expense

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Details")) {
                    TextField("Emoji (optional)", text: $emoji)
                    TextField("Category Name", text: $name)

                    Picker("Category Type", selection: $type) {
                        ForEach(CategoryType.allCases, id: \.self) { type in
                            Text(type.rawValue.capitalized).tag(type)
                        }
                    }
                    .pickerStyle(SegmentedPickerStyle())
                }
            }
            .navigationTitle("New Category")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let category = Category(
                            id: UUID(),
                            name: name,
                            type: type,
                            emoji: emoji.isEmpty ? nil : emoji
                        )
                        onSave(category)
                        dismiss()
                    }
                    .disabled(name.isEmpty)
                }
            }
        }
    }
}
