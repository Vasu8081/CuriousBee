import Foundation
import CryptoKit

enum CryptoKeyManager {
    private static var keyTag: String {
        let appName = Bundle.main.bundleIdentifier ?? "default.app"
        return appName + ".crypto.key"
    }

    static func getSymmetricKey() -> SymmetricKey? {
        let tag = keyTag.data(using: .utf8)!

        // Try to find existing key
        var query: [String: Any] = [
            kSecClass as String: kSecClassKey,
            kSecAttrApplicationTag as String: tag,
            kSecReturnData as String: true
        ]

        var result: AnyObject?
        let status = SecItemCopyMatching(query as CFDictionary, &result)

        if status == errSecSuccess, let keyData = result as? Data {
            return SymmetricKey(data: keyData)
        }

        // If key not found, generate and store a new one
        let newKey = SymmetricKey(size: .bits256)
        let keyData = newKey.withUnsafeBytes { Data($0) }

        query = [
            kSecClass as String: kSecClassKey,
            kSecAttrApplicationTag as String: tag,
            kSecValueData as String: keyData,
            kSecAttrAccessible as String: kSecAttrAccessibleWhenUnlockedThisDeviceOnly
        ]

        let addStatus = SecItemAdd(query as CFDictionary, nil)
        if addStatus == errSecSuccess {
            return newKey
        } else {
            print("Failed to store encryption key: \(addStatus)")
            return nil
        }
    }
}

extension Data {
    func encrypt() -> Data? {
        guard let key = CryptoKeyManager.getSymmetricKey() else { return nil }
        do {
            let sealedBox = try AES.GCM.seal(self, using: key)
            return sealedBox.combined
        } catch {
            print("Encryption error: \(error)")
            return nil
        }
    }

    func decrypt() -> Data? {
        guard let key = CryptoKeyManager.getSymmetricKey() else { return nil }
        do {
            let sealedBox = try AES.GCM.SealedBox(combined: self)
            return try AES.GCM.open(sealedBox, using: key)
        } catch {
            print("Decryption error: \(error)")
            return nil
        }
    }
}
