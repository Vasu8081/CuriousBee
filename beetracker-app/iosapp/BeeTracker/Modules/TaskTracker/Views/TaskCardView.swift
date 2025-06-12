import SwiftUI

struct TaskCardView: View {
    @ObservedObject var task: TasksViewModel
    let onEdit: () -> Void
    let onSchedule: () -> Void
    let onDelete: () -> Void
    let onToggleComplete: () -> Void

    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack(alignment: .top) {
                Button(action: onToggleComplete) {
                    Image(systemName: task.is_completed ?? false ? "checkmark.circle.fill" : "circle")
                        .foregroundColor(task.is_completed ?? false ? .green : .gray)
                }
                .buttonStyle(.plain)

                VStack(alignment: .leading, spacing: 4) {
                    HStack {
                        Text(task.title ?? "")
                            .fontWeight(.medium)
                            .strikethrough(task.is_completed ?? false, color: .gray)
                        if let deadline = task.deadline {
                            Text("Due: \(deadline.formatted(date: .abbreviated, time: .omitted))")
                                .font(.caption2)
                                .foregroundColor(.secondary)
                        }
                    }
                    if let notes = task.notes {
                        Text(notes)
                            .font(.caption2)
                            .foregroundColor(.gray)
                    }

                    if let style = task.interaction_style {
                        Text(style)
                            .font(.caption2)
                            .foregroundColor(.accentColor)
                    }
                }

                Spacer()

                if !(task.is_completed ?? false) {
                    if task.calendarEntry == nil {
                        Button(action: onSchedule) {
                            Image(systemName: "calendar.badge.plus")
                                .foregroundColor(.blue)
                        }
                        .buttonStyle(.plain)
                    }

                    Button(role: .destructive, action: onDelete) {
                        Image(systemName: "trash")
                    }
                    .buttonStyle(.plain)
                }
            }
            .contentShape(Rectangle())
            .onTapGesture {
                if !(task.is_completed ?? false) {
                    onEdit()
                }
            }
        }
        .padding(.vertical, 4)
    }
}
