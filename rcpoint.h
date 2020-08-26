#ifndef RCPOINT_H
#define RCPOINT_H

#include <QPoint>
#include <QJsonArray>


class RCPoint{
    int row_ = 0, col_ = 0;

public:
    RCPoint();
    RCPoint(int row, int col);
    ~RCPoint();

    int row() const;
    int col() const;

    void read(const QJsonArray& json);
    void write(QJsonArray& json) const;

    bool operator==(const RCPoint& other) const;
};

#endif // RCPOINT_H
