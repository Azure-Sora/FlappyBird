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
#include <QStringList>

GameMainWindow::GameMainWindow(QWidget *parent,QWidget *mainWindow) :
    QMainWindow(parent)
    ,mainWindow(mainWindow)
    ,ui(new Ui::GameMainWindow)
    ,bird1(new Bird)
    ,bird2(new Bird)
{
    ui->setupUi(this);

    this->setFixedSize(800,800);
    gameOver = new QLabel("游戏结束",this);
    gameOver->resize(200,200);
    gameOver->move(300,300);
    gameOver->setFont(QFont("黑体",35,QFont::Bold));
    gameOver->setVisible(false);

    ui->btnStartGame->installEventFilter(this);//已弃用

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
    gameMode = static_cast<MainWindow *>(mainWindow)->isMultiplayer == true ? multiplayer : singelplayer;

    score=0;
    pipeUp = new Pipe(0,Pipe::up,this);
    pipeDown = new Pipe(0,Pipe::down,this);

    if(gameMode == GameMainWindow::singelplayer)
    {
        createPipes();
        connect(bird1,&Bird::flyStatusChanged,bird1,&Bird::flapWing);
        bird1->birdX=400;
        bird1->birdY=400;
        bird1->speed=0;
        connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed);
        connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
    }

    if(gameMode == GameMainWindow::multiplayer)
    {
        isServer = static_cast<MainWindow *>(mainWindow)->isServer;
        connect(bird1,&Bird::flyStatusChanged,bird1,&Bird::flapWing);
        bird1->birdX=500;
        bird1->birdY=400;
        bird1->speed=0;
        connect(bird2,&Bird::flyStatusChanged,bird2,&Bird::flapWing);
        bird2->birdX=300;
        bird2->birdY=400;
        bird2->speed=0;
        if(isServer)
        {
            initServer();
        }
        else
        {
            initClient();
        }
    }


//    if(static_cast<MainWindow *>(mainWindow)->isMultiplayer == true) initClient();

    gameRunning = true;
}

void GameMainWindow::updateFrame()
{
    if(isServer)
    {
        birdMove();
        bird2Move();
        repaint();
        checkCrash();
        syncWithClient();
    }
    else
    {
        repaint();
    }
}

void GameMainWindow::paintEvent(QPaintEvent *event)
{
    QPainter birdPainter(this);
    birdPainter.translate(bird1->birdX,bird1->birdY);
    //    painter.drawEllipse(QPoint(0,0),20,20);
    switch (bird1->flyStatus) {
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
    if(gameMode == GameMainWindow::multiplayer)
    {
        QPainter bird2Painter(this);
        bird2Painter.translate(bird2->birdX,bird2->birdY);
        switch (bird2->flyStatus) {
        case 1:
            bird2Painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_blue_down.png"));
            break;
        case 2:
            bird2Painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_blue_middle.png"));
            break;
        case 3:
            bird2Painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_blue_up.png"));
            break;
        default:
            bird2Painter.drawPixmap(0,0,40,40,QPixmap(":/res/bird_blue_up.png"));
            break;
        }
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
        bird1->fly();
    }

}

void GameMainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        bird1->fly();
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
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);

}

void GameMainWindow::initClient()
{
//    qDebug() << "initclient";
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
//    socket = static_cast<MainWindow *>(mainWindow)->socket;
    connect(myMain->socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf = myMain->socket->readAll();
        QString bufStr;
        bufStr.prepend(buf);
        qDebug() << bufStr;
        if(bufStr == "fly")
        {
            bird2->fly();
            return;
        }
        //bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeDownX-Score-gameRunning
        QStringList data = bufStr.split("-");
        syncWithServer(data);
    });


}

void GameMainWindow::birdMove()
{
    if(bird1->birdY >= 30)
    {
        bird1->speed += gravity;
        bird1->birdY += bird1->speed;
    }
    else
    {
        bird1->birdY = 30;
        bird1->speed = 1;
        bird1->birdY += bird1->speed;
    }
}

void GameMainWindow::checkCrash()
{
    pipeUp->isCrashed(bird1);
    pipeDown->isCrashed(bird1);
    if(gameMode == GameMainWindow::multiplayer)
    {
        pipeUp->isCrashed(bird2);
        pipeDown->isCrashed(bird2);
    }
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

void GameMainWindow::syncWithServer(QStringList data)
{
    //bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeUpY-pipeDownX-pipeDownY-Score-gameRunning
    bird1->birdY=data.at(0).toInt();
    bird2->birdY=data.at(1).toInt();
    bird1->flyStatus=data.at(2).toInt();
    bird2->flyStatus=data.at(3).toInt();
    pipeUp->x=data.at(4).toInt();
    pipeUp->y=data.at(5).toInt();
    pipeDown->x=data.at(6).toInt();
    pipeDown->y=data.at(7).toInt();
    score=data.at(8).toInt();
    gameRunning = (data.at(9).toInt() == 1 ? true : false);
}

void GameMainWindow::syncWithClient()
{
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
    QStringList data;
    data << QString::number(bird1->birdY) << "-" << QString::number(bird2->birdY) << "-"
         << QString::number(bird1->flyStatus) << "-" << QString::number(bird2->flyStatus) << "-"
         << QString::number(pipeUp->x) << "-" << QString::number(pipeUp->y) << "-"
         << QString::number(pipeDown->x) << "-" << QString::number(pipeDown->y) << "-"
         << QString::number(score) << "-" << (gameRunning == true ? "1" : "0");
    QString tmpstr = data.join("");
    QByteArray tmpbytearr = tmpstr.toLocal8Bit();
    myMain->client->write(tmpbytearr);
}

void GameMainWindow::bird2Move()
{
    if(bird2->birdY >= 30)
    {
        bird2->speed += gravity;
        bird2->birdY += bird2->speed;
    }
    else
    {
        bird2->birdY = 30;
        bird2->speed = 1;
        bird2->birdY += bird2->speed;
    }
}

