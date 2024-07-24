#include "fragmentmanager_identification.h"
#include "header.h"
#include "identification.h"
#include "iotv_types.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

FragmentManager_Identification::FragmentManager_Identification(size_t max_buff_size) :
    _max_buff_size(max_buff_size), _overflow(false), _err(false), _size(0)
{

}

void FragmentManager_Identification::addPkg(RAII_Header pkg)
{
    header_t *header = pkg.header();
    uint16_t fragment = header->fragment - 1; // отчет фрагментов в header идёт с единицы
    uint16_t fragments = header->fragments;
    uint64_t pkgSize = headerSize(header);

    // Если размер пакета больше максимального размера буфера
    if (_size + pkgSize >= _max_buff_size)
    {
        _overflow = true;
        clear();
        return;
    }

    // Если буфер пуст
    if (_buff.size() == 0)
    {
        _buff.resize(fragments);
        _buff.at(fragment) = pkg;
        _size += pkgSize;
        return;
    }

    // Если номер фрагмента превышает размер фрагментов буфера
    // Или количество фрагментов пакета не равно количеству фрагментов буфера
    if ((fragment >= _buff.size()) || (fragments != _buff.size()))
    {
        _err = true;
        clear();
        return;
    }

    _buff.at(fragment) = pkg;
    _size += pkgSize;

    _overflow = false;
}

RAII_Header FragmentManager_Identification::pkg()
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

void FragmentManager_Identification::clear()
{
    _buff.clear();
    _size = 0;
}

size_t FragmentManager_Identification::size() const
{
    return _size;
}

bool FragmentManager_Identification::isComplete() const
{
    if (_buff.size() == 0)
        return false;

    for (const auto &el : _buff)
    {
        if (el.header() == nullptr)
            return false;
    }

    return true;
}

bool FragmentManager_Identification::overflow() const
{
    return _overflow;
}

bool FragmentManager_Identification::error() const
{
    return _err;
}
