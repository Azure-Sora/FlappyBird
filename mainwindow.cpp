#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QAction>
#include <QPainter>
#include <QTimer>
#include <QPoint>
#include <QRandomGenerator>
#include "pipe.h"
#include <QPixmap>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QDebug>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bird(new Bird)
{
    ui->setupUi(this);

    this->setFixedSize(800,800);
    ui->btnStartGame->installEventFilter(this);

    QTimer *timer = new QTimer;
    connect(ui->btnStartGame,&QPushButton::clicked,[=](){
        initGame();
        timer->start(50);
        ui->btnStartGame->setDisabled(true);
    });

    connect(timer,&QTimer::timeout,this,[=](){
        if(gameRunning == false) timer->stop();
        updateFrame();
    });

    connect(ui->btnConnect,&QPushButton::clicked,this,&MainWindow::initClient);

//    QTimer *pipeGenerator = new QTimer;
//    pipeGenerator->start(1000);
//    connect(pipeGenerator,&QTimer::timeout,[=](){
//        createPipes();
//    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGame()
{
    this->setFixedSize(800,800);
    connect(ui->actionExit, &QAction::triggered, [=](){
        this->close();
    });
    connect(bird,&Bird::flyStatusChanged,bird,&Bird::flapWing);
    bird->birdX=400;
    bird->birdY=400;
    bird->speed=0;
    pipeUp = new Pipe(0,Pipe::up,this);
    pipeDown = new Pipe(0,Pipe::down,this);
    createPipes();
    gameRunning = true;
//    emit this->continueGame();
}

void MainWindow::updateFrame()
{
    bird->speed += gravity;
    bird->birdY += bird->speed;
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.translate(bird->birdX,bird->birdY);
//    painter.drawEllipse(QPoint(0,0),20,20);
    switch (bird->flyStatus) {
    case 1:
        painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_down.png"));
        break;
    case 2:
        painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_middle.png"));
        break;
    case 3:
        painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_up.png"));
        break;
    default:
        break;
    }
    if(gameRunning == true)
    {
        painter.translate(pipeUp->x, pipeUp->y);
        painter.drawRect(0,0,pipeUp->width,pipeUp->height);
        painter.translate(pipeDown->x, pipeDown->y);
        painter.drawRect(0,0,pipeDown->width,pipeDown->height);
    }

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        bird->fly();
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        bird->fly();
    }
}

void MainWindow::createPipes()
{
//    int holeWidth = QRandomGenerator::global()->bounded(200,300);
//    int holeCenter = QRandomGenerator::global()->bounded(300,600);
    int holeWidth = 200;
    int holeCenter = 400;
    pipeUp->initPosition(holeWidth,holeCenter);
    pipeDown->initPosition(holeWidth,holeCenter);
}

void MainWindow::initServer()
{
    server = new QTcpServer(this);
}

void MainWindow::initClient()
{
    socket = new QTcpSocket(this);
    QString ip = ui->ipEdit->text();
    int port = ui->portEdit->text().toInt();
    qDebug() << ip << "\n" << port;
    socket->connectToHost(ip,port);
    if(socket->waitForConnected(5000))
    {
        ui->connectionStatus->setText("已连接");
    }
    else
    {
        ui->connectionStatus->setText("连接失败");
    }
    connect(socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf = socket->readAll();
        QString bufStr;
        bufStr.prepend(buf);
        qDebug() << bufStr;
        if(bufStr == "fly")
        {
            bird->fly();
        }
    });


}


