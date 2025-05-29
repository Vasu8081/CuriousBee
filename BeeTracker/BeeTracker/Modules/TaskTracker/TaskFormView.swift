import SwiftUI

struct TaskFormView: View {
    @Environment(\.dismiss) var dismiss
    @ObservedObject var viewModel: TaskViewModel

    let isDeadlineMode: Bool

    @State private var title: String = ""
    @State private var notes: String = ""
    @State private var deadline: Date? = nil
    @State private var interactionStyle: TaskInteractionStyle = .canDoAloneOrTogether
    @State private var primaryDoer: TaskDoer? = nil

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
                    LazyVGrid(columns: Array(repeating: .init(.flexible()), count: 2), spacing: 12) {
                        ForEach(TaskInteractionStyle.allCases, id: \.self) { style in
                            VStack(spacing: 4) {
                                Text(style.emoji)
                                Text(style.description)
                                    .font(.caption2)
                                    .multilineTextAlignment(.center)
                            }
                            .padding(8)
                            .frame(maxWidth: .infinity)
                            .background(interactionStyle == style ? Color.accentColor.opacity(0.2) : Color.gray.opacity(0.1))
                            .cornerRadius(8)
                            .onTapGesture {
                                interactionStyle = style
                            }
                        }
                    }
                    .padding(.vertical, 4)
                }

                Section(header: Text("Primary Doer (optional)")) {
                    Picker("Who's the main person doing it?", selection: Binding(
                        get: { primaryDoer ?? .me },
                        set: { primaryDoer = $0 })) {
                        Text("None").tag(TaskDoer?.none)
                        ForEach(TaskDoer.allCases, id: \.self) { doer in
                            Text(doer.rawValue.capitalized).tag(TaskDoer?.some(doer))
                        }
                    }
                }
            }
            .navigationTitle("New Task")
            .toolbar {
                ToolbarItem(placement: .confirmationAction) {
                    Button("Save") {
                        let task = Task(
                            title: title,
                            notes: notes.isEmpty ? nil : notes,
                            deadline: isDeadlineMode ? deadline : nil,
                            interactionStyle: interactionStyle,
                            presencePreference: .both,
                            assignedTo: .both,
                            primaryDoer: primaryDoer
                        )
                        viewModel.addTask(task)
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
