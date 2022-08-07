#include <QtTest>
#include <QVariant>

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
        // uint8_t
        uint8_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);\

        Raw raw(Raw::DATA_TYPE::UNSIGNED_INTEGER_8, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // uint16_t
        uint16_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(*reinterpret_cast<uint16_t*>(data.data()), value);

        Raw raw(Raw::DATA_TYPE::UNSIGNED_INTEGER_16, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // uint32_t
        uint32_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<uint32_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::UNSIGNED_INTEGER_32, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // uint64_t
        uint64_t value = -1234567890;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<uint64_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::UNSIGNED_INTEGER_64, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int8_t
        int8_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(static_cast<int8_t>(data.at(0)), value);

        Raw raw(Raw::DATA_TYPE::INTEGER_8, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int16_t
        int16_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(*reinterpret_cast<int16_t*>(data.data()), value);

        Raw raw(Raw::DATA_TYPE::INTEGER_16, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int32_t
        int32_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int32_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INTEGER_32, data);
        QCOMPARE(raw.strData(), QString::number(value));
    }

    {
        // int64_t
        int64_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int64_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INTEGER_64, data);
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

        Raw raw(Raw::DATA_TYPE::BOOL_8, data);
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
