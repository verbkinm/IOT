#include "fragmentcollector.h"

Fragment_Collector::Fragment_Collector(size_t max_buff_size) :
    _max_buff_size(max_buff_size), _overflow(false), _err(false), _size(0)
{

}

void Fragment_Collector::addPkg(RAII_Header pkg)
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

//RAII_Header Fragment_Collector::pkg()
//{

//}

size_t Fragment_Collector::size() const
{
    return _size;
}

bool Fragment_Collector::isComplete() const
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

bool Fragment_Collector::overflow() const
{
    return _overflow;
}

bool Fragment_Collector::error() const
{
    return _err;
}

void Fragment_Collector::clear()
{
    _buff.clear();
    _size = 0;
}
