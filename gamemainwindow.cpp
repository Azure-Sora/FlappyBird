#include "gamemainwindow.h"
#include "ui_gamemainwindow.h"
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
#include <QLabel>
#include <QFont>
#include "mainwindow.h"

GameMainWindow::GameMainWindow(QWidget *parent,QWidget *mainWindow) :
    QMainWindow(parent)
    ,mainWindow(mainWindow)
    ,ui(new Ui::GameMainWindow)
    , bird(new Bird)
{
    ui->setupUi(this);

    this->setFixedSize(800,800);
    gameOver = new QLabel("游戏结束",this);
    gameOver->resize(200,200);
    gameOver->move(300,300);
    gameOver->setFont(QFont("黑体",35,QFont::Bold));
    gameOver->setVisible(false);

    ui->btnStartGame->installEventFilter(this);

    QTimer *timer = new QTimer;
    connect(ui->actionExit, &QAction::triggered, [=](){
        this->close();
    });
    connect(ui->btnStartGame,&QPushButton::clicked,[=](){
        initGame();
        timer->start(50);
        ui->btnStartGame->setDisabled(true);
    });
    connect(ui->actionStart,&QAction::triggered,[=](){
        initGame();
        timer->start(50);
        ui->btnStartGame->setDisabled(true);
    });
//    connect(ui->actionRestart,&QAction::triggered,[=](){
//        emit restartGame();
//        this->close();
//    });

    connect(timer,&QTimer::timeout,this,[=](){
        if(gameRunning == false) timer->stop();
        updateFrame();
    });

}

GameMainWindow::~GameMainWindow()
{
    delete ui;
    delete server;
    delete socket;
    delete pipeUp;
    delete pipeDown;
    delete gameOver;
}

void GameMainWindow::initGame()
{
    this->setFixedSize(800,800);
    connect(bird,&Bird::flyStatusChanged,bird,&Bird::flapWing);
    bird->birdX=400;
    bird->birdY=400;
    bird->speed=0;
    score=0;
    pipeUp = new Pipe(0,Pipe::up,this);
    pipeDown = new Pipe(0,Pipe::down,this);
    createPipes();
    connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed);
    connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
    if(static_cast<MainWindow *>(mainWindow)->isMultiplayer == true) initClient();

    gameRunning = true;
}

void GameMainWindow::updateFrame()
{
    birdMove();
    repaint();
    //    qDebug() << " bdy= " << bird->birdY << " pux= " << pipeUp->x << " pdx= " << pipeDown->x ;
    checkCrash();
}

void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter birdPainter(this);
    birdPainter.translate(bird->birdX,bird->birdY);
    //    painter.drawEllipse(QPoint(0,0),20,20);
    switch (bird->flyStatus) {
    case 1:
        birdPainter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_down.png"));
        break;
    case 2:
        birdPainter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_middle.png"));
        break;
    case 3:
        birdPainter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_up.png"));
        break;
    default:
        birdPainter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_yellow_up.png"));
        break;
    }

    if(gameRunning == true)
    {
        QPainter upPipePainter(this);
        upPipePainter.translate(pipeUp->x, pipeUp->y);
        //        upPipePainter.drawRect(0,0,pipeUp->width,pipeUp->height);
        upPipePainter.drawPixmap(0,0,pipeUp->width,pipeUp->height,QPixmap(":/res/pipe_up.png"));
        QPainter downPipePainter(this);
        downPipePainter.translate(pipeDown->x, pipeDown->y);
        downPipePainter.drawPixmap(0,0,pipeDown->width,pipeDown->height,QPixmap(":/res/pipe_down.png"));
        //        downPipePainter.drawRect(0,0,pipeDown->width,pipeDown->height);
    }

}

void GameMainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        bird->fly();
    }

}

void GameMainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        bird->fly();
    }
}

void GameMainWindow::closeEvent(QCloseEvent *event)
{
    emit closed();
}

void GameMainWindow::createPipes()
{
    int holeWidth = QRandomGenerator::global()->bounded(150,300);
    int holeCenter = QRandomGenerator::global()->bounded(200,600);
    pipeUp->initPosition(holeWidth,holeCenter);
    pipeDown->initPosition(holeWidth,holeCenter);
    connect(pipeUp, &Pipe::resetMe, this, &GameMainWindow::resetPipes);
    connect(pipeUp, &Pipe::getScore, [=](){-
            score++;
        ui->scoreLabel->setText(QString::number(score));
    });
}

void GameMainWindow::initServer()
{
    server = new QTcpServer(this);
}

void GameMainWindow::initClient()
{
    qDebug() << "initclient";
    socket = static_cast<MainWindow *>(mainWindow)->socket;
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

void GameMainWindow::birdMove()
{
    if(bird->birdY >= 30)
    {
        bird->speed += gravity;
        bird->birdY += bird->speed;
    }
    else
    {
        bird->birdY = 30;
        bird->speed = 1;
        bird->birdY += bird->speed;
    }
}

void GameMainWindow::checkCrash()
{
    pipeUp->isCrashed(bird);
    pipeDown->isCrashed(bird);
}

void GameMainWindow::crashed()
{
    gameRunning = false;
    gameOver->show();
}

void GameMainWindow::resetPipes()
{
    int holeWidth = QRandomGenerator::global()->bounded(150,300);
    int holeCenter = QRandomGenerator::global()->bounded(200,600);
    pipeUp->reset(holeWidth,holeCenter);
    pipeDown->reset(holeWidth,holeCenter);
}

