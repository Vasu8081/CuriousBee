import SwiftUI

struct ProductAddForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject var productViewModel: ProductViewModel

    var viewModel: ProductInfosViewModel

    @State private var quantity: String = ""
    @State private var startedDate: Date = Date()
    @State private var completedDate: Date? = nil
    @State private var price: String = ""
    @State private var selectedStatusId: UUID? = nil

    // Order-related fields
    @State private var orderedDate: Date = Date()
    @State private var expectedDeliveryDate: Date = Calendar.current.date(byAdding: .day, value: 5, to: Date()) ?? Date()
    @State private var receivedDate: Date? = nil

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Product Info")) {
                    HStack {
                        Text("Days per Quantity")
                        Spacer()
                        TextField("Days", text: Binding(
                            get: {
                                if let val = viewModel._days_per_quantity {
                                    return String(format: "%.2f", val)
                                } else {
                                    return ""
                                }
                            },
                            set: { newValue in
                                viewModel._days_per_quantity = Double(newValue)
                            }
                        ))
                        .keyboardType(.decimalPad)
                        .multilineTextAlignment(.trailing)
                        .frame(width: 80)
                    }

                    HStack {
                        Text("Unit Size")
                        Spacer()
                        Text(viewModel._quantity_unit_size ?? "N/A")
                            .foregroundColor(.secondary)
                    }
                }

                Section(header: Text("Product Details")) {
                    TextField("Quantity", text: $quantity)
                        .keyboardType(.decimalPad)

                    TextField("Price", text: $price)
                        .keyboardType(.decimalPad)

                    DatePicker("Started Using Date", selection: $startedDate, displayedComponents: .date)

                    DatePicker("Completed Date", selection: Binding(
                        get: { completedDate ?? Date() },
                        set: { completedDate = $0 }
                    ), displayedComponents: .date)
                }

                Section(header: Text("Status")) {
                    Picker("Status", selection: $selectedStatusId) {
                        ForEach(productViewModel.getStatus(), id: \.id) { status in
                            Text(status._name ?? "Unnamed").tag(status._id)
                        }
                    }
                    .pickerStyle(.menu)
                }

                Section(header: Text("Order Info")) {
                    DatePicker("Ordered Date", selection: $orderedDate, displayedComponents: .date)
                    DatePicker("Expected Delivery", selection: $expectedDeliveryDate, displayedComponents: .date)

                    DatePicker("Received Date", selection: Binding(
                        get: { receivedDate ?? Date() },
                        set: { receivedDate = $0 }
                    ), displayedComponents: .date)
                }

                Section {
                    Button("Save Product") {
                        saveProduct()
                    }
                    .disabled(!isFormValid())
                }
            }
            .navigationTitle("Add Product")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }

    private func isFormValid() -> Bool {
        guard Double(quantity) != nil,
              Double(price) != nil,
              selectedStatusId != nil else {
            return false
        }
        return true
    }

    private func saveProduct() {
        let order = Orders(
            _id: UUID(),
            _ordered_date: orderedDate,
            _expected_delivery_date: expectedDeliveryDate,
            _received_date: receivedDate,
            _product_id: nil // Will be set in backend if needed
        )

        let product = Products(
            _id: UUID(),
            _product_info_id: viewModel._id,
            _quantity: Double(quantity),
            _started_using_date: startedDate,
            _product_completed_date: completedDate,
            _price: Double(price),
            _status_id: selectedStatusId,
            order: order
        )

        productViewModel.saveProduct(product)
        dismiss()
    }
}
