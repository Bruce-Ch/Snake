#include <QStateMachine>
#include <QState>
#include <QFinalState>

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

    yetToStart->assignProperty(ui->label, "text", "In state yetToStart");
    playing->assignProperty(ui->label, "text", "In state playing");
    stop->assignProperty(ui->label, "text", "In state stop");
    interrupt->assignProperty(ui->label, "text", "In state interrupt");

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
