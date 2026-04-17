#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <sodium.h>

#define AUTH_KEY_SIZE 32U
#define LOCAL_ENCRYPT_KEY_SIZE 32U

struct RegistrationData{
    QByteArray authKey;
    QByteArray salt;
    QByteArray publicKey;
    QByteArray encryptedVault;
    QByteArray vaultNonce;
};

class CryptoService : public QObject
{
    Q_OBJECT
public:
    explicit CryptoService(QObject *parent = nullptr);

    RegistrationData generateNewAccount(const QString &password);
signals:

private:
    QByteArray m_salt[crypto_pwhash_SALTBYTES];
    unsigned char m_authKey[AUTH_KEY_SIZE];
    //unsigned char m_localEncryptKey[LOCAL_ENCRYPT_KEY_SIZE];
    unsigned char m_secretKey[crypto_box_SECRETKEYBYTES];
};

#endif // CRYPTOSERVICE_H
