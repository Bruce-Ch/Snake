#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QVector>
#include <QQueue>
#include <QJsonArray>
#include <QJsonObject>

#include <cstdlib>

#include "rcpoint.h"


class Playground{
public:
    enum PointState{
        Empty, Barrier, Target, Body
    };

    enum Direction{
        Up, Down, Left, Right
    };

private:
    const int range;

    QVector<QVector<bool>> barrier;
    QQueue<RCPoint> body;
    RCPoint target;
    int digesting = 0;

    Direction direction = Right, lastDirection = Right;

    bool targetSetFlag = false;
    bool directionSetFlag = false;

public:
    explicit Playground(int range_);
    ~Playground();

    void clear();
    void changeBarrier(const RCPoint& point);
    void setTarget();
    bool getNextFrame();
    void changeDirection(Direction direction);

    bool pointInRange(const RCPoint& point) const;

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;

    QVector<QVector<PointState>> toPlate() const;

    bool targetSet() const;
    bool directionSet() const;

private:
    QString barrierToString() const;
    void barrierFromString(const QString& str);
};

#endif // PLAYGROUND_H
