#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>
#include <QState>
#include <QFinalState>
#include <QStateMachine>

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

private:
    Ui::Snake *ui;
};
#endif // SNAKE_H
