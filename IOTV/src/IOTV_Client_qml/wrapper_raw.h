#ifndef WRAPPER_RAW_H
#define WRAPPER_RAW_H

#include <QObject>
#include "lib/raw/raw.h"

class Wrapper_Raw : public QObject
{
    Q_OBJECT
public:
    explicit Wrapper_Raw(QObject *parent = nullptr);


    Raw::RAW raw;

signals:

};

#endif // WRAPPER_RAW_H
