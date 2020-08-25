#include "snake.h"
#include "ui_snake.h"

Snake::Snake(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Snake), playground(42, QVector<bool>(42, false))
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Snake::getNextFrame);
    timer->setInterval(200);

    std::srand(std::time(nullptr));

    setStateMachine();
}

Snake::~Snake()
{
    delete ui;
}

const Snake::SaveFormat Snake::saveFormat = Snake::Json;

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
    body.push_back(QPoint(20, 20));
    body.push_back(QPoint(20, 21));

    target = QPoint(-1, -1);
    hover = QPoint(-1, -1);
    digesting = 0;
    time = 0;
    direction = Right;

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
    if(target.x() == -1 && target.y() == -1){
        setTarget();
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
    stateIdx = 3;
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
    stateIdx = 4;
    QMessageBox message;
    message.setText("You lose! Your score is " + QString::number(time) + ".");
    message.setWindowTitle("You lose!");
    message.exec();
}

void Snake::getNextFrame(){
    time++;

    QVector<QVector<int>> plate = getPlate();
    QPoint nextPoint;
    QPoint head = body.back();
    switch (direction) {
    case Up:
        nextPoint = QPoint(head.x() - 1, head.y());
        break;
    case Down:
        nextPoint = QPoint(head.x() + 1, head.y());
        break;
    case Left:
        nextPoint = QPoint(head.x(), head.y() - 1);
        break;
    case Right:
        nextPoint = QPoint(head.x(), head.y() + 1);
        break;
    }
    if(plate[nextPoint.x()][nextPoint.y()] == 1 || plate[nextPoint.x()][nextPoint.y()] == 3){
        emit gameover();
        return;
    }

    if(digesting > 0){
        digesting--;
    } else {
        body.pop_front();
    }

    body.push_back(nextPoint);
    if(nextPoint.x() == target.x() && nextPoint.y() == target.y()){
        digesting += 3;
        setTarget();
    }

    update();
}

QVector<QVector<int>> Snake::getPlate(){
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
    return plate;
}

void Snake::paintEvent(QPaintEvent *){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int left, top, right, bottom;
    ui->centralwidget->getContentsMargins(&left, &top, &right, &bottom);
    double side = qMin<double>((ui->centralwidget->width() - left - right) * 0.75, ui->centralwidget->height() - top - bottom);
    painter.scale(side / 600, side / 600);
    QVector<QVector<int>> plate = getPlate();

    if(stateIdx == 1){
        painter.setPen(QColor("green"));
    } else {
        painter.setPen(QColor("white"));
    }

    for(int i = 0; i < 42; i ++){
        for(int j = 0; j < 42; j++){
            switch (plate[i][j]) {
            case 0:
                painter.setBrush(QColor("white"));
                break;
            case 1:
                painter.setBrush(QColor("black"));
                break;
            case 2:
                painter.setBrush(QColor("red"));
                break;
            case 3:
                painter.setBrush(QColor("blue"));
                break;
            case 4:
                painter.setBrush(QColor("purple"));
                break;
            default:
                assert(0 && plate[i][j]);
            }
            QPoint xy = rc2xy(QPoint(i, j));
            painter.drawRect(xy.x(), xy.y(), 13, 13);
        }
    }
    painter.end();
    ui->timeLCDNumber->display(time);
}

void Snake::mouseMoveEvent(QMouseEvent *event){
    if(stateIdx != 1){
        return;
    }
    int x = event->x(), y = event->y();
    hover = xy2rc(xy2rxry(QPoint(x, y)));
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

void Snake::keyPressEvent(QKeyEvent *event){
    if(stateIdx != 2){
        event->ignore();
        return;
    }
    switch (event->key()) {
    case Qt::Key_Up:
        direction = direction == Down ? Down : Up;
        break;
    case Qt::Key_Down:
        direction = direction == Up ? Up : Down ;
        break;
    case Qt::Key_Left:
        direction = direction == Right ? Right : Left;
        break;
    case Qt::Key_Right:
        direction = direction == Left ? Left : Right;
        break;
    default:
        event->ignore();
    }
}

QPoint Snake::rc2xy(QPoint rc){
    int y = 50 + rc.x() * 13;
    int x = 50 + rc.y() * 13;
    return QPoint(x, y);
}

QPoint Snake::xy2rc(QPoint xy){
    int row = (xy.y() - 50) / 13;
    int col = (xy.x() - 50) / 13;
    return QPoint(row, col);
}

QPoint Snake::xy2rxry(QPoint xy){
    int left, top, right, bottom;
    ui->centralwidget->getContentsMargins(&left, &top, &right, &bottom);
    double side = qMin<double>((ui->centralwidget->width() - left - right) * 0.75, ui->centralwidget->height() - top - bottom);
    int rx = xy.x() / side * 600;
    int ry = xy.y() / side * 600;
    return QPoint(rx, ry);
}

void Snake::setTarget(){
    int r = std::rand() % 1600;
    QVector<QVector<int>> plate = getPlate();
    for(int i = r; i < r + 1600; i++){
        int row = (r / 40) % 40 + 1;
        int col = r % 40 + 1;
        if(plate[row][col] == 0){
            target = QPoint(row, col);
            update();
            return;
        }
    }
}

QString Snake::pg2str(const QVector<QVector<bool> > &playground){
    QString ret;
    for(const auto& row: playground){
        for(bool item: row){
            ret += item ? '1' : '0';
        }
    }
    return ret;
}

QVector<QVector<bool> > Snake::str2pg(const QString &str){
    QVector<QVector<bool> > ret(42, QVector<bool>(42));
    for(int i = 0; i < 1764; i++){
        int row = i / 42;
        int col = i % 42;
        ret[row][col] = str[i] == '1' ? true : false;
    }
    return ret;
}


void Snake::write(QJsonObject &json){
    QString strPlayground = pg2str(playground);
    json["playground"] = strPlayground;
    QJsonArray bodyJsonArray;
    for(QPoint item: body){
        QJsonArray itemJsonArray;
        itemJsonArray.append(item.x());
        itemJsonArray.append(item.y());
        bodyJsonArray.append(itemJsonArray);
    }
    json["body"] = bodyJsonArray;
    json["direction"] = int(direction);
    QJsonArray targetJsonArray;
    targetJsonArray.append(target.x());
    targetJsonArray.append(target.y());
    json["target"] = targetJsonArray;
    json["digesting"] = digesting;
    json["time"] = time;
}

void Snake::read(const QJsonObject &json){
    if(json.contains("playground") && json["playground"].isString()){
        QString strPlayground = json["playground"].toString();
        playground = str2pg(strPlayground);
    }
    if(json.contains("body") && json["body"].isArray()){
        QJsonArray bodyJsonArray = json["body"].toArray();
        body.clear();
        for(int i = 0; i < bodyJsonArray.size(); i++){
            QJsonArray itemJsonArray = bodyJsonArray[i].toArray();
            body.push_back(QPoint(itemJsonArray[0].toInt(), itemJsonArray[1].toInt()));
        }
    }
    if(json.contains("direction") && json["direction"].isDouble()){
        direction = Direction(json["direction"].toInt());
    }
    if(json.contains("target") && json["target"].isArray()){
        QJsonArray targetJsonArray = json["target"].toArray();
        target = QPoint(targetJsonArray[0].toInt(), targetJsonArray[1].toInt());
    }
    if(json.contains("digesting") && json["digesting"].isDouble()){
        digesting = json["digesting"].toInt();
    }
    if(json.contains("time") && json["time"].isDouble()){
        time = json["time"].toInt();
    }
}

void Snake::saveGame(){
    QString filename = QFileDialog::getSaveFileName(this, "Save Files", QApplication::instance()->applicationDirPath() + "../../../");
    QFile saveFile(filename);
    //QFile saveFile(saveFormat == Json ? QStringLiteral("save.json") : QStringLiteral("save.dat"));
    if (!saveFile.open(QIODevice::WriteOnly)) {
        return;
    }

    QJsonObject gameObject;
    write(gameObject);
    saveFile.write(QJsonDocument(gameObject).toJson());
    //saveFile.write(saveFormat == Json ? QJsonDocument(gameObject).toJson() : QCborValue::fromJsonValue(gameObject).toCbor());
}

void Snake::loadGame(){
    QString filename = QFileDialog::getOpenFileName(this, "Load Files", QApplication::instance()->applicationDirPath() + "../../../");
    QFile loadFile(filename);
    //QFile loadFile(saveFormat == Json ? QStringLiteral("save.json") : QStringLiteral("save.dat"));
    if (!loadFile.exists() || !loadFile.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    //QJsonDocument loadDoc(saveFormat == Json ? QJsonDocument::fromJson(saveData) : QJsonDocument(QCborValue::fromCbor(saveData).toMap().toJsonObject()));

    read(loadDoc.object());
    emit loadGameSuceed();
    update();
}
