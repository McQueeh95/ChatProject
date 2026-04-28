#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <sodium.h>
#include "securebuffer.h"

#define AUTH_KEY_SIZE 32U
#define LOCAL_ENCRYPT_KEY_SIZE 32U

namespace KdfConfig{
    enum class KeyId: uint64_t{
        ServerAuth = 1,
        LocalVault = 2
    };

    constexpr char CONTEXT_SERVER[] = "ServerAu";
    constexpr char CONTEXT_VAULT[] = "LocalEnc";
}

struct RegistrationData{
    QByteArray authKey;
    QByteArray salt;
    QByteArray publicKey;
    QByteArray encryptedVault;
    QByteArray vaultNonce;
};

struct DerivedKeys{
    QByteArray authKey;
    QByteArray localEncryptKey;
};

struct EncryptedMessage{
    QByteArray cipherText;
    QByteArray nonce;
};

class CryptoService : public QObject
{
    Q_OBJECT
public:
    explicit CryptoService(QObject *parent = nullptr);

    RegistrationData generateNewAccount(const QString &password);
    DerivedKeys generateHashedPassword(const char* password, size_t pwdLen, const QByteArray& salt);
    void decryptSecretKey(const QByteArray& encryptedVault, const QByteArray& nonce, const unsigned char* key);
    EncryptedMessage encryptMessage(const QString& text, const QByteArray& peerPublicKey);
    QString decryptMessage(const QByteArray &cipheredText, const QByteArray& peerPublicKey, const QByteArray &nonce);
    void clearSecretKey();

    static constexpr size_t MASTER_KEY_LEN = 32;
    static constexpr size_t AUTH_KEY_LEN = 32;
    static constexpr size_t LOCAL_ENC_KEY_LEN = 32;
signals:

private:
    unsigned char* m_secretKey = nullptr;
};

#endif // CRYPTOSERVICE_H
