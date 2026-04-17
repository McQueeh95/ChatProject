#ifndef CRYPTOSERVICE_H
#define CRYPTOSERVICE_H

#include <QObject>
#include <QByteArray>
#include <sodium.h>
#include "securebuffer.h"

#define AUTH_KEY_SIZE 32U
#define LOCAL_ENCRYPT_KEY_SIZE 32U

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

class CryptoService : public QObject
{
    Q_OBJECT
public:
    explicit CryptoService(QObject *parent = nullptr);

    RegistrationData generateNewAccount(const QString &password);
    DerivedKeys generateHashedPassword(const char* password, const QByteArray& salt);
signals:

private:
    unsigned char m_secretKey[crypto_box_SECRETKEYBYTES];
};

#endif // CRYPTOSERVICE_H
