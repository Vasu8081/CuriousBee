import Foundation

class PeriodViewModel: ObservableObject {
    @Published var periodInfosViewModel: PeriodInfosViewModel = PeriodInfosViewModel(model: PeriodInfos())
    @Published var symptomsViewModel: [SymptomsViewModel] = []
    
    private var symptomIdToNameMap: [String: String] = [:]
    init(){
        
    }
    
    func reload(){
        symptomsViewModel.removeAll()
        symptomIdToNameMap.removeAll()
        ServerEndPoints.shared.getAllPeriodInfos { result in
            switch result {
            case .success(let periodInfo):
                DispatchQueue.main.async {
                    self.periodInfosViewModel = PeriodInfosViewModel(model: periodInfo[0])
                }
            case .failure(let error):
                print("Failed to fetch period infos: \(error)")
            }
        }
        
        ServerEndPoints.shared.getAllSymptoms { result in
            switch result {
            case .success(let symptoms):
                DispatchQueue.main.async {
                    for symptom in symptoms {
                        self.symptomsViewModel.append(SymptomsViewModel(model: symptom))
                        self.symptomIdToNameMap[symptom._id?.uuidString ?? ""] = symptom._name
                    }
                }
            case .failure(let error):
                print("Failed to fetch symptoms: \(error)")
            }
        }
    }
    
    func getAverageCycleLength() -> Int {
        return periodInfosViewModel._average_cycle_length ?? 25
    }
    
    func getAverageDurationOfPeriod() -> Int {
        return periodInfosViewModel._average_period_duration ?? 5
    }
    
    func getPeriodInfoId() -> UUID {
        return periodInfosViewModel._id ?? UUID()
    }
    
    func addPeriodEntry(entry: PeriodEntries){
        self.periodInfosViewModel.period_entries.append(PeriodEntriesViewModel(model: entry))
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func getSymptomName(for symptomId: UUID?) -> String {
        guard let symptomId = symptomId else { return "NA" }
        return symptomIdToNameMap[symptomId.uuidString] ?? "NA"
    }
    
    func deletePeriodEntry(entry: PeriodEntriesViewModel) {
        self.periodInfosViewModel.period_entries.removeAll { $0._id == entry._id }
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func getAllSymptoms() -> [SymptomsViewModel] {
        return self.symptomsViewModel
    }
    
    func getOngoingPeriodViewModel() -> PeriodEntriesViewModel? {
        return self.periodInfosViewModel.period_entries.first { entry in
            guard let isEnded = entry._is_ended else { return false }
            return !isEnded
        }
    }
    
    func startNextExpectedPeriod(_ startDate: Date) {
        let new_id = UUID()
        
        let finalEndDate = Calendar.current.date(
            byAdding: .day,
            value: self.getAverageDurationOfPeriod() - 1,
            to: startDate
        ) ?? startDate

        let newEntry = PeriodEntries(
            _id: new_id,
            _period_info_id: self.getPeriodInfoId(),
            _start_date: startDate,
            _end_date: finalEndDate,
            _notes: nil,
            _is_ended: false,
            period_symptoms: []
        )

        self.addPeriodEntry(entry: newEntry)
    }
    
    func getNextExpectedPeriodDate() -> Date? {
        return self.periodInfosViewModel._predicted_next_period_date
    }
    
    func endOngoingPeriod() {
        guard let ongoingPeriodViewModel = self.getOngoingPeriodViewModel() else { return }
        ongoingPeriodViewModel._is_ended = true
        ongoingPeriodViewModel._end_date = Date()
        if let index = self.periodInfosViewModel.period_entries.firstIndex(where: { $0._id == ongoingPeriodViewModel._id }) {
            self.periodInfosViewModel.period_entries[index]._is_ended = true
            self.periodInfosViewModel.period_entries[index]._end_date = ongoingPeriodViewModel._end_date
        }
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func deleteOngoingPeriod() {
        guard let ongoingPeriodViewModel = self.getOngoingPeriodViewModel() else { return }
        self.periodInfosViewModel.period_entries.removeAll { $0._id == ongoingPeriodViewModel._id }
        self.periodInfosViewModel = self.periodInfosViewModel
    }
    
    func historyEntriesSorted() -> [PeriodEntriesViewModel] {
        self.periodInfosViewModel.period_entries
            .filter { $0._is_ended == true || Calendar.current.startOfDay(for: $0._end_date ?? .distantPast) < Calendar.current.startOfDay(for: Date()) }
            .sorted { ($0._start_date ?? .distantPast) > ($1._start_date ?? .distantPast) }
    }
}
