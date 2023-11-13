#ifndef WRAP_QBYTEARRAY_H
#define WRAP_QBYTEARRAY_H

#include <QObject>

class Wrap_QByteArray
{
public:
    Wrap_QByteArray();

    void setData(const QByteArray data);
    QByteArray data() const;

private:
    QByteArray _data;
};

#endif // WRAP_QBYTEARRAY_H
