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
#include <QKeyEvent>

#include <cassert>
#include <cstdlib>
#include <ctime>

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

    QVector<QVector<int>> getPlate(); // 0空1墙2食3身4hover

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    QPoint rc2xy(QPoint rc);
    QPoint xy2rc(QPoint xy);

    void setTarget();

private slots:
    void yetToStartInit();
    void playingInit();
    void interruptInit();
    void stopInit();
    void yetToStartLeave();
    void playingLeave();

    void getNextFrame();

signals:
    void gameover();

private:
    Ui::Snake *ui;

    QVector<QVector<bool>> playground;
    QList<QPoint> body; // 头位于back
    int direction = 4; // 1上2下3左4右
    QPoint target;
    int digesting = 0;

    int time = 0;
    int stateIdx = 1;

    QPoint hover;

    QTimer* timer;
};
#endif // SNAKE_H
