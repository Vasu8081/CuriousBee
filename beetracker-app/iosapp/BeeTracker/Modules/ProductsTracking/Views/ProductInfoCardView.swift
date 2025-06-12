import SwiftUI

struct ProductInfoCardView: View {
    let viewModel: ProductInfosViewModel
    @EnvironmentObject var productViewModel: ProductViewModel
    @State private var showAddForm = false

    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack {
                Text(viewModel.name ?? "Unnamed Product")
                    .font(.headline)

                Spacer()

                Text(viewModel.productStatus())
                    .font(.caption)
                    .padding(6)
                    .background(statusColor)
                    .cornerRadius(6)
                    .foregroundColor(.white)

                Button(action: {
                    showAddForm = true
                }) {
                    Image(systemName: "cart.badge.plus")
                        .padding(6)
                        .background(Color.blue.opacity(0.2))
                        .cornerRadius(8)
                }

                Button(role: .destructive, action: {
                    productViewModel.delete(productInfo: viewModel)
                }) {
                    Image(systemName: "trash")
                        .padding(6)
                        .background(Color.red.opacity(0.2))
                        .cornerRadius(8)
                }
            }

            HStack {
                Image(systemName: "calendar")
                Text("Days left: \(viewModel.daysLeft())")
                    .font(.subheadline)
            }
            .foregroundColor(.secondary)
        }
        .padding()
        .background(
            RoundedRectangle(cornerRadius: 12)
                .fill(Color.white)
                .shadow(color: .black.opacity(0.05), radius: 4, x: 0, y: 2)
        )
        .sheet(isPresented: $showAddForm) {
            ProductAddForm(viewModel: viewModel)
        }
    }

    private var statusColor: Color {
        switch viewModel.productStatus().lowercased() {
        case "ordered": return .green
        case "empty": return .blue
        case "in use": return .orange
        default: return .gray
        }
    }
}
