import SwiftUI

struct AddPeriodForm: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var periodViewModel: PeriodViewModel

    @State private var startDate = Date()
    @State private var endDate = Date()
    @State private var duration = 5
    @State private var notes = ""
    @State private var isEnded = false
    @State private var selectedSymptoms: Set<SymptomsViewModel> = []

    var isBackdated: Bool {
        Calendar.current.dateComponents([.day], from: startDate, to: Date()).day ?? 0 > 5
    }

    var symptoms: [SymptomsViewModel] {
        periodViewModel.getAllSymptoms()
    }

    var body: some View {
        NavigationView {
            Form {
                dateSection
                if isEnded {
                    symptomsSection
                }
                notesSection
            }
            .navigationTitle("Add Period")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") { dismiss() }
                }
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let finalEndDate = isBackdated ? endDate : Calendar.current.date(byAdding: .day, value: duration - 1, to: startDate) ?? startDate
                        let new_id = UUID()
                        var symptomsList: [PeriodSymptoms] = []
                        for symptom in selectedSymptoms {
                            symptomsList.append(PeriodSymptoms(_period_entry_id: new_id, _symptom_id: symptom._id))
                        }
                        let newEntry = PeriodEntries(
                            _id: new_id,
                            _period_info_id: periodViewModel.getPeriodInfoId(),
                            _start_date: startDate,
                            _end_date: finalEndDate,
                            _notes: notes.isEmpty ? nil : notes,
                            _is_ended: isEnded,
                            period_symptoms: symptomsList
                        )
                        periodViewModel.addPeriodEntry(entry: newEntry)
                        dismiss()
                    }
                }
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
