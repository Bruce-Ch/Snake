#include "snake.h"
#include "ui_snake.h"

Snake::Snake(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Snake), playground(42, QVector<bool>(42, false))
{
    ui->setupUi(this);
    QTimer * timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){ time++; ui->timeLCDNumber->display(time);});
    connect(ui->pauseTimeButton, &QPushButton::clicked, timer, &QTimer::stop);
    connect(ui->continueTimeButton, SIGNAL(clicked()), timer, SLOT(start()));
    timer->setInterval(1000);
    timer->start();
    setStateMachine();
}

Snake::~Snake()
{
    delete ui;
}

void Snake::setStateMachine(){
    QStateMachine* stateMachine = new QStateMachine(this);
    QState* game = new QState();
    QFinalState* finish = new QFinalState();
    QState* yetToStart = new QState(game);
    QState* playing = new QState(game);
    QState* stop = new QState(game);
    QState* interrupt = new QState(game);
    game->setInitialState(yetToStart);
    stateMachine->addState(game);
    stateMachine->addState(finish);
    stateMachine->setInitialState(game);

    yetToStartSetState(yetToStart);
    playingSetState(playing);
    interruptSetState(interrupt);
    stopSetState(stop);

    yetToStart->addTransition(ui->actionStart, &QAction::triggered, playing);
    yetToStart->addTransition(ui->actionLoad, &QAction::triggered, interrupt);
    playing->addTransition(ui->actionPause, &QAction::triggered, interrupt);
    interrupt->addTransition(ui->actionContinue, &QAction::triggered, playing);
    interrupt->addTransition(ui->actionRestart, &QAction::triggered, yetToStart);
    interrupt->addTransition(ui->actionSave, &QAction::triggered, interrupt);
    stop->addTransition(ui->actionRestart, &QAction::triggered, yetToStart);
    game->addTransition(ui->actionQuit, &QAction::triggered, finish);

    connect(ui->startButton, &QPushButton::clicked, ui->actionStart, &QAction::triggered);
    connect(ui->pauseButton, &QPushButton::clicked, ui->actionPause, &QAction::triggered);
    connect(ui->restartButton, &QPushButton::clicked, ui->actionRestart, &QAction::triggered);
    connect(ui->saveButton, &QPushButton::clicked, ui->actionSave, &QAction::triggered);
    connect(ui->loadButton, &QPushButton::clicked, ui->actionLoad, &QAction::triggered);
    connect(ui->continueButton, &QPushButton::clicked, ui->actionContinue, &QAction::triggered);
    connect(ui->quitButton, &QPushButton::clicked, ui->actionQuit, &QAction::triggered);
    connect(stateMachine, &QStateMachine::finished, QCoreApplication::instance(), &QCoreApplication::quit);

    connect(yetToStart, &QState::entered, this, &Snake::yetToStartInit);
    connect(playing, &QState::entered, this, &Snake::playingInit);
    connect(interrupt, &QState::entered, this, &Snake::interruptInit);
    connect(stop, &QState::entered, this, &Snake::stopInit);
    connect(yetToStart, &QState::exited, this, &Snake::yetToStartLeave);

    stateMachine->start();
}

void Snake::yetToStartSetState(QState *yetToStart){
    yetToStart->assignProperty(ui->label, "text", "In state yetToStart");

    yetToStart->assignProperty(ui->actionPause, "enabled", false);
    yetToStart->assignProperty(ui->actionContinue, "enabled", false);
    yetToStart->assignProperty(ui->actionSave, "enabled", false);
    yetToStart->assignProperty(ui->actionRestart, "enabled", false);
    yetToStart->assignProperty(ui->actionStart, "enabled", true);
    yetToStart->assignProperty(ui->actionQuit, "enabled", true);
    yetToStart->assignProperty(ui->actionLoad, "enabled", true);

    yetToStart->assignProperty(ui->pauseButton, "enabled", false);
    yetToStart->assignProperty(ui->continueButton, "enabled", false);
    yetToStart->assignProperty(ui->saveButton, "enabled", false);
    yetToStart->assignProperty(ui->restartButton, "enabled", false);
    yetToStart->assignProperty(ui->startButton, "enabled", true);
    yetToStart->assignProperty(ui->quitButton, "enabled", true);
    yetToStart->assignProperty(ui->loadButton, "enabled", true);
}

void Snake::yetToStartInit(){
    for(int i = 0; i < 42; i ++){
        for(int j = 0; j < 42; j ++){
            if(i == 0 || i == 41 || j == 0 || j == 41){
                playground[i][j] = true;
            } else {
                playground[i][j] = false;
            }
        }
    }

    body.clear();
    body.push_back(QPoint(20, 21));
    body.push_back(QPoint(20, 20));

    target = QPoint(-1, -1);
    hover = QPoint(-1, -1);

    stateIdx = 1;

    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);

    update();
}

void Snake::yetToStartLeave(){
    setMouseTracking(false);
    ui->centralwidget->setMouseTracking(false);
    hover = QPoint(-1, -1);
}

void Snake::playingSetState(QState *playing){
    playing->assignProperty(ui->label, "text", "In state playing");

    playing->assignProperty(ui->actionStart, "enabled", false);
    playing->assignProperty(ui->actionLoad, "enabled", false);
    playing->assignProperty(ui->actionContinue, "enabled", false);
    playing->assignProperty(ui->actionRestart, "enabled", false);
    playing->assignProperty(ui->actionSave, "enabled", false);
    playing->assignProperty(ui->actionQuit, "enabled", true);
    playing->assignProperty(ui->actionPause, "enabled", true);

    playing->assignProperty(ui->startButton, "enabled", false);
    playing->assignProperty(ui->loadButton, "enabled", false);
    playing->assignProperty(ui->continueButton, "enabled", false);
    playing->assignProperty(ui->restartButton, "enabled", false);
    playing->assignProperty(ui->saveButton, "enabled", false);
    playing->assignProperty(ui->quitButton, "enabled", true);
    playing->assignProperty(ui->pauseButton, "enabled", true);
}

void Snake::playingInit(){
    stateIdx = 2;
}

void Snake::interruptSetState(QState *interrupt){
    interrupt->assignProperty(ui->label, "text", "In state interrupt");

    interrupt->assignProperty(ui->actionStart, "enabled", false);
    interrupt->assignProperty(ui->actionPause, "enabled", false);
    interrupt->assignProperty(ui->actionLoad, "enabled", false);
    interrupt->assignProperty(ui->actionQuit, "enabled", true);
    interrupt->assignProperty(ui->actionContinue, "enabled", true);
    interrupt->assignProperty(ui->actionRestart, "enabled", true);
    interrupt->assignProperty(ui->actionSave, "enabled", true);

    interrupt->assignProperty(ui->startButton, "enabled", false);
    interrupt->assignProperty(ui->pauseButton, "enabled", false);
    interrupt->assignProperty(ui->loadButton, "enabled", false);
    interrupt->assignProperty(ui->quitButton, "enabled", true);
    interrupt->assignProperty(ui->continueButton, "enabled", true);
    interrupt->assignProperty(ui->restartButton, "enabled", true);
    interrupt->assignProperty(ui->saveButton, "enabled", true);
}

void Snake::interruptInit(){
    stateIdx = 3;
}

void Snake::stopSetState(QState *stop){
    stop->assignProperty(ui->label, "text", "In state stop");

    stop->assignProperty(ui->actionStart, "enabled", false);
    stop->assignProperty(ui->actionPause, "enabled", false);
    stop->assignProperty(ui->actionContinue, "enabled", false);
    stop->assignProperty(ui->actionLoad, "enabled", false);
    stop->assignProperty(ui->actionSave, "enabled", false);
    stop->assignProperty(ui->actionQuit, "enabled", true);
    stop->assignProperty(ui->actionRestart, "enabled", true);

    stop->assignProperty(ui->startButton, "enabled", false);
    stop->assignProperty(ui->pauseButton, "enabled", false);
    stop->assignProperty(ui->continueButton, "enabled", false);
    stop->assignProperty(ui->loadButton, "enabled", false);
    stop->assignProperty(ui->saveButton, "enabled", false);
    stop->assignProperty(ui->quitButton, "enabled", true);
    stop->assignProperty(ui->restartButton, "enabled", true);
}

void Snake::stopInit(){
    stateIdx = 4;
}

void Snake::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QVector<QVector<int>> plate(42, QVector<int>(42, 0));
    for(int i = 0; i < 42; i ++){
        for(int j = 0; j < 42; j ++){
            plate[i][j] = playground[i][j];
        }
    }
    if(target.x() >= 1 && target.x() <= 40 && target.y() >= 1 && target.y() <= 40){
        plate[target.x()][target.y()] = 2;
    }
    for(auto point: body){
        plate[point.x()][point.y()] = 3;
    }
    if(hover.x() >= 1 && hover.x() <= 40 && hover.y() >= 1 && hover.y() <= 40){
        plate[hover.x()][hover.y()] = 4;
    }

    for(int i = 0; i < 42; i ++){
        for(int j = 0; j < 42; j++){
            switch (plate[i][j]) {
            case 0:
                painter.setBrush(QColor("white"));
                painter.setPen(QColor("green"));
                break;
            case 1:
                painter.setBrush(QColor("black"));
                painter.setPen(QColor("green"));
                break;
            case 2:
                painter.setBrush(QColor("red"));
                painter.setPen(QColor("green"));
                break;
            case 3:
                painter.setBrush(QColor("blue"));
                painter.setPen(QColor("green"));
                break;
            case 4:
                painter.setBrush(QColor("purple"));
                painter.setPen(QColor("green"));
                break;
            default:
                assert(0 && plate[i][j]);
            }
            QPoint xy = rc2xy(QPoint(i, j));
            painter.drawRect(xy.x(), xy.y(), 10, 10);
        }
    }
    painter.end();
}

void Snake::mouseMoveEvent(QMouseEvent *event){
    if(stateIdx != 1){
        return;
    }
    int x = event->x(), y = event->y();
    hover = xy2rc(QPoint(x, y));
    update();
}

void Snake::mouseReleaseEvent(QMouseEvent *){
    if(stateIdx != 1){
        return;
    }
    if(hover.x() >= 1 && hover.x() <= 40 && hover.y() >= 1 && hover.y() <= 40){
        playground[hover.x()][hover.y()] = !playground[hover.x()][hover.y()];
    }
    update();
}

QPoint Snake::rc2xy(QPoint rc){
    int y = 50 + rc.x() * 10;
    int x = 50 + rc.y() * 10;
    return QPoint(x, y);
}

QPoint Snake::xy2rc(QPoint xy){
    int row = (xy.y() - 50) / 10;
    int col = (xy.x() - 50) / 10;
    return QPoint(row, col);
}
