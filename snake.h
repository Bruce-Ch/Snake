#ifndef SNAKE_H
#define SNAKE_H

#include <QMainWindow>

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

private:
    Ui::Snake *ui;
};
#endif // SNAKE_H
