#include "pointconverter.h"

int PointConverter::left = 0;
int PointConverter::top = 0;
int PointConverter::size = 0;

void PointConverter::setTop(int value){
    top = value;
}

void PointConverter::setLeft(int value){
    left = value;
}

void PointConverter::setSize(int value){
    size = value;
}

QPoint PointConverter::rc2xy(const RCPoint &rc){
    int y = top + rc.row() * 13;
    int x = left + rc.col() * 13;
    return QPoint(x, y);
}

RCPoint PointConverter::xy2rc(const QPoint &xy){
    int row = (xy.y() - top) / size;
    int col = (xy.x() - left) / size;
    return RCPoint(row, col);
}
