import Foundation

class ProductViewModel: ObservableObject {
    @Published var productInfosViewModel: [ProductInfosViewModel] = []
    @Published var productStatusViewModel: [ ProductStatusViewModel] = []
    @Published var searchQuery: String = ""
    
    var productStatussesMap: [String: ProductStatusViewModel] = [:]
    init(){
        
    }
    
    func reload() {
        ServerEndPoints.shared.getAllProductInfos { result in
            switch result {
            case .success(let products):
                DispatchQueue.main.async {
                    for product in products {
                        self.productInfosViewModel.append(ProductInfosViewModel(model: product))
                    }
                }
            case .failure(let error):
                print("Failed to fetch tasks: \(error)")
            }
        }
        
        ServerEndPoints.shared.getAllProductStatus { result in
            switch result {
            case .success(let statuses):
                DispatchQueue.main.async {
                    for status in statuses {
                        let model = ProductStatusViewModel(model: status)
                        self.productStatusViewModel.append(model)
                        self.productStatussesMap[status._id?.uuidString ?? ""] = model
                    }
                }
            case .failure(let error):
                print("Failed to fetch tasks: \(error)")
            }
        }
    }
    
    func saveProduct(_ product: Products) {
        
    }
    
    func delete(productInfo: ProductInfosViewModel) {
        
    }
    
    var filteredProducts: [ProductInfosViewModel] {
        let trimmed = searchQuery.trimmingCharacters(in: .whitespacesAndNewlines).lowercased()

        let matched = productInfosViewModel.filter { info in
            trimmed.isEmpty || (info._name?.lowercased().contains(trimmed) ?? false)
        }

        return matched.sorted(by: urgencySort)
    }
    
    private func urgencySort(_ lhs: ProductInfosViewModel, _ rhs: ProductInfosViewModel) -> Bool {
        let lhsScore = lhs.daysLeft()
        let rhsScore = rhs.daysLeft()

        if lhsScore != rhsScore {
            return lhsScore < rhsScore
        }
        return true
    }
    
    func getStatus() -> [ProductStatusViewModel] {
        return self.productStatusViewModel
    }
}
