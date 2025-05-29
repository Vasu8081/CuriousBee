import SwiftUI

struct AddPeriodForm: View {
    @Binding var tracker: PeriodTracker
    @Environment(\.dismiss) var dismiss

    @State private var startDate = Date()
    @State private var duration = 5
    @State private var notes = ""

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Period Details")) {
                    DatePicker("Start Date", selection: $startDate, displayedComponents: .date)
                    Stepper("Duration: \(duration) days", value: $duration, in: 1...10)
                    TextField("Notes (optional)", text: $notes)
                }
            }
            .navigationTitle("Add Period")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") { dismiss() }
                }
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let endDate = Calendar.current.date(byAdding: .day, value: duration - 1, to: startDate) ?? startDate
                        let entry = PeriodEntry(
                            startDate: startDate,
                            endDate: endDate,
                            notes: notes,
                            symptoms: []
                        )
                        tracker.history.append(entry)
                        dismiss()
                    }
                }
            }
        }
    }
}
