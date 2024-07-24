#pragma once

#include <vector>

#include "raii_header.h"

class Fragment_Collector
{
public:
    explicit Fragment_Collector(size_t max_buff_size);

    void addPkg(RAII_Header pkg);
    virtual RAII_Header pkg() = 0;

    size_t size() const;

    bool isComplete() const;
    bool overflow() const;
    bool error() const;

protected:
    void clear();

    std::vector<RAII_Header> _buff;
    const size_t _max_buff_size;

    bool _overflow; // переполнение буфера
    bool _err;      // ошибка фрагментов
    size_t _size;   // размер буфера в байтах
};
