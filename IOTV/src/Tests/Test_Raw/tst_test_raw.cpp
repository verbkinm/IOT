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
    void test_case2();

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
        QCOMPARE(raw.strData().first, QString::number(value));
    }

    {
        // int16_t
        int16_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(*reinterpret_cast<int16_t*>(data.data()), value);

        Raw raw(Raw::DATA_TYPE::INT_16, data);
        QCOMPARE(raw.strData().first, QString::number(value));

        {
            QByteArray data;
            data.push_back(0xab);
            data.push_back(0xcd);

            Raw raw(Raw::DATA_TYPE::INT_16, data);
            QCOMPARE(raw.strData().first, "-12885");
        }
    }

    {
        // int32_t
        int32_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int32_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INT_32, data);
        QCOMPARE(raw.strData().first, QString::number(value));
    }

    {
        // int64_t
        int64_t value = -1;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<int64_t*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::INT_64, data);
        QCOMPARE(raw.strData().first, QString::number(value));
    }

    {
        // float
        float value = -1;
        QByteArray data = foo(sizeof(float), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<float*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::FLOAT_32, data);
        QCOMPARE(raw.strData().first, QString::number(value, 'l', 4));
    }

    {
        // double
        double value = 13213113.1564654;
        QByteArray data = foo(sizeof(double), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<double*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::DOUBLE_64, data);
        QCOMPARE(raw.strData().first, QString::number(value, 'l', 4));
    }

    {
        // bool
        bool value = false;
        QByteArray data = foo(sizeof(value), (const char*)&value);

        QCOMPARE(QString::number(*reinterpret_cast<bool*>(data.data())), QString::number(value));

        Raw raw(Raw::DATA_TYPE::BOOL, data);
        QCOMPARE(raw.strData().first, value ? "true" : "false");
    }

    {
        // string
        QString value = "Hello string!!";
        QByteArray data = value.toLocal8Bit();

        QCOMPARE(data.data(), value);

        Raw raw(Raw::DATA_TYPE::STRING, data);
        QCOMPARE(raw.strData().first, value);
    }

    {
        // raw
        QByteArray dataRaw;
        dataRaw.push_back(0xab);
        dataRaw.push_back(0xcd);

        Raw raw(Raw::DATA_TYPE::RAW, dataRaw);
        QCOMPARE(raw.strData().first, "ab:cd");
    }

    {
        // none
        QString value = "";
        QByteArray data = value.toLocal8Bit();

        QCOMPARE(data.data(), value);

        Raw raw(Raw::DATA_TYPE::NONE, data);
        QCOMPARE(raw.strData().first, value);
    }

}

void Test_Raw::test_case2()
{
    {
        int arg1 = 10;
        int arg2 = 0;

        Raw lhs(static_cast<int8_t>(arg1));
        Raw rhs(static_cast<int8_t>(arg2));

        QCOMPARE(lhs.size(), 1);
        QCOMPARE(lhs.strData().first, QString::number(arg1));
        QCOMPARE(rhs.strData().first, QString::number(arg2));

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        QCOMPARE(lhs + rhs, Raw(static_cast<int8_t>(arg1 + arg2)));
        QCOMPARE(lhs - rhs, Raw(static_cast<int8_t>(arg1 - arg2)));
        QCOMPARE(lhs * rhs, Raw(static_cast<int8_t>(arg1 * arg2)));
        QCOMPARE(lhs / rhs, Raw());
    }

    {
        int64_t arg1 = 6532;
        int64_t arg2 = 0;

        Raw lhs(static_cast<int64_t>(arg1));
        Raw rhs(static_cast<int64_t>(arg2));

        QCOMPARE(lhs.size(), sizeof(int64_t));
        QCOMPARE(lhs.strData().first, QString::number(arg1));
        QCOMPARE(rhs.strData().first, QString::number(arg2));

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        QCOMPARE(lhs + rhs, Raw(static_cast<int64_t>(arg1 + arg2)));
        QCOMPARE(lhs - rhs, Raw(static_cast<int64_t>(arg1 - arg2)));
        QCOMPARE(lhs * rhs, Raw(static_cast<int64_t>(arg1 * arg2)));
        QCOMPARE(lhs / rhs, Raw());
    }

    {
        double arg1 = 6532.369595;
        double arg2 = 0.0;

        Raw lhs(static_cast<double>(arg1));
        Raw rhs(static_cast<double>(arg2));

        QCOMPARE(lhs.size(), sizeof(double));
        QCOMPARE(lhs.strData().first, QString::number(arg1, 'g', 8));
        QCOMPARE(rhs.strData().first, "0.0000");//QString::number(arg2));

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        QCOMPARE(lhs + rhs, Raw(static_cast<double>(arg1 + arg2)));
        QCOMPARE(lhs - rhs, Raw(static_cast<double>(arg1 - arg2)));
        QCOMPARE(lhs * rhs, Raw(static_cast<double>(arg1 * arg2)));
        QCOMPARE(lhs / rhs, Raw());
    }

    {
        QString arg1 = "string3";
        QString arg2 = "string2";

        Raw lhs(arg1);
        Raw rhs(arg2);

        QCOMPARE(lhs.size(), arg1.toStdString().size());
        QCOMPARE(lhs.strData().first, arg1);
        QCOMPARE(rhs.strData().first, arg2);

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        Raw raw = (lhs + rhs);
        Raw raw2 (arg1 + arg2);
        QCOMPARE(lhs + rhs, Raw(arg1 + arg2));
        QCOMPARE(lhs - rhs, Raw());
        QCOMPARE(lhs * rhs, Raw());
        QCOMPARE(lhs / rhs, Raw());
    }

    // Разные типы
    {
        int8_t arg1 = 123;
        int64_t arg2 = -32;

        Raw lhs(arg1);
        Raw rhs(arg2);

        QCOMPARE(lhs.size(), sizeof(arg1));
        QCOMPARE(lhs.strData().first, QString::number(arg1));
        QCOMPARE(rhs.strData().first, QString::number(arg2));

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        QCOMPARE(lhs + rhs, Raw(static_cast<double>(arg1 + arg2)));
        QCOMPARE(lhs - rhs, Raw(static_cast<double>(arg1 - arg2)));
        QCOMPARE(lhs * rhs, Raw(static_cast<double>(arg1 * arg2)));
        QCOMPARE(lhs / rhs, Raw(static_cast<double>(arg1) / arg2));
    }

    {
        int8_t arg1 = 1;
        double arg2 = -12.32;

        Raw lhs(arg1);
        Raw rhs(arg2);

        QCOMPARE(lhs.size(), sizeof(arg1));
        QCOMPARE(lhs.strData().first, QString::number(arg1));
        QCOMPARE(rhs.strData().first, "-12.3200");

        QCOMPARE(lhs == rhs, false);
        QCOMPARE(lhs != rhs, true);
        QCOMPARE(lhs < rhs, false);
        QCOMPARE(lhs > rhs, true);
        QCOMPARE(lhs <= rhs, false);
        QCOMPARE(lhs >= rhs, true);

        QCOMPARE(lhs + rhs, Raw(static_cast<double>(arg1 + arg2)));
        QCOMPARE(lhs - rhs, Raw(static_cast<double>(arg1 - arg2)));
        QCOMPARE(lhs * rhs, Raw(static_cast<double>(arg1 * arg2)));
        QCOMPARE(lhs / rhs, Raw(static_cast<double>(arg1 / arg2)));
    }
}

QTEST_APPLESS_MAIN(Test_Raw)

#include "tst_test_raw.moc"
