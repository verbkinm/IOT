#include "iotvp_abstractreadwrite.h"

uint32_t IOTVP_AbstractReadWrite::dataSize() const
{
    return _data.size();
}

const QByteArray &IOTVP_AbstractReadWrite::data() const
{
    return _data;
}

void IOTVP_AbstractReadWrite::setData(const QByteArray &newData)
{
    _data = newData;
}
