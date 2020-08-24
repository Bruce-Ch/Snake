#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QState>
#include <QFinalState>
#include <QStateMachine>
#include <QVector>
#include <QPoint>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>

#include <cassert>

QT_BEGIN_NAMESPACE
namespace Ui { class Snake; }
QT_END_NAMESPACE

class Snake : public QMainWindow
{
    Q_OBJECT

public:
    Snake(QWidget *parent = nullptr);
    ~Snake();

private:
    void setStateMachine();
    void yetToStartSetState(QState* yetToStart);
    void playingSetState(QState* playing);
    void interruptSetState(QState* interrupt);
    void stopSetState(QState* stop);

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    QPoint rc2xy(QPoint rc);
    QPoint xy2rc(QPoint xy);

private slots:
    void yetToStartInit();
    void playingInit();
    void interruptInit();
    void stopInit();
    void yetToStartLeave();

private:
    Ui::Snake *ui;

    QVector<QVector<bool>> playground;
    QList<QPoint> body;
    int direction = 4; // 1上2下3左4右
    QPoint target;

    int time = 0;
    int stateIdx = 1;

    QPoint hover;
};
#endif // SNAKE_H
