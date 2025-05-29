import SwiftUI

struct ProductFormView: View {
    @ObservedObject var viewModel: ProductViewModel
    @Environment(\.dismiss) var dismiss

    @State private var name = ""
    @State private var category: Category = .Nocat
    @State private var notes = ""
    @State private var daysPerQuantity = ""
    @State private var quantityOrdered = ""
    @State private var startedUsingDate = Date()
    @State private var hasStartedUsing = false
    @State private var isOrdering = false
    @State private var expectedDeliveryDays = 3
    @State private var orderedDate = Date()

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Product Info")) {
                    TextField("Name", text: $name)
                    Picker("Category", selection: $category) {
                        ForEach(Category.allCases, id: \.self) { cat in
                            Text(cat.rawValue).tag(cat)
                        }
                    }
                }

                Section(header: Text("Usage Settings")) {
                    TextField("Days per Quantity", text: $daysPerQuantity)
                        .keyboardType(.decimalPad)

                    TextField("Quantity Ordered", text: $quantityOrdered)
                        .keyboardType(.decimalPad)

                    Toggle("Started using?", isOn: $hasStartedUsing)
                    if hasStartedUsing {
                        DatePicker("Start Date", selection: $startedUsingDate, displayedComponents: .date)
                    }
                }

                Section(header: Text("Notes")) {
                    TextField("Optional notes", text: $notes)
                }

                Section(header: Text("Order Info")) {
                    Toggle("Product is ordered", isOn: $isOrdering)
                    if isOrdering {
                        DatePicker("Ordered Date", selection: $orderedDate, displayedComponents: .date)
                        Stepper("Expected in \(expectedDeliveryDays) days", value: $expectedDeliveryDays, in: 1...30)
                    }
                }

                Section {
                    Button("Save") {
                        save()
                    }
                    .disabled(!isValid)
                }
            }
            .navigationTitle("Add Product")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") { dismiss() }
                }
            }
        }
    }

    private var isValid: Bool {
        !name.isEmpty && Double(daysPerQuantity) != nil
    }

    private func save() {
        guard let dpq = Double(daysPerQuantity),
              let qty = Double(quantityOrdered) else {
            return
        }

        var newProduct = Product.create(
            name: name,
            category: category,
            notes: notes.isEmpty ? nil : notes,
            daysPerQuantity: dpq,
            quantityOrdered: qty,
            startedUsingDate: hasStartedUsing ? startedUsingDate : nil
        )

        if isOrdering {
            newProduct.applyOrder(
                orderedDate: orderedDate,
                expectedDeliveryDays: expectedDeliveryDays,
                quantityOrdered: qty
            )
        }

        viewModel.addProduct(newProduct)
        dismiss()
    }
}
