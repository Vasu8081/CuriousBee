import SwiftUI

struct TaskFormView: View {
    @Environment(\.dismiss) var dismiss
    @EnvironmentObject private var taskViewModel: TaskViewModel
    @EnvironmentObject private var userViewModel: UserViewModel

    let isDeadlineMode: Bool

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

                if isDeadlineMode {
                    Section(header: Text("Deadline")) {
                        DatePicker("Select Deadline", selection: Binding(
                            get: { deadline ?? Date() },
                            set: { deadline = $0 }),
                            displayedComponents: .date)
                    }
                }

                Section(header: Text("How will it be done?")) {
                    Toggle(isOn: $isShared) {
                        Text(isShared ? "Together" : "Alone")
                            .fontWeight(.medium)
                    }
                    .toggleStyle(SwitchToggleStyle(tint: .pink))
                }
                
                Section(header: Text("Interaction Style")) {
                    Toggle(isOn: $isConscience) {
                        Text(isConscience ? "Full Conscience" : "Partial Conscience")
                            .fontWeight(.medium)
                    }
                    .toggleStyle(SwitchToggleStyle(tint: .pink))
                }

                Section(header: Text("Primary Doer (optional)")) {
                    Picker("Assign To", selection: $primaryDoer) {
                        Text("Unassigned").tag(UUID?.none)
                        ForEach(userViewModel.getUserViewModels(), id: \.id) { user in
                            if let id = user._id {
                                Text(user._name ?? "Unnamed").tag(id) // Use flat UUID
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
                            let task = Tasks(_id: UUID(), _title: title, _notes: notes.isEmpty ? nil : notes, _deadline: isDeadlineMode ? deadline : nil, _interaction_style: "Full Conscience", _primary_doer_user_id: primaryDoer, _other_users_presence_necessary: isShared, _is_completed: false)
                            taskViewModel.addTask(task)
                        }
                        else{
                            let task = Tasks(_id: UUID(), _title: title, _notes: notes.isEmpty ? nil : notes, _deadline: isDeadlineMode ? deadline : nil, _interaction_style: "Partial Conscience", _primary_doer_user_id: primaryDoer, _other_users_presence_necessary: isShared, _is_completed: false)
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
