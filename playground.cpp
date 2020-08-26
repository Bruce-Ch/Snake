#include "playground.h"

Playground::Playground(int range_):range(range_), barrier(range + 2, QVector<bool>(range + 2, false)){
    clear();
}

Playground::~Playground(){}

void Playground::clear(){
    for(int i = 0; i < range + 2; i ++){
        for(int j = 0; j < range + 2; j ++){
            if(i == 0 || i == range + 1 || j == 0 || j == range + 1){
                barrier[i][j] = true;
            } else {
                barrier[i][j] = false;
            }
        }
    }

    body.clear();
    body.enqueue(RCPoint(20, 20));
    body.enqueue(RCPoint(20, 21));

    target = RCPoint(-1, -1);

    digesting = 0;

    direction = Right;
    lastDirection = Right;

    targetSetFlag = false;
}

void Playground::changeBarrier(const RCPoint &point){
    for(const RCPoint& item: body){
        if(point == item){
            return;
        }
    }
    if(pointInRange(point)){
        barrier[point.row()][point.col()] = !barrier[point.row()][point.col()];
    }
}

void Playground::setTarget(){
    int r = std::rand() % 1600;
    QVector<QVector<PointState>> plate = toPlate();
    for(int i = r; i < r + 1600; i++){
        int row = (i / 40) % 40 + 1;
        int col = i % 40 + 1;
        if(plate[row][col] == Empty){
            target = RCPoint(row, col);
            targetSetFlag = true;
            return;
        }
    }
}

bool Playground::getNextFrame(){
    QVector<QVector<PointState>> plate = toPlate();
    RCPoint nextPoint;
    RCPoint head = body.back();
    switch (direction) {
    case Up:
        nextPoint = RCPoint(head.row() - 1, head.col());
        break;
    case Down:
        nextPoint = RCPoint(head.row() + 1, head.col());
        break;
    case Left:
        nextPoint = RCPoint(head.row(), head.col() - 1);
        break;
    case Right:
        nextPoint = RCPoint(head.row(), head.col() + 1);
        break;
    }
    lastDirection = direction;

    if(plate[nextPoint.row()][nextPoint.col()] == Barrier || plate[nextPoint.row()][nextPoint.col()] == Body){
        return false;
    }

    if(digesting > 0){
        digesting--;
    } else {
        body.pop_front();
    }

    body.push_back(nextPoint);
    if(nextPoint == target){
        digesting += 3;
        setTarget();
    }

    return true;
}

void Playground::changeDirection(Direction direction_){
    switch (direction_) {
    case Up:
        if(lastDirection != Down){
            direction = Up;
        }
        break;
    case Down:
        if(lastDirection != Up){
            direction = Down;
        }
        break;
    case Left:
        if(lastDirection != Right){
            direction = Left;
        }
        break;
    case Right:
        if(lastDirection != Left){
            direction = Right;
        }
        break;
    }
}

bool Playground::pointInRange(const RCPoint& point) const {
    return point.row() >= 1 && point.row() <= range && point.col() >= 1 && point.col() <= range;
}

QString Playground::barrierToString() const {
    QString ret;
    for(const auto& row: barrier){
        for(bool item: row){
            ret += item ? '1' : '0';
        }
    }
    return ret;
}

void Playground::barrierFromString(const QString &str){
    for(int i = 0; i < (range + 2) * (range + 2); i++){
        int row = i / (range + 2);
        int col = i % (range + 2);
        barrier[row][col] = str[i] == '1' ? true : false;
    }
}

void Playground::read(const QJsonObject &json){
    if(json.contains("barrier") && json["barrier"].isString()){
        barrierFromString(json["barrier"].toString());
    }
    if(json.contains("body") && json["body"].isArray()){
        QJsonArray bodyJsonArray = json["body"].toArray();
        body.clear();
        for(int i = 0; i < bodyJsonArray.size(); i++){
            RCPoint item;
            item.read(bodyJsonArray[i].toArray());
            body.push_back(item);
        }
    }
    if(json.contains("direction") && json["direction"].isDouble()){
        direction = Direction(json["direction"].toInt());
    }
    if(json.contains("lastDirection") && json["lastDirection"].isDouble()){
        lastDirection = Direction(json["lastDirection"].toInt());
    }
    if(json.contains("target") && json["target"].isArray()){
        target.read(json["target"].toArray());
    }
    if(json.contains("digesting") && json["digesting"].isDouble()){
        digesting = json["digesting"].toInt();
    }
    if(json.contains("targetSetFlag") && json["targetSetFlag"].isBool()){
        targetSetFlag = json["targetSetFlag"].toBool();
    }
}

void Playground::write(QJsonObject &json) const {
    QString strBarrier = barrierToString();
    json["barrier"] = strBarrier;

    QJsonArray bodyJsonArray;
    for(const RCPoint& item: body){
        QJsonArray itemJsonArray;
        item.write(itemJsonArray);
        bodyJsonArray.append(itemJsonArray);
    }
    json["body"] = bodyJsonArray;

    json["direction"] = int(direction);
    json["lastDirection"] = int(lastDirection);

    QJsonArray targetJsonArray;
    target.write(targetJsonArray);
    json["target"] = targetJsonArray;

    json["digesting"] = digesting;
    json["targetSetFlag"] = targetSetFlag;
}

QVector<QVector<Playground::PointState>> Playground::toPlate() const {
    QVector<QVector<PointState>> plate(range + 2, QVector<PointState>(range + 2, Empty));
    for(int i = 0; i < range + 2; i ++){
        for(int j = 0; j < range + 2; j ++){
            plate[i][j] = PointState(int(barrier[i][j]));
        }
    }
    if(pointInRange(target)){
        plate[target.row()][target.col()] = Target;
    }
    for(auto point: body){
        plate[point.row()][point.col()] = Body;
    }
    return plate;
}

bool Playground::targetSet() const {
    return targetSetFlag;
}
