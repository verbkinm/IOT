#include <QtTest>
#include <QVariant>

#include <cstring>
#include <algorithm>

#include <raw.h>

class Test_Raw : public QObject
{
    Q_OBJECT

public:
    Test_Raw();
    ~Test_Raw();

private:
    QByteArray foo(size_t size, const char *value);

private slots:
    void test_case1();

};

Test_Raw::Test_Raw()
{

}

Test_Raw::~Test_Raw()
{

}

QByteArray Test_Raw::foo(size_t size, const char *value)
{
    char* arr = new char[size];
    std::memcpy(arr, value, size);

    QByteArray data;
    for (size_t i = 0; i < size; i++)
        data.push_back(arr[i]);
    delete[] arr;

    return data;
}

void Test_Raw::test_case1()
{
    {
        // int8_t
        int8_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(static_cast<int8_t>(data.at(0)), value);

        Raw raw(Raw::DATA_TYPE::INT_8, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int16_t
        int16_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(*reinterpret_cast<int16_t*>(data.data()), value);

        Raw raw(Raw::DATA_TYPE::INT_16, data);
        QCOMPARE(raw.strData(), QString::number(value));

        {
            QByteArray data;
            data.push_back(0xab);
            data.push_back(0xcd);

            Raw raw(Raw::DATA_TYPE::INT_16, data);
            QCOMPARE(raw.strData(), "-21555");
        }
    }

    {
        // int32_t
        int32_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int32_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INT_32, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int64_t
        int64_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int64_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INT_64, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // float
        float value = -1;
        QByteArray data = foo(sizeof(float), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<float*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::FLOAT_32, data);
        QCOMPARE(raw.strData(), QString::number(value, 'l', 4));
    }

    {
        // double
        double value = 13213113.1564654;
        QByteArray data = foo(sizeof(double), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<double*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::DOUBLE_64, data);
        QCOMPARE(raw.strData(), QString::number(value, 'l', 4));
    }

    {
        // bool
        bool value = false;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<bool*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::BOOL, data);
        QCOMPARE(raw.strData(), value ? "true" : "false");
    }

    {
        // string
        QString value = "Hello string!!";
        QByteArray data = value.toLocal8Bit();

        QCOMPARE(data.data(), value);

        Raw raw(Raw::DATA_TYPE::STRING, data);
        QCOMPARE(raw.strData(), value);
    }

    {
        // raw
        QByteArray dataRaw;
        dataRaw.push_back(0xab);
        dataRaw.push_back(0xcd);

        Raw raw(Raw::DATA_TYPE::RAW, dataRaw);
        QCOMPARE(raw.strData(), "ab:cd");
    }

    {
        // none
        QString value = "";
        QByteArray data = value.toLocal8Bit();

        QCOMPARE(data.data(), value);

        Raw raw(Raw::DATA_TYPE::NONE, data);
        QCOMPARE(raw.strData(), value);
    }

}

QTEST_APPLESS_MAIN(Test_Raw)

#include "tst_test_raw.moc"
