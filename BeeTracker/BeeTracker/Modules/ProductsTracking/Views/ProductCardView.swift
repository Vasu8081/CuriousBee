import SwiftUI

struct ProductCardView: View {
    let product: Product
    let onTap: () -> Void
    let onDelete: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 6) {
            header
            categoryText
            usageProgress
            deliveryInfo
            notesText
            footer
        }
        .padding()
        .background(
            RoundedRectangle(cornerRadius: 12)
                .fill(Color.white)
                .shadow(color: .black.opacity(0.05), radius: 5, x: 0, y: 2)
                .overlay(
                    RoundedRectangle(cornerRadius: 12)
                        .stroke(borderColor, lineWidth: 1)
                )
        )
        .onTapGesture { onTap() }
    }

    private var header: some View {
        HStack {
            Text(product.name)
                .font(.headline)
            Spacer()
            if product.status == .restocked {
                Image(systemName: "checkmark.circle.fill")
                    .foregroundColor(.green)
            }
        }
    }

    private var categoryText: some View {
        Text("Category: \(product.category.rawValue)")
            .font(.caption)
            .foregroundColor(.secondary)
    }

    private var usageProgress: some View {
        Group {
            if let daysLeft = product.daysLeft {
                ProgressView(value: daysLeftPercent) {
                    Text("Days left: \(daysLeft)")
                        .font(.caption)
                }
            } else {
                Text("Not started yet")
                    .font(.caption)
                    .foregroundColor(.gray)
            }
        }
    }

    private var deliveryInfo: some View {
        Group {
            if product.isAwaitingDelivery, let eta = product.expectedDeliveryDate {
                Text("📦 Expected in \(daysLeft(from: eta)) days")
                    .font(.caption2)
                    .foregroundColor(.blue)
            }
        }
    }

    private var notesText: some View {
        Group {
            if let notes = product.notes, !notes.isEmpty {
                Text("Note: \(notes)")
                    .font(.footnote)
                    .italic()
                    .foregroundColor(.gray)
            }
        }
    }

    private var footer: some View {
        HStack {
            Button(role: .destructive) {
                onDelete()
            } label: {
                Label("Delete", systemImage: "trash")
                    .font(.caption)
            }

            Spacer()

            Text("Updated: \(formatted(product.lastUpdated))")
                .font(.caption2)
                .foregroundColor(.secondary)
        }
    }

    private var daysLeftPercent: Double {
        guard let daysLeft = product.daysLeft else { return 0 }
        let total = product.daysPerQuantity * product.quantityOrdered
        return max(min(Double(daysLeft) / total, 1), 0)
    }

    private var borderColor: Color {
        switch daysLeftPercent {
        case ...0.05: return .red.opacity(0.5)
        case ...0.15: return .orange.opacity(0.5)
        default: return .green.opacity(0.3)
        }
    }

    private func daysLeft(from date: Date) -> Int {
        let diff = Calendar.current.dateComponents([.day], from: Date(), to: date).day ?? 0
        return max(diff, 0)
    }

    private func formatted(_ date: Date) -> String {
        let formatter = DateFormatter()
        formatter.dateStyle = .short
        return formatter.string(from: date)
    }
}
