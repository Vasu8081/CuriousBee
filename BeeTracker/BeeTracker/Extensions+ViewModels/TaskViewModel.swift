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
        self.taskViewModels.append(TasksViewModel(model: task))
    }
    
    func deleteTask(_ task: TasksViewModel) {
        self.taskViewModels.removeAll { $0._id == task._id }
    }
    
    func toggleComplete(_ task: TasksViewModel) {
        task._is_completed?.toggle()
        self.taskViewModels = self.taskViewModels
    }
    
    func getCompletedTasks() -> [TasksViewModel] {
        return self.taskViewModels.filter { $0._is_completed ?? false }
    }
    
    func getUncompletedTasks() -> [TasksViewModel] {
        return self.taskViewModels.filter { !($0._is_completed ?? true) }
    }
    
    func triggerRefresh() {
        self.taskViewModels = self.taskViewModels
    }
    
}

