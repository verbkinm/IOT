#pragma once

#include <vector>

#include "raii_header.h"

class FragmentManager_Identification
{
public:
    FragmentManager_Identification(size_t max_buff_size);

    void addPkg(RAII_Header pkg);
    RAII_Header pkg();

    size_t size() const;

    bool isComplete() const;
    bool overflow() const;
    bool error() const;

private:
    void clear();

    std::vector<RAII_Header> _buff;
    const size_t _max_buff_size;

    bool _overflow; // переполнение буфера
    bool _err;      // ошибка фрагментов
    size_t _size;   // размер буфера в байтах
};

