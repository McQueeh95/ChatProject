#ifndef SECUREBUFFER_H
#define SECUREBUFFER_H

#include <QString>
#include <QByteArray>
#include <sodium.h>
#include <cstring>
#include <QDebug>

class SecureBuffer
{
public:
    SecureBuffer() = default;

    ~SecureBuffer()
    {
        if(m_data)
            sodium_free(m_data);
    }

    void load(const QString &password)
    {
        clear();
        QByteArray utf8 = password.toUtf8();

        m_size = utf8.size();

        m_data = static_cast<char*>(sodium_malloc(m_size));
        memcpy(m_data, utf8.constData(), m_size);
        utf8.fill('\0');
    }

    void load(const unsigned char* data, size_t size)
    {
        clear();
        m_size = size;
        m_data = static_cast<char*>(sodium_malloc(m_size));

        if(m_data){
            memcpy(m_data, data, m_size);
        }
    }

    void load(const QByteArray &bytes)
    {
        load(reinterpret_cast<const unsigned char*>(bytes.constData()), bytes.size());
    }

    bool isEmpty() const { return m_data == nullptr; }
    const char* dataChar() const { return static_cast<char*>(m_data); }
    const unsigned char* dataUCHar() const {return static_cast<unsigned char*>(m_data); }
    size_t size() const {return m_size; }
    void clear()
    {
        if(m_data)
        {
            sodium_free(m_data);
            m_data = nullptr;
            m_size = 0;
        }
    }

    SecureBuffer(const SecureBuffer&) = delete;
    SecureBuffer& operator=(const SecureBuffer&) = delete;

private:
    void* m_data = nullptr;
    size_t m_size = 0;
};

#endif // SECUREBUFFER_H
