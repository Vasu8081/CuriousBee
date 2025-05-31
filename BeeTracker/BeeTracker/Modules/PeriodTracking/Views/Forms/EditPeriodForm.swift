import SwiftUI

struct EditPeriodForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject var periodViewModel: PeriodViewModel

    @ObservedObject var entryViewModel: PeriodEntriesViewModel

    @State private var startDate: Date = Date()
    @State private var endDate: Date = Date()
    @State private var duration: Int = 5
    @State private var notes: String = ""
    @State private var isEnded: Bool = false
    @State private var selectedSymptoms: Set<SymptomsViewModel> = []

    var isBackdated: Bool {
        return isEnded
    }

    var symptoms: [SymptomsViewModel] {
        periodViewModel.getAllSymptoms()
    }

    var body: some View {
        NavigationStack {
            Form {
                dateSection
                if isEnded {
                    symptomsSection
                }
                notesSection
            }
            .navigationTitle("Edit Period")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let finalEndDate = isBackdated ? endDate : Calendar.current.date(byAdding: .day, value: duration - 1, to: startDate) ?? startDate

                        entryViewModel._start_date = startDate
                        entryViewModel._end_date = finalEndDate
                        entryViewModel._is_ended = isEnded
                        entryViewModel._notes = notes.isEmpty ? nil : notes

                        entryViewModel.period_symptoms = selectedSymptoms.map {
                            PeriodSymptomsViewModel(model: PeriodSymptoms(_period_entry_id: entryViewModel._id, _symptom_id: $0._id))
                        }

                        dismiss()
                    }
                }

                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") { dismiss() }
                }
            }
            .onAppear {
                startDate = entryViewModel._start_date ?? Date()
                endDate = entryViewModel._end_date ?? startDate
                notes = entryViewModel._notes ?? ""
                isEnded = entryViewModel._is_ended ?? false
                duration = Calendar.current.dateComponents([.day], from: startDate, to: endDate).day ?? 5
                selectedSymptoms = Set(entryViewModel.period_symptoms.compactMap { symptom in
                    periodViewModel.getAllSymptoms().first(where: { $0._id == symptom._symptom_id })
                })
            }
        }
    }

    private var dateSection: some View {
        Section(header: Text("Period Details")) {
            DatePicker("Start Date", selection: $startDate, displayedComponents: .date)

            if isBackdated {
                DatePicker("End Date", selection: $endDate, in: startDate..., displayedComponents: .date)
            } else {
                Stepper("Duration: \(duration) days", value: $duration, in: 1...10)
            }

            Toggle("Mark as Ended", isOn: $isEnded)
        }
    }

    private var symptomsSection: some View {
        Section(header: Text("Symptoms")) {
            ForEach(symptoms, id: \.id) { symptom in
                Toggle(symptom._name ?? "Unnamed Symptom", isOn: Binding<Bool>(
                    get: { selectedSymptoms.contains(symptom) },
                    set: { isOn in
                        if isOn {
                            selectedSymptoms.insert(symptom)
                        } else {
                            selectedSymptoms.remove(symptom)
                        }
                    }
                ))
            }
        }
    }

    private var notesSection: some View {
        Section(header: Text("Notes")) {
            TextField("Optional notes", text: $notes)
        }
    }
}
