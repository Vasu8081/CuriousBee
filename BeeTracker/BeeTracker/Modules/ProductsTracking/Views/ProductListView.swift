import SwiftUI

struct ProductListView: View {
    @EnvironmentObject private var productViewModel: ProductViewModel
    @State private var showAddForm = false
    @State private var selectedProduct: ProductInfosViewModel?

    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 12) {
                    TextField("Search products...", text: $productViewModel.searchQuery)
                        .textFieldStyle(RoundedBorderTextFieldStyle())
                        .padding(.horizontal)
                        .padding(.top)

                    ForEach(productViewModel.filteredProducts) { product in
                        ProductInfoCardView(viewModel: product)
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
//                if let binding = binding(for: product) {
//                    NavigationView {
//                        ProductDetailView(product: binding, viewModel: viewModel)
//                    }
//                }
            }
            .sheet(isPresented: $showAddForm) {
//                ProductAddForm()
            }
        }
    }
}
