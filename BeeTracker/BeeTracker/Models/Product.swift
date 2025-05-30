import Foundation

enum ProductStat: String, Codable, CaseIterable {
    case inUse = "In Use"
    case runningLow = "Running Low"
    case completed = "Completed"
    case restocked = "Restocked"
}

enum Category: String, Codable, CaseIterable {
    case FaceCare = "Face Care"
    case HairCare = "Hair Care"
    case BodyCare = "Body Care"
    case Skincare = "Skincare"
    case Makeup = "Makeup"
    case Fragrance = "Fragrance"
    case Utility = "Utility"
    case PersonalCare = "Personal Care"
    case HomeCare = "Home Care"
    case Nocat = "No Category"
}

struct Product: Identifiable, Codable, Equatable {
    let id: UUID
    var name: String
    var category: Category
    var status: ProductStat
    var reminderEnabled: Bool
    var notes: String?
    var daysPerQuantity: Double
    var quantityOrdered: Double
    var lastUpdated: Date
    var orderedDate: Date?
    var expectedDeliveryDays: Int?
    var receivedDate: Date?
    var startedUsingDate: Date?

    static func create(
        name: String,
        category: Category,
        notes: String? = nil,
        daysPerQuantity: Double,
        quantityOrdered: Double = 0.0,
        startedUsingDate: Date? = nil
    ) -> Product {
        return Product(
            id: UUID(),
            name: name,
            category: category,
            status: .inUse,
            reminderEnabled: true,
            notes: notes,
            daysPerQuantity: daysPerQuantity,
            quantityOrdered: quantityOrdered,
            lastUpdated: Date(),
            orderedDate: nil,
            expectedDeliveryDays: nil,
            receivedDate: nil,
            startedUsingDate: startedUsingDate
        )
    }

    mutating func applyOrder(orderedDate: Date, expectedDeliveryDays: Int, quantityOrdered: Double) {
        self.orderedDate = orderedDate
        self.expectedDeliveryDays = expectedDeliveryDays
        self.quantityOrdered = quantityOrdered
        self.lastUpdated = Date()
    }

    mutating func markReceived(receivedDate: Date) {
        self.receivedDate = receivedDate
        self.status = .restocked
        self.lastUpdated = Date()
    }

    mutating func markStartedUsing(date: Date = Date()) {
        self.startedUsingDate = date
        self.status = .inUse
        self.lastUpdated = Date()
    }

    var daysLeft: Int? {
        guard let start = startedUsingDate else { return nil }
        let totalDays = daysPerQuantity * quantityOrdered
        let daysElapsed = Date().timeIntervalSince(start) / (60 * 60 * 24)
        let remaining = totalDays - daysElapsed
        return max(Int(remaining.rounded()), 0)
    }

    var expectedDeliveryDate: Date? {
        guard let ordered = orderedDate, let days = expectedDeliveryDays else { return nil }
        return Calendar.current.date(byAdding: .day, value: days, to: ordered)
    }

    var isAwaitingDelivery: Bool {
        orderedDate != nil && receivedDate == nil
    }

    var hasDeliveryPassed: Bool {
        guard let expected = expectedDeliveryDate else { return false }
        return Date() > expected
    }

    var isRunningLow: Bool {
        if let remaining = daysLeft {
            return remaining <= 5 && status == .inUse
        }
        return false
    }

    var isCompleted: Bool {
        return daysLeft == 0 && status == .inUse
    }
}
