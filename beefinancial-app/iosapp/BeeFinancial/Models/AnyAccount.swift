import Foundation

enum AnyAccount: Identifiable, Codable, Hashable, Equatable {
    case bank(BankAccount)
    case loan(LoanAccount)
    case creditCard(CreditCardAccount)
    case borrow(BorrowAccount)
    case give(GiveAccount)
    case fd(FDAccount)
    case rd(RDAccount)
    case chit(ChitAccount)
    case property(PropertyAccount)
    case stock(StockAccount)

    var id: UUID {
        switch self {
        case .bank(let acc): return acc.id
        case .loan(let acc): return acc.id
        case .creditCard(let acc): return acc.id
        case .borrow(let acc): return acc.id
        case .give(let acc): return acc.id
        case .fd(let acc): return acc.id
        case .rd(let acc): return acc.id
        case .chit(let acc): return acc.id
        case .property(let acc): return acc.id
        case .stock(let acc): return acc.id
        }
    }

    var name: String {
        switch self {
        case .bank(let acc): return acc.name
        case .loan(let acc): return acc.name
        case .creditCard(let acc): return acc.name
        case .borrow(let acc): return acc.name
        case .give(let acc): return acc.name
        case .fd(let acc): return acc.name
        case .rd(let acc): return acc.name
        case .chit(let acc): return acc.name
        case .property(let acc): return acc.name
        case .stock(let acc): return acc.name
        }
    }

    var balance: Double {
        switch self {
        case .bank(let acc): return acc.balance
        case .loan(let acc): return acc.balance
        case .creditCard(let acc): return acc.balance
        case .borrow(let acc): return acc.balance
        case .give(let acc): return acc.balance
        case .fd(let acc): return acc.balance
        case .rd(let acc): return acc.balance
        case .chit(let acc): return acc.balance
        case .property(let acc): return acc.balance
        case .stock(let acc): return acc.balance
        }
    }

    var remarks: String? {
        switch self {
        case .bank(let acc): return acc.remarks
        case .loan(let acc): return acc.remarks
        case .creditCard(let acc): return acc.remarks
        case .borrow(let acc): return acc.remarks
        case .give(let acc): return acc.remarks
        case .fd(let acc): return acc.remarks
        case .rd(let acc): return acc.remarks
        case .chit(let acc): return acc.remarks
        case .property(let acc): return acc.remarks
        case .stock(let acc): return acc.remarks
        }
    }

    var transactions: [Transaction] {
        get {
            switch self {
            case .bank(let acc): return acc.transactions
            case .loan(let acc): return acc.transactions
            case .creditCard(let acc): return acc.transactions
            case .borrow(let acc): return acc.transactions
            case .give(let acc): return acc.transactions
            case .fd(let acc): return acc.transactions
            case .rd(let acc): return acc.transactions
            case .chit(let acc): return acc.transactions
            case .property(let acc): return acc.transactions
            case .stock(let acc): return acc.transactions
            }
        }
        set {
            switch self {
            case .bank(var acc): acc.transactions = newValue; self = .bank(acc)
            case .loan(var acc): acc.transactions = newValue; self = .loan(acc)
            case .creditCard(var acc): acc.transactions = newValue; self = .creditCard(acc)
            case .borrow(var acc): acc.transactions = newValue; self = .borrow(acc)
            case .give(var acc): acc.transactions = newValue; self = .give(acc)
            case .fd(var acc): acc.transactions = newValue; self = .fd(acc)
            case .rd(var acc): acc.transactions = newValue; self = .rd(acc)
            case .chit(var acc): acc.transactions = newValue; self = .chit(acc)
            case .property(var acc): acc.transactions = newValue; self = .property(acc)
            case .stock(var acc): acc.transactions = newValue; self = .stock(acc)
            }
        }
    }

    var type: AccountType {
        switch self {
        case .bank: return .bank
        case .loan: return .loan
        case .creditCard: return .creditCard
        case .borrow: return .borrow
        case .give: return .give
        case .fd: return .fd
        case .rd: return .rd
        case .chit: return .chit
        case .property: return .property
        case .stock: return .stock
        }
    }

    func summary() -> String {
        switch self {
        case .bank(let acc): return acc.summary()
        case .loan(let acc): return acc.summary()
        case .creditCard(let acc): return acc.summary()
        case .borrow(let acc): return acc.summary()
        case .give(let acc): return acc.summary()
        case .fd(let acc): return acc.summary()
        case .rd(let acc): return acc.summary()
        case .chit(let acc): return acc.summary()
        case .property(let acc): return acc.summary()
        case .stock(let acc): return acc.summary()
        }
    }

    enum CodingKeys: String, CodingKey {
        case type, payload
    }

    enum AccountCodingError: Error {
        case unknownType
    }

    static func == (lhs: AnyAccount, rhs: AnyAccount) -> Bool {
        lhs.id == rhs.id
    }

    func hash(into hasher: inout Hasher) {
        hasher.combine(id)
    }

    init(from decoder: Decoder) throws {
        let container = try decoder.container(keyedBy: CodingKeys.self)
        let type = try container.decode(AccountType.self, forKey: .type)

        switch type {
        case .bank: self = .bank(try container.decode(BankAccount.self, forKey: .payload))
        case .loan: self = .loan(try container.decode(LoanAccount.self, forKey: .payload))
        case .creditCard: self = .creditCard(try container.decode(CreditCardAccount.self, forKey: .payload))
        case .borrow: self = .borrow(try container.decode(BorrowAccount.self, forKey: .payload))
        case .give: self = .give(try container.decode(GiveAccount.self, forKey: .payload))
        case .fd: self = .fd(try container.decode(FDAccount.self, forKey: .payload))
        case .rd: self = .rd(try container.decode(RDAccount.self, forKey: .payload))
        case .chit: self = .chit(try container.decode(ChitAccount.self, forKey: .payload))
        case .property: self = .property(try container.decode(PropertyAccount.self, forKey: .payload))
        case .stock: self = .stock(try container.decode(StockAccount.self, forKey: .payload))
        }
    }

    func encode(to encoder: Encoder) throws {
        var container = encoder.container(keyedBy: CodingKeys.self)
        try container.encode(self.type, forKey: .type)

        switch self {
        case .bank(let acc): try container.encode(acc, forKey: .payload)
        case .loan(let acc): try container.encode(acc, forKey: .payload)
        case .creditCard(let acc): try container.encode(acc, forKey: .payload)
        case .borrow(let acc): try container.encode(acc, forKey: .payload)
        case .give(let acc): try container.encode(acc, forKey: .payload)
        case .fd(let acc): try container.encode(acc, forKey: .payload)
        case .rd(let acc): try container.encode(acc, forKey: .payload)
        case .chit(let acc): try container.encode(acc, forKey: .payload)
        case .property(let acc): try container.encode(acc, forKey: .payload)
        case .stock(let acc): try container.encode(acc, forKey: .payload)
        }
    }
}
