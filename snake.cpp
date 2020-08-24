#include "snake.h"
#include "ui_snake.h"

Snake::Snake(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Snake)
{
    ui->setupUi(this);
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
