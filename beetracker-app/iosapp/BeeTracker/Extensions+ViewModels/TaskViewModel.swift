import Foundation

class TaskViewModel : ObservableObject {
    @Published var taskViewModels: [TasksViewModel] = []
    
    init() {
        
    }
    
    func reload() {
        ServerEndPoints.shared.getAllTasks { result in
            switch result {
            case .success(let tasks):
                DispatchQueue.main.async {
                    for task in tasks {
                        self.taskViewModels.append(TasksViewModel(model: task))
                    }
                }
            case .failure(let error):
                print("Failed to fetch tasks: \(error)")
            }
        }
    }
    
    func addTask(_ task: Tasks ) {
        let model = TasksViewModel(model: task)
        model.save()
        self.taskViewModels.append(model)
    }
    
    func deleteTask(_ task: TasksViewModel) {
        guard let id = task.id else {
            print("Cannot delete entry: id is nil")
            return
        }
        ServerEndPoints.shared.deleteTasks(id: id) { result in
            switch result {
            case .success:
                self.taskViewModels.removeAll { $0.id == task.id }
            case .failure(let error):
                print("Failed to delete task entry: \(error)")
            }
        }
    }
    
    func toggleComplete(_ task: TasksViewModel) {
        task.is_completed?.toggle()
        task.save()
        self.taskViewModels = self.taskViewModels
    }
    
    func getCompletedTasks() -> [TasksViewModel] {
        return self.taskViewModels.filter { $0.is_completed ?? false }
    }
    
    func getUncompletedTasks() -> [TasksViewModel] {
        return self.taskViewModels.filter { !($0.is_completed ?? true) }
    }
    
    func triggerRefresh() {
        self.taskViewModels = self.taskViewModels
    }
    
}

