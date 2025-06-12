import SwiftUI

struct CategoryTabView: View {
    @ObservedObject var userViewModel: UserViewModel
    @State private var searchText = ""
    @State private var showingAddCategoryForm = false
    @State private var selectedRange: CategoryFilterRange = .thisMonth

    var filteredCategories: [Category] {
        if searchText.isEmpty {
            return userViewModel.userData.categories
        } else {
            return userViewModel.userData.categories.filter {
                $0.name.lowercased().contains(searchText.lowercased())
            }
        }
    }

    var body: some View {
        NavigationView {
            VStack(spacing: 8) {
                Picker("Range", selection: $selectedRange) {
                    ForEach(CategoryFilterRange.allCases) { range in
                        Text(range.rawValue).tag(range)
                    }
                }
                .pickerStyle(SegmentedPickerStyle())
                .padding(.horizontal)

                TextField("Search categories", text: $searchText)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                    .padding(.horizontal)

                List(filteredCategories, id: \.id) { category in
                    NavigationLink(destination: CategoryDetailView(
                        category: category,
                        userViewModel: userViewModel,
                        selectedRange: selectedRange
                    )) {
                        HStack {
                            Text(category.emoji ?? "📁")
                            Text(category.name)
                                .font(.headline)
                            Spacer()
                            Text("₹\(totalFor(category: category), specifier: "%.2f")")
                                .foregroundColor(category.type == .income ? .green : .red)
                        }
                    }
                }
            }
            .navigationTitle("Categories")
            .sheet(isPresented: $showingAddCategoryForm) {
                AddCategoryFormView { newCategory in
                    userViewModel.userData.categories.append(newCategory)
                    userViewModel.needSync = true
                    showingAddCategoryForm = false
                }
            }
            .overlay(
                VStack {
                    Spacer()
                    HStack {
                        Spacer()
                        Button(action: { showingAddCategoryForm = true }) {
                            Image(systemName: "plus.circle.fill")
                                .resizable()
                                .frame(width: 56, height: 56)
                                .foregroundColor(.blue)
                                .padding()
                        }
                    }
                }
            )
        }
    }

    private func totalFor(category: Category) -> Double {
        userViewModel.userData.allTransactions
            .filter { txn in
                txn.category.id == category.id &&
                selectedRange.matches(txn.date)
            }
            .map { txn in
                switch category.type {
                case .income, .investment, .transfer:
                    return txn.amount
                case .expense:
                    return -txn.amount
                }
            }
            .reduce(0, +)
    }
}
