import SwiftUI

struct ProductDetailView: View {
    @Binding var product: Product
    @ObservedObject var viewModel: ProductViewModel
    @Environment(\.dismiss) var dismiss

    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {

                // 🛒 Order Product Button
                if !product.isAwaitingDelivery {
                    Button {
                        product.orderedDate = Date()
                        product.expectedDeliveryDays = 3
                        product.quantityOrdered = 1
                        product.lastUpdated = Date()
                        product.reminderEnabled = true
                    } label: {
                        Label("Order Product", systemImage: "cart.badge.plus")
                            .font(.body)
                            .padding()
                            .frame(maxWidth: .infinity)
                            .background(Color.blue.opacity(0.1))
                            .cornerRadius(10)
                    }
                }

                // 📦 Product Name
                TextField("Product Name", text: $product.name)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .submitLabel(.done)

                // 🏷 Category Picker
                let allCategories: [Category?] = [nil] + Category.allCases
                Picker("Category", selection: Binding<Category?>(
                    get: { product.category },
                    set: { product.category = $0! }
                )) {
                    ForEach(allCategories, id: \.self) { category in
                        Text(category?.rawValue ?? "None").tag(category)
                    }
                }
                .pickerStyle(MenuPickerStyle())

                // 🔢 Days Per Quantity
                Stepper("Days per Quantity: \(Int(product.daysPerQuantity))", value: $product.daysPerQuantity, in: 1...90)
                    .onChange(of: product.daysPerQuantity) { _, _ in product.lastUpdated = Date() }

                // 🔢 Quantity Ordered
                Stepper("Quantity Ordered: \(Int(product.quantityOrdered))", value: $product.quantityOrdered, in: 1...30)
                    .onChange(of: product.quantityOrdered) { _, _ in product.lastUpdated = Date() }

                // 📅 Started Using Date
                DatePicker("Started Using Date", selection: Binding<Date>(
                    get: { product.startedUsingDate ?? Date() },
                    set: {
                        product.startedUsingDate = $0
                        product.lastUpdated = Date()
                    }
                ), displayedComponents: .date)

                // 🔔 Reminder Toggle
                Toggle("Reminder Enabled", isOn: $product.reminderEnabled)

                // 🚚 Order Details Section
                if product.orderedDate != nil {
                    Divider()
                    Text("🛍 Order Details")
                        .font(.headline)

                    DatePicker("Ordered Date", selection: Binding<Date>(
                        get: { product.orderedDate ?? Date() },
                        set: {
                            product.orderedDate = $0
                            product.lastUpdated = Date()
                        }
                    ), displayedComponents: .date)

                    Stepper("Expected Delivery in \(product.expectedDeliveryDays ?? 3) days", value: Binding(
                        get: { product.expectedDeliveryDays ?? 3 },
                        set: {
                            product.expectedDeliveryDays = $0
                            product.lastUpdated = Date()
                        }
                    ), in: 1...30)

                    HStack {
                        Text("ETA:")
                        Spacer()
                        if let eta = product.expectedDeliveryDate {
                            Text(formatted(eta))
                        } else {
                            Text("-")
                        }
                    }

                    if product.hasDeliveryPassed {
                        Text("⚠️ Delivery is overdue")
                            .font(.caption)
                            .foregroundColor(.red)
                    }

                    if product.receivedDate == nil {
                        Button("Mark as Received") {
                            product.receivedDate = Date()
                            product.status = .restocked
                            product.lastUpdated = Date()
                            viewModel.save()
                        }
                        .buttonStyle(.bordered)
                    } else {
                        HStack {
                            Text("Received:")
                            Spacer()
                            Text(formatted(product.receivedDate))
                        }
                    }
                }

                // 📝 Notes
                TextField("Notes (optional)", text: Binding(
                    get: { product.notes ?? "" },
                    set: {
                        product.notes = $0.isEmpty ? nil : $0
                        product.lastUpdated = Date()
                    }
                ))
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .submitLabel(.done)

                // ✅ Save Button
                Button("Done") {
                    product.lastUpdated = Date()
                    viewModel.save()
                    hideKeyboard()
                    dismiss()
                }
                .buttonStyle(.borderedProminent)
                .frame(maxWidth: .infinity)
                .padding(.top, 16)
            }
            .padding()
        }
        .navigationTitle("Edit Product")
        .navigationBarTitleDisplayMode(.inline)
        .gesture(
            TapGesture().onEnded { hideKeyboard() }
        )
    }

    private func formatted(_ date: Date?) -> String {
        guard let date else { return "-" }
        let formatter = DateFormatter()
        formatter.dateStyle = .medium
        return formatter.string(from: date)
    }
}

#if canImport(UIKit)
extension View {
    func hideKeyboard() {
        UIApplication.shared.sendAction(
            #selector(UIResponder.resignFirstResponder),
            to: nil, from: nil, for: nil
        )
    }
}
#endif
