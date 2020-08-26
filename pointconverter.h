#ifndef POINTCONVERTER_H
#define POINTCONVERTER_H

#include <QPoint>

#include "rcpoint.h"


class PointConverter{
    static int left, top;
    static int size;

public:
    static QPoint rc2xy(const RCPoint& rc);
    static RCPoint xy2rc(const QPoint& xy);

    static void setLeft(int value);
    static void setTop(int value);
    static void setSize(int value);

private:
    PointConverter() = default;
    ~PointConverter() = default;
};

#endif // POINTCONVERTER_H
