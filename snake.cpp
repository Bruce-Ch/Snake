#include "snake.h"
#include "ui_snake.h"

Snake::Snake(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Snake), playground(range)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Snake::getNextFrame);
    timer->setInterval(200);

    std::srand(std::time(nullptr));

    PointConverter::setTop(top);
    PointConverter::setLeft(left);
    PointConverter::setSize(size);

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
    yetToStart->addTransition(this, &Snake::loadGameSuceed, interrupt);
    //yetToStart->addTransition(ui->actionLoad, &QAction::triggered, interrupt);
    playing->addTransition(ui->actionPause, &QAction::triggered, interrupt);
    playing->addTransition(this, &Snake::gameover, stop);
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
    connect(playing, &QState::exited, this, &Snake::playingLeave);

    connect(ui->actionLoad, &QAction::triggered, this, &Snake::loadGame);
    connect(ui->actionSave, &QAction::triggered, this, &Snake::saveGame);

    stateMachine->start();
}

void Snake::yetToStartSetState(QState *yetToStart){
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
    playground.clear();

    hover = RCPoint(-1, -1);
    time = 0;

    stateIdx = YetToStart;

    setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);

    update();
}

void Snake::yetToStartLeave(){
    setMouseTracking(false);
    ui->centralwidget->setMouseTracking(false);
    hover = RCPoint(-1, -1);
}

void Snake::playingSetState(QState *playing){
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
    stateIdx = Playing;
    if(!playground.targetSet()){
        playground.setTarget();
    }
    timer->start();
}

void Snake::playingLeave(){
    timer->stop();
}

void Snake::interruptSetState(QState *interrupt){
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
    stateIdx = Interrupt;
}

void Snake::stopSetState(QState *stop){
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
    stateIdx = Stop;
    QMessageBox message;
    message.setText("You lose! Your score is " + QString::number(time) + ".");
    message.setWindowTitle("You lose!");
    message.exec();
}

void Snake::getNextFrame(){
    time++;
    if(!playground.getNextFrame()){
        emit gameover();
    }
    update();
}

void Snake::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF target(0, 0, width(), height());
    QRectF source(0.0, 0.0, 800, 481);
    QPixmap pixmap(":/images/background.jpg");
    painter.drawPixmap(target, pixmap, source);

    int left, top, right, bottom;
    ui->centralwidget->getContentsMargins(&left, &top, &right, &bottom);
    double side = qMin<double>((ui->centralwidget->width() - left - right) * 0.75, ui->centralwidget->height() - top - bottom);
    painter.scale(side / standardPixel, side / standardPixel);
    QVector<QVector<Playground::PointState>> plate = playground.toPlate();

    if(stateIdx == YetToStart){
        painter.setPen(QColor("darkkhaki"));
    } else {
        painter.setPen(QColor("lightyellow"));
    }

    for(int i = 0; i < range + 2; i ++){
        for(int j = 0; j < range + 2; j++){
            switch (plate[i][j]) {
            case Playground::Empty:
                painter.setBrush(QColor("bisque"));
                break;
            case Playground::Barrier:
                painter.setBrush(QColor("black"));
                break;
            case Playground::Target:
                painter.setBrush(QColor("red"));
                break;
            case Playground::Body:
                painter.setBrush(QColor("blue"));
                break;
            }
            QPoint xy = PointConverter::rc2xy(RCPoint(i, j));
            painter.drawRect(xy.x(), xy.y(), size, size);
        }
    }

    if(stateIdx == YetToStart && playground.pointInRange(hover)){
        painter.setBrush(QColor("purple"));
        QPoint xy = PointConverter::rc2xy(hover);
        painter.drawRect(xy.x(), xy.y(), size, size);
    }

    painter.end();
    ui->timeLCDNumber->display(time);
}

void Snake::mouseMoveEvent(QMouseEvent *event){
    if(stateIdx != YetToStart){
        return;
    }
    int x = event->x(), y = event->y();
    hover = PointConverter::xy2rc(xy2rxry(QPoint(x, y)));
    update();
}

void Snake::mouseReleaseEvent(QMouseEvent *){
    if(stateIdx != YetToStart){
        return;
    }
    playground.changeBarrier(hover);
    update();
}

void Snake::keyPressEvent(QKeyEvent *event){
    if(stateIdx != Playing){
        QMainWindow::keyPressEvent(event);
        return;
    }
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_W:
        playground.changeDirection(Playground::Up);
        break;
    case Qt::Key_Down:
    case Qt::Key_S:
        playground.changeDirection(Playground::Down);
        break;
    case Qt::Key_Left:
    case Qt::Key_A:
        playground.changeDirection(Playground::Left);
        break;
    case Qt::Key_Right:
    case Qt::Key_D:
        playground.changeDirection(Playground::Right);
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

QPoint Snake::xy2rxry(QPoint xy) const {
    int left, top, right, bottom;
    ui->centralwidget->getContentsMargins(&left, &top, &right, &bottom);
    double side = qMin<double>((ui->centralwidget->width() - left - right) * 0.75, ui->centralwidget->height() - top - bottom);
    int rx = xy.x() / side * standardPixel;
    int ry = xy.y() / side * standardPixel;
    return QPoint(rx, ry);
}

void Snake::write(QJsonObject &json) const {
    QJsonObject playgroundJsonObject;
    playground.write(playgroundJsonObject);
    json["playground"] = playgroundJsonObject;
    json["time"] = time;
}

void Snake::read(const QJsonObject &json){
    if(json.contains("playground") && json["playground"].isObject()){
        playground.read(json["playground"].toObject());
    }
    if(json.contains("time") && json["time"].isDouble()){
        time = json["time"].toInt();
    }
}

void Snake::saveGame() const {
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save Files",
                              QApplication::instance()->applicationDirPath() + "../../../", "JSON data (*.json)");
    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return;
    }

    QJsonObject gameObject;
    write(gameObject);
    saveFile.write(QJsonDocument(gameObject).toJson());
}

void Snake::loadGame(){
    QString filename = QFileDialog::getOpenFileName(this, "Load Files",
                              QApplication::instance()->applicationDirPath() + "../../../", "JSON data (*.json)");
    QFile loadFile(filename);
    if (!loadFile.exists() || !loadFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    read(loadDoc.object());
    emit loadGameSuceed();
    update();
}

