import SwiftUI

struct ProductListView: View {
    @StateObject private var viewModel = ProductViewModel()
    @State private var showAddForm = false
    @State private var selectedProduct: Product?

    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 12) {
                    // 🔍 Search
                    TextField("Search products...", text: $viewModel.searchQuery)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .padding(.horizontal)
                        .padding(.top)

                    ForEach(viewModel.filteredProducts) { product in
                        ProductCardView(product: product) {
                            selectedProduct = product
                        } onDelete: {
                            viewModel.deleteProduct(product)
                        }
                    }

                    Spacer()
                }
                .padding(.horizontal)
            }
            .navigationTitle("📦 Products")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button {
                        showAddForm = true
                    } label: {
                        Image(systemName: "plus")
                    }
                }
            }
            .sheet(item: $selectedProduct) { product in
                if let binding = binding(for: product) {
                    NavigationView {
                        ProductDetailView(product: binding, viewModel: viewModel)
                    }
                }
            }
            .sheet(isPresented: $showAddForm) {
                ProductFormView(viewModel: viewModel)
            }
        }
    }

    private func binding(for item: Product) -> Binding<Product>? {
        guard let index = viewModel.products.firstIndex(where: { $0.id == item.id }) else {
            return nil
        }
        return $viewModel.products[index]
    }
}
