#include "wrap_qbytearray.h"

Wrap_QByteArray::Wrap_QByteArray()
{

}

void Wrap_QByteArray::setData(const QByteArray data)
{
    _data = data;
}

QByteArray Wrap_QByteArray::data() const
{
    return _data;
}
