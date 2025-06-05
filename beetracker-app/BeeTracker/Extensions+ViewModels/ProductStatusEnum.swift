import Foundation

class ProductStatusEnum {
    static let shared = ProductStatusEnum()

    @Published var productStatusViewModel: [ ProductStatusViewModel] = []
    var productStatussesMap: [String: ProductStatusViewModel] = [:]

    private init() {}
    
    func reload() {
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
    
    func getStatus(for id: String?) -> ProductStatusViewModel {
        guard let idStr = id else {
            return ProductStatusViewModel(model: ProductStatus())
        }
        return self.productStatussesMap[idStr] ?? ProductStatusViewModel(model: ProductStatus())
    }
    
    func getStatusName(for id: String?) -> String {
        return self.getStatus(for: id)._name ?? ""
    }
}
