#ifndef PROTOCOL_CLASS_H
#define PROTOCOL_CLASS_H

#include "iot_server.h"

template <class T>
class Array
{
private:
    int _size;
    T *_data;
public:
    Array()
    {
        _size = 0;
        _data = nullptr;
    }

    Array(int length)
    {
        _data = new T[length];
        _size = length;
    }

    ~Array()
    {
        if(_data)
            delete[] _data;
    }

    void push_back(T value)
    {
        int newSize = _size + 1;
        T* tmp_data = new T[newSize];

        if(_data)
            memcpy(tmp_data, _data, _size * sizeof(T));

        tmp_data[newSize - 1] = value;

        clear();

        _data = new T[newSize];
        _size = newSize;

        memcpy(_data, tmp_data, _size * sizeof (T));

        delete[] tmp_data;
    }

    void clear()
    {
        if(_data)
        {
            delete[] _data;
            _data = nullptr;
            _size = 0;
        }
    }

    T& operator[](int index)
    {
//        if(index < 0 || index >= _size)
//            exit(-1);

        return _data[index];
    }
    Array<T>& operator=(Array<T>& rhs)
    {
        clear();
        _data = new T[rhs.size()];
        _size = rhs.size();
        if(rhs._data)
            memcpy(_data, rhs._data, _size * sizeof(T));

        return *this;
    }

    int size()
    {
        return _size;
    }

//    T* data()
//    {
//        return _data;
//    }

    void remove(int from, int count)
    {
        Array<T> tmpData;

        for(int i = 0; i < size(); i++)
        {
            if( (i < from) || (i >= (from + count)) )
                tmpData.push_back(_data[i]);
        }
        *this = tmpData;
    }
};
 
//template <typename T> // метод, определенный вне тела класса, нуждается в собственном определении шаблона метода
//int Array<T>::getLength()  // обратите внимание, имя класса - Array<T>, а не просто Array
 

class Protocol_class
{
public:
    enum class query_type
    {
        QUERY_WAY,
        QUERY_READ,
        QUERY_WRITE,
        ERROR
    };

    static query_type checkQueryData(const Array<char> &data);

    static void response_WAY(const IOT_Server &iotHost, Array<char> &data);
    static void response_READ(const IOT_Server &iotHost, Array<char> &data);
    static void response_WRITE(IOT_Server &iotHost, Array<char> &data);
};

#endif // PROTOCOL_CLASS_H
