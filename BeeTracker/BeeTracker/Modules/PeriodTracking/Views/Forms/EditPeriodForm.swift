import SwiftUI

struct EditPeriodForm: View {
    @Binding var entry: PeriodEntry
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Edit Dates")) {
                    DatePicker("Start", selection: $entry.startDate, displayedComponents: .date)
                    DatePicker("End", selection: $entry.endDate, displayedComponents: .date)
                }

                Section(header: Text("Notes")) {
                    TextField("Notes", text: $entry.notes)
                }

                Section(header: Text("Symptoms")) {
                    ForEach(SymptomTag.allCases, id: \.self) { tag in
                        Toggle(tag.displayName, isOn: Binding(
                            get: { entry.symptoms.contains(tag) },
                            set: { isOn in
                                if isOn {
                                    entry.symptoms.append(tag)
                                } else {
                                    entry.symptoms.removeAll { $0 == tag }
                                }
                            }
                        ))
                    }
                }
            }
            .navigationTitle("Edit Period")
            .toolbar {
                ToolbarItem(placement: .cancellationAction) {
                    Button("Done") { dismiss() }
                }
            }
        }
    }
}
