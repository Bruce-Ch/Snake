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
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include <cassert>
#include <cstdlib>
#include <ctime>

#include "playground.h"
#include "rcpoint.h"
#include "pointconverter.h"

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
    void keyPressEvent(QKeyEvent* event) override;

    QPoint xy2rxry(QPoint xy) const;

    void write(QJsonObject& json) const;
    void read(const QJsonObject& json);

private slots:
    void yetToStartInit();
    void playingInit();
    void interruptInit();
    void stopInit();
    void yetToStartLeave();
    void playingLeave();

    void getNextFrame();

    void saveGame() const;
    void loadGame();

signals:
    void gameover();
    void loadGameSuceed();

private:
    Ui::Snake *ui;

    const static int left = 50, top = 80, size = 13;
    const static int range = 40;
    const static int standardPixel = 600;

    Playground playground;

    enum StateType{
        YetToStart, Playing, Interrupt, Stop
    } stateIdx = YetToStart;

    RCPoint hover;

    int time = 0;

    QTimer* timer;
};
#endif // SNAKE_H
