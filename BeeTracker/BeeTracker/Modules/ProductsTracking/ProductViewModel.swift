import Foundation
import SwiftUI
import UserNotifications

class ProductViewModel: ObservableObject {
    @Published var searchQuery: String = ""
    static let shared = ProductViewModel()
    
    @Published var products: [Product] {
        didSet {
            userViewModel.userData.products = products
            userViewModel.save()
        }
    }

    private let userViewModel: UserViewModel

    init(userViewModel: UserViewModel = .shared) {
        self.userViewModel = userViewModel
        self.products = userViewModel.userData.products
    }

    var filteredProducts: [Product] {
        let trimmed = searchQuery.trimmingCharacters(in: .whitespacesAndNewlines).lowercased()
        let matched = products.filter {
            trimmed.isEmpty || $0.name.localizedCaseInsensitiveContains(trimmed)
        }
        return matched.sorted(by: urgencySort)
    }

    private func urgencySort(_ lhs: Product, _ rhs: Product) -> Bool {
        func urgencyScore(for item: Product) -> Int {
            return item.daysLeft ?? 0
        }

        let lhsScore = urgencyScore(for: lhs)
        let rhsScore = urgencyScore(for: rhs)

        if lhsScore != rhsScore {
            return lhsScore < rhsScore
        } else {
            return lhs.lastUpdated < rhs.lastUpdated
        }
    }

    func addProduct(_ item: Product) {
        products.append(item)
    }

    func updateProduct(_ item: Product) {
        if let index = products.firstIndex(where: { $0.id == item.id }) {
            products[index] = item
        }
    }

    func deleteProduct(_ item: Product) {
        products.removeAll { $0.id == item.id }
    }

    func deleteProduct(atOffsets offsets: IndexSet) {
        products.remove(atOffsets: offsets)
    }

    func markAsOrdered(_ item: Product, expectedInDays: Int) {
        guard let index = products.firstIndex(where: { $0.id == item.id }) else { return }
        products[index].orderedDate = Date()
        products[index].expectedDeliveryDays = expectedInDays
        products[index].receivedDate = nil
        products[index].status = .restocked
        products[index].lastUpdated = Date()
    }

    func markAsReceived(_ item: Product) {
        guard let index = products.firstIndex(where: { $0.id == item.id }) else { return }

        let current = products[index]
        let now = Date()

        if let oldStart = current.startedUsingDate,
           let oldDaysLeft = current.daysLeft {
            let newStart = Calendar.current.date(byAdding: .day, value: -oldDaysLeft, to: now)
            products[index].startedUsingDate = newStart
        } else {
            products[index].startedUsingDate = now
        }

        products[index].receivedDate = now
        products[index].orderedDate = nil
        products[index].expectedDeliveryDays = nil
        products[index].status = .inUse
        products[index].lastUpdated = now
    }
    
    func save() {
        userViewModel.save()
    }

    func scheduleLowStockNotifications() {
        for product in products {
            guard product.reminderEnabled,
                  let daysLeft = product.daysLeft,
                  daysLeft < 5 else { continue }

            let content = UNMutableNotificationContent()
            content.title = "Low Stock Alert"
            content.body = "🛒 \(product.name) is running low. Only \(daysLeft) day(s) left."
            content.sound = .default

            let request = UNNotificationRequest(
                identifier: product.id.uuidString,
                content: content,
                trigger: nil  // Delivers immediately
            )

            UNUserNotificationCenter.current().add(request)
        }
    }

}
