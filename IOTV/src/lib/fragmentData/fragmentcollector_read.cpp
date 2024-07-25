#include "fragmentcollector_read.h"
#include "read_write.h"
#include "iotv_types.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Fragment_Collector_Read::Fragment_Collector_Read(size_t max_buff_size) : Fragment_Collector(max_buff_size)
{

}

RAII_Header Fragment_Collector_Read::pkg()
{
    if (_buff.size() == 0)
        return {};

    // Копия первого элемента буфера
    RAII_Header result_raii_header = _buff.at(0);

    read_write_t *result_rw_pkg = static_cast<read_write_t *>(result_raii_header.header()->pkg);
    if (result_rw_pkg == nullptr)
    {
        RETURN_WARNING;
        return {};
    }

    result_rw_pkg->dataSize = 0;
    free(result_rw_pkg->data);
    result_rw_pkg->data = nullptr;

    // Считаем размер описания из всех пакетов
    size_t totalDataSize = 0;
    for (const auto &el : _buff)
    {
        read_write_t *rw_pkg = static_cast<read_write_t *>(el.header()->pkg);
        if (rw_pkg == nullptr)
        {
            RETURN_WARNING;
            return {};
        }

        totalDataSize += rw_pkg->dataSize;
    }

    if (totalDataSize == 0)
    {
        RETURN_WARNING;
        return result_raii_header;
    }

    // Выделяем память для цельных данных
    result_rw_pkg->dataSize = totalDataSize;
    result_rw_pkg->data = (char *)malloc(totalDataSize);

    if (result_rw_pkg->data == nullptr)
    {
        RETURN_WARNING;
        return {};
    }

    size_t offset = 0;
    for (const auto &el : _buff)
    {
        read_write_t *rw_pkg = static_cast<read_write_t *>(el.header()->pkg);
        if (rw_pkg == nullptr)
        {
            RETURN_WARNING;
            return {};
        }

        memcpy((result_rw_pkg->data + offset), rw_pkg->data, rw_pkg->dataSize);
        free(rw_pkg->data);
        rw_pkg->data = nullptr;
        rw_pkg->dataSize = 0;

        offset += rw_pkg->dataSize;
    }
    clear();

    result_raii_header.header()->dataSize = READ_WRITE_SIZE + totalDataSize;

    return result_raii_header;
}
