import SwiftUI

extension ProductInfosViewModel {
    func productStatus() -> String {
        var status: String = "Not Started"
        for prod in self.products {
            print(status)
            status = ProductStatusEnum.shared.getStatusName(for: prod.status_id?.uuidString)
        }
        
        return status
    }
    
    func daysLeft() -> Int {
        let today = Calendar.current.startOfDay(for: Date())
        var totalDaysLeft = 0

        for prod in self.products {
            guard prod.product_completed_date == nil else { continue }
            let quantity = prod.quantity ?? 0
            let daysPerQty = self.days_per_quantity ?? 0
            let expectedDays = daysPerQty * quantity
            if let startDate = prod.started_using_date {
                let daysUsed = Calendar.current.dateComponents([.day], from: startDate, to: today).day ?? 0
                let remaining = max(Int(expectedDays) - daysUsed, 0)
                totalDaysLeft += remaining
            } else {
                totalDaysLeft += Int(expectedDays)
            }
        }
        return totalDaysLeft
    }
    
}
