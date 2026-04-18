#include "cryptoservice.h"
#include <QDebug>

CryptoService::CryptoService(QObject *parent)
    : QObject{parent}
{}

RegistrationData CryptoService::generateNewAccount(const QString &password)
{
    RegistrationData data;
    data.salt.resize(crypto_pwhash_SALTBYTES);
    randombytes_buf(data.salt.data(), data.salt.size());

    const qint8 TOTAL_KEY_LEN = 64;
    unsigned char fullKey[TOTAL_KEY_LEN];

    if(crypto_pwhash
        (fullKey, TOTAL_KEY_LEN, password.toUtf8().constData(), password.toUtf8().size(),
                      reinterpret_cast<const unsigned char*>(data.salt.constData()),
                    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        qDebug() << "Out of memory crypto_pwhash";
    }

    data.authKey = QByteArray(reinterpret_cast<char*>(fullKey), 32);
    unsigned char localEncryptKey[32];
    memcpy(localEncryptKey, fullKey + 32, 32);

    sodium_memzero(fullKey, sizeof(fullKey));

    //Keypair generation
    data.publicKey.resize(crypto_box_PUBLICKEYBYTES);
    m_secretKey = static_cast<unsigned char*>(sodium_malloc(crypto_box_SECRETKEYBYTES));
    crypto_box_keypair(reinterpret_cast<unsigned char*>(data.publicKey.data()), m_secretKey);

    //Encryption of secret key to store on sercer
    data.vaultNonce.resize(crypto_secretbox_NONCEBYTES);
    unsigned char cipheredSecretKey[crypto_secretbox_MACBYTES + 32];
    randombytes_buf(data.vaultNonce.data(), sizeof(data.vaultNonce));

    crypto_secretbox_easy(cipheredSecretKey, m_secretKey, 32, reinterpret_cast<const unsigned char*>(data.vaultNonce.constData()),
                          localEncryptKey);
    sodium_memzero(localEncryptKey, sizeof(localEncryptKey));

    data.encryptedVault = QByteArray(reinterpret_cast<unsigned char*>(cipheredSecretKey));

    return data;
}

DerivedKeys CryptoService::generateHashedPassword(const char* password, size_t pwdLen, const QByteArray& salt)
{
    DerivedKeys keys;
    const size_t TOTAL_KEY_LEN = 64;
    unsigned char fullKey[TOTAL_KEY_LEN];

    if(crypto_pwhash
        (fullKey, TOTAL_KEY_LEN, password, pwdLen,
         reinterpret_cast<const unsigned char*>(salt.constData()),
         crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
         crypto_pwhash_ALG_DEFAULT) != 0)
    {
        qDebug() << "Out of memory crypto_pwhash";
        return keys;
    }
    keys.authKey = QByteArray(reinterpret_cast<char*>(fullKey), 32);
    keys.localEncryptKey = QByteArray(reinterpret_cast<char*>(fullKey) + 32, 32);
    sodium_memzero(fullKey, TOTAL_KEY_LEN);
    return keys;
}

void CryptoService::decryptSecretKey(const QByteArray &encryptedVault, const QByteArray& nonce, const char* key)
{
    m_secretKey = static_cast<unsigned char*>(sodium_malloc(crypto_box_SECRETKEYBYTES));
    if(crypto_secretbox_open_easy(m_secretKey, reinterpret_cast<const unsigned char*>(encryptedVault.constData()),
                               encryptedVault.size(), reinterpret_cast<const unsigned char*>(nonce.constData()),
                                   reinterpret_cast<const unsigned char*>(key)) != 0)
    {
        qDebug() << "Message forged!";
    }
}
