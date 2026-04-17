#ifndef SECUREBUFFER_H
#define SECUREBUFFER_H

#include <QString>
#include <QByteArray>
#include <sodium.h>
#include <cstring>

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

    bool isEmpty() const { return m_data == nullptr; }
    const char* data() const { return m_data; }
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
    char* m_data = nullptr;
    size_t m_size = 0;
};

#endif // SECUREBUFFER_H
