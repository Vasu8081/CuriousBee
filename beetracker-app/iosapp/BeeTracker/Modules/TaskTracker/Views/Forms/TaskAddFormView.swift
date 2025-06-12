import SwiftUI

struct TaskFormView: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var taskViewModel: TaskViewModel
    @EnvironmentObject private var userViewModel: UserViewModel

    @State private var isDeadlineMode = false
    @State private var title: String = ""
    @State private var notes: String = ""
    @State private var deadline: Date? = nil
    @State private var isShared: Bool = false
    @State private var isConscience: Bool = false
    @State private var primaryDoer: UUID? = nil

    var body: some View {
        NavigationStack {
            Form {
                Section(header: Text("Task Details")) {
                    TextField("What needs to be done?", text: $title)
                    TextField("Notes (optional)", text: $notes)
                }

                Section {
                    Toggle("Add Deadline", isOn: $isDeadlineMode)
                }

                if isDeadlineMode {
                    Section(header: Text("Deadline")) {
                        DatePicker("Select Deadline", selection: Binding(
                            get: { deadline ?? Date() },
                            set: { deadline = $0 }),
                            displayedComponents: .date)
                    }
                }

                Section(header: Text("How will it be done?")) {
                    Picker("How will it be done?", selection: $isShared) {
                        Text("Alone").tag(false)
                        Text("Together").tag(true)
                    }
                    .pickerStyle(.segmented)
                }

                Section(header: Text("Interaction Style")) {
                    Picker("Interaction Style", selection: $isConscience) {
                        Text("Partial").tag(false)
                        Text("Full").tag(true)
                    }
                    .pickerStyle(.segmented)
                }

                Section(header: Text("Primary Doer (optional)")) {
                    Picker("Assign To", selection: $primaryDoer) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            if let id = user.id {
                                Text(user.name ?? "Unnamed").tag(id) // Use flat UUID
                            }
                        }
                    }
                    .pickerStyle(MenuPickerStyle())
                }
            }
            .navigationTitle("New Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        if isConscience {
                            let task = Tasks(id: UUID(), title: title, notes: notes.isEmpty ? nil : notes, deadline: isDeadlineMode ? deadline : nil, interaction_style: "Full Conscience", primary_doer_user_id: primaryDoer, other_users_presence_necessary: isShared, is_completed: false)
                            taskViewModel.addTask(task)
                        }
                        else{
                            let task = Tasks(id: UUID(), title: title, notes: notes.isEmpty ? nil : notes, deadline: isDeadlineMode ? deadline : nil, interaction_style: "Partial Conscience", primary_doer_user_id: primaryDoer, other_users_presence_necessary: isShared, is_completed: false)
                            taskViewModel.addTask(task)
                        }

        
                        dismiss()
                    }
                    .disabled(title.trimmingCharacters(in: .whitespaces).isEmpty)
                }

                ToolbarItem(placement: .cancellationAction) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }
}
