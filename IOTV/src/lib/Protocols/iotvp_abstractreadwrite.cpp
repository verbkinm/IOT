#include "iotvp_abstractreadwrite.h"

uint64_t IOTVP_AbstractReadWrite::dataSize() const
{
    return (_data.size() > std::numeric_limits<uint32_t>::max()) ? std::numeric_limits<uint32_t>::max() : _data.size();
}

const QByteArray &IOTVP_AbstractReadWrite::data() const
{
    return _data;
}

void IOTVP_AbstractReadWrite::setData(const QByteArray &newData)
{
    if (_data != newData)
        _data = (newData.size() > std::numeric_limits<uint32_t>::max()) ? newData.mid(0, std::numeric_limits<uint32_t>::max()) : newData;
}
