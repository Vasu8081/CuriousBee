
import Foundation

extension GroupsViewModel {
    func reload(id: String){
        print("VK reloading")
        ServerEndPoints.shared.getGroups(id: id) { result in
            switch(result){
            case .success(let groups):
                self.fetch(from: groups)
            case .failure(let error):
                print(error)
            }
        }
    }
}
