#include "fragmentcollector_identification.h"
#include "header.h"
#include "identification.h"
#include "iotv_types.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

FragmentCollector_Identification::FragmentCollector_Identification(size_t max_buff_size) : Fragment_Collector(max_buff_size)
{

}

RAII_Header FragmentCollector_Identification::pkg()
{
    if (_buff.size() == 0)
        return {};

    // Копия первого элемента буфера
    RAII_Header result_raii_header = _buff.at(0);

    Identification *result_ident_pkg = static_cast<Identification *>(result_raii_header.header()->pkg);
    if (result_ident_pkg == nullptr)
    {
        RETURN_WARNING;
        return {};
    }

    result_ident_pkg->descriptionSize = 0;
    free(result_ident_pkg->description);
    result_ident_pkg->description = nullptr;

    // Считаем размер описания из всех пакетов
    size_t totalDescriptionSize = 0;
    for (const auto &el : _buff)
    {
        Identification *ident_pkg = static_cast<Identification *>(el.header()->pkg);
        if (ident_pkg == nullptr)
        {
            RETURN_WARNING;
            return {};
        }

        totalDescriptionSize += ident_pkg->descriptionSize;
    }

    if (totalDescriptionSize == 0)
    {
        RETURN_WARNING;
        return result_raii_header;
    }

    // Выделяем память для цельных данных
    result_ident_pkg->descriptionSize = totalDescriptionSize;
    result_ident_pkg->description = (char *)malloc(totalDescriptionSize);

    if (result_ident_pkg->description == nullptr)
    {
        RETURN_WARNING;
        return {};
    }

    size_t offset = 0;
    for (const auto &el : _buff)
    {
        Identification *ident_pkg = static_cast<Identification *>(el.header()->pkg);
        if (ident_pkg == nullptr)
        {
            RETURN_WARNING;
            return {};
        }

        memcpy((result_ident_pkg->description + offset), ident_pkg->description, ident_pkg->descriptionSize);
        offset += ident_pkg->descriptionSize;
    }

    result_raii_header.header()->dataSize = IDENTIFICATION_SIZE + totalDescriptionSize;

    return result_raii_header;
}
