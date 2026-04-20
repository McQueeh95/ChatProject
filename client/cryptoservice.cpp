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

    unsigned char masterkey[crypto_kdf_KEYBYTES];

    if(crypto_pwhash
        (masterkey, sizeof(masterkey), password.toUtf8().constData(), password.toUtf8().size(),
                      reinterpret_cast<const unsigned char*>(data.salt.constData()),
                    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0)
    {
        qDebug() << "Out of memory crypto_pwhash";
    }

    data.authKey.resize(AUTH_KEY_LEN);
    crypto_kdf_derive_from_key(reinterpret_cast<unsigned char*>(data.authKey.data()), data.authKey.size(),
                               static_cast<uint64_t>(KdfConfig::KeyId::ServerAuth),
                               KdfConfig::CONTEXT_SERVER, masterkey);

    unsigned char localEncryptKey[LOCAL_ENC_KEY_LEN];
    crypto_kdf_derive_from_key(localEncryptKey, LOCAL_ENC_KEY_LEN,
                               static_cast<uint64_t>(KdfConfig::KeyId::LocalVault),
                               KdfConfig::CONTEXT_VAULT, masterkey);

    sodium_memzero(masterkey, sizeof(masterkey));

    //Keypair generation
    data.publicKey.resize(crypto_box_PUBLICKEYBYTES);
    m_secretKey = static_cast<unsigned char*>(sodium_malloc(crypto_box_SECRETKEYBYTES));
    crypto_box_keypair(reinterpret_cast<unsigned char*>(data.publicKey.data()), m_secretKey);

    //Encryption of secret key to store on sercer
    data.vaultNonce.resize(crypto_secretbox_NONCEBYTES);
    unsigned char cipheredSecretKey[crypto_secretbox_MACBYTES + 32];
    randombytes_buf(data.vaultNonce.data(), sizeof(data.vaultNonce));

    crypto_secretbox_easy(cipheredSecretKey, m_secretKey, crypto_box_SECRETKEYBYTES, reinterpret_cast<const unsigned char*>(data.vaultNonce.constData()),
                          localEncryptKey);
    sodium_memzero(localEncryptKey, sizeof(localEncryptKey));

    data.encryptedVault = QByteArray(reinterpret_cast<unsigned char*>(cipheredSecretKey));

    return data;
}

DerivedKeys CryptoService::generateHashedPassword(const char* password, size_t pwdLen, const QByteArray& salt)
{
    DerivedKeys keys;
    unsigned char masterkey[crypto_kdf_KEYBYTES];

    if(crypto_pwhash
        (masterkey, sizeof(masterkey), password, pwdLen,
         reinterpret_cast<const unsigned char*>(salt.constData()),
         crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
         crypto_pwhash_ALG_DEFAULT) != 0)
    {
        qDebug() << "Out of memory crypto_pwhash";
        return keys;
    }

    keys.authKey.resize(AUTH_KEY_LEN);
    crypto_kdf_derive_from_key(reinterpret_cast<unsigned char*>(keys.authKey.data()), keys.authKey.size(),
                               static_cast<uint64_t>(KdfConfig::KeyId::ServerAuth),
                               KdfConfig::CONTEXT_SERVER, masterkey);

    keys.localEncryptKey.resize(LOCAL_ENC_KEY_LEN);
    crypto_kdf_derive_from_key(reinterpret_cast<unsigned char*>(keys.localEncryptKey.data()), keys.localEncryptKey.size(),
                               static_cast<uint64_t>(KdfConfig::KeyId::LocalVault),
                               KdfConfig::CONTEXT_VAULT, masterkey);

    sodium_memzero(masterkey, MASTER_KEY_LEN);

    return keys;
}

void CryptoService::decryptSecretKey(const QByteArray &encryptedVault, const QByteArray& nonce, const unsigned char* key)
{
    m_secretKey = static_cast<unsigned char*>(sodium_malloc(crypto_box_SECRETKEYBYTES));
    if(crypto_secretbox_open_easy(m_secretKey, reinterpret_cast<const unsigned char*>(encryptedVault.constData()),
                               encryptedVault.size(), reinterpret_cast<const unsigned char*>(nonce.constData()), (key)) != 0)
    {
        qDebug() << "Message forged!";
    }
}
