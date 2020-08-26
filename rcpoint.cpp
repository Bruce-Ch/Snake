#include "rcpoint.h"

RCPoint::RCPoint(){}

RCPoint::RCPoint(int row, int col): row_(row), col_(col){}

RCPoint::~RCPoint(){}

int RCPoint::row() const{
    return row_;
}

int RCPoint::col() const{
    return col_;
}

void RCPoint::read(const QJsonArray &json){
    row_ = json[0].toInt();
    col_ = json[1].toInt();
}

void RCPoint::write(QJsonArray &json) const {
    json.append(row_);
    json.append(col_);
}

bool RCPoint::operator==(const RCPoint &other) const {
    return row_ == other.row_ && col_ == other.col_;
}
