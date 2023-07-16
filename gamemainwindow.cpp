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
#include "ground.h"
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>

GameMainWindow::GameMainWindow(QWidget *parent,QWidget *mainWindow) :
    QMainWindow(parent)
    ,mainWindow(mainWindow)
    ,ui(new Ui::GameMainWindow)
    ,bird1(new Bird)
    ,bird2(new Bird)
    ,background(new Background)
    ,ground(new Ground(0))
    ,bkgdMusic(new QSoundEffect)
    ,gameScene(day)
    ,gameTimer(new QTimer)
    ,server(nullptr)
    ,socket(nullptr)
{
    ui->setupUi(this);
    this->setWindowTitle("Flappy Bird");

    this->setFixedSize(800,800);//初始化游戏结束的字
    gameOver = new QLabel("游戏结束",this);
    gameOver->resize(200,200);
    gameOver->move(300,300);
    gameOver->setFont(QFont("黑体",35,QFont::Bold));
    gameOver->setVisible(false);

    gameTime = 0;
    QTimer *timer = new QTimer;
    connect(ui->actionExit, &QAction::triggered, [=](){
        this->close();
    });
    connect(ui->btnStartGame,&QPushButton::clicked,[=](){
        initGame();
        timer->start(25);
        ui->btnStartGame->setDisabled(true);
        ui->actionStart->setDisabled(true);
    });
    connect(ui->actionStart,&QAction::triggered,[=](){
        initGame();
        timer->start(25);
        gameTimer->start(1000);
        ui->btnStartGame->setDisabled(true);
        ui->actionStart->setDisabled(true);
    });

    connect(timer,&QTimer::timeout,this,[=](){
        if(gameRunning == false) //若游戏结束，开始结束收尾
        {
            timer->stop();
            gameTimer->stop();
            bkgdMusic->stop();
            pipeUp->moveTimer->stop();
            pipeUp->stepMoveTimer->stop();
            pipeUp->upAndDownMoveTimer->stop();
            pipeDown->moveTimer->stop();
            pipeDown->stepMoveTimer->stop();
            pipeDown->upAndDownMoveTimer->stop();

            delete timer;
            delete gameTimer;


            crashed(); //用于在2P也能显示出游戏结束，但会导致播放两次die音效，暂时想不到更好的实现方案
        }
        updateFrame();
    });

    connect(gameTimer, &QTimer::timeout ,[=](){
        gameTime++;
        if(gameTime == 24) //游戏运行24后卡点音乐进入feverTime
        {
            feverTime();
        }
//        qDebug () << QString::number(gameTime);
    });

}

GameMainWindow::~GameMainWindow()
{
    delete ui;
}

void GameMainWindow::initGame()
{
    this->setFixedSize(800,800);
    //初始化各种属性
    gameMode = static_cast<MainWindow *>(mainWindow)->isMultiplayer == true ? multiplayer : singelplayer;
    difficulty = static_cast<MainWindow *>(mainWindow)->difficulty + 1;
    ground->difficulty=difficulty;
    score=0;
    pipeUp = new Pipe(0,Pipe::up,this,false);
    pipeDown = new Pipe(0,Pipe::down,this,false);

    initMusic();

    if(gameMode == GameMainWindow::singelplayer) //单人游戏
    {
        isServer = true;
        pipeUp->isActive = true;
        pipeDown->isActive = true;
        createPipes();
        connect(bird1,&Bird::flyStatusChanged,bird1,&Bird::flapWing); //让鸟飞的时候扇翅膀
        bird1->birdX=400;
        bird1->birdY=400;
        bird1->speed=0;
        connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed); //初始化碰撞检测
        connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
        connect(ground,&Ground::hitGround,this,&GameMainWindow::crashed);
    }

    if(gameMode == GameMainWindow::multiplayer) //多人游戏
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
        if(isServer) //主机
        {
            initServer();
            pipeUp->isActive = true;
            pipeDown->isActive = true;
            createPipes();
            connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed);
            connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
            connect(ground,&Ground::hitGround,this,&GameMainWindow::crashed);
        }
        else //2P
        {
            initClient();
        }
    }

    gameRunning = true;
}

void GameMainWindow::updateFrame() //每帧更新画面
{
//    qDebug() << gameRunning;
    if(gameMode == GameMainWindow::singelplayer)
    {
        birdMove();
        repaint();
        checkCrash();
        return;
    }
    if(isServer)
    {
        birdMove();
        bird2Move();
        repaint();
        checkCrash();
        syncWithClient(); //每帧都与2P同步
    }
    else
    {
        repaint(); //2P只负责绘制画面，不参与逻辑运算
    }
}

void GameMainWindow::paintEvent(QPaintEvent *event) //从底层到顶层逐层绘制
{
    QPainter backgroundPainter(this);
    backgroundPainter.translate(background->x, 0);
    if(gameScene == GameMainWindow::day)
    {
        backgroundPainter.drawPixmap(0,0,1800,800,QPixmap(":/res/background_day.png"));
    }
    else if(gameScene == GameMainWindow::night)
    {
        backgroundPainter.drawPixmap(0,0,1800,800,QPixmap(":/res/background_night.png"));
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

    QPainter groundPainter(this);
    groundPainter.translate(ground->x, 700);
    groundPainter.drawPixmap(0,0,1600,100,QPixmap(":/res/ground.png"));


    QPainter birdPainter(this);
    birdPainter.translate(bird1->birdX,bird1->birdY);
    //    painter.drawEllipse(QPoint(0,0),20,20);
    switch (bird1->flyStatus) { //实现鸟扇翅膀
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

}

void GameMainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
//        bird1->fly();
        if(gameMode == GameMainWindow::singelplayer || isServer == true)
        {
            bird1->fly();
        }
        if(gameMode == GameMainWindow::multiplayer && isServer == false)
        {
            MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
            myMain->socket->write("fly"); //多人且不是主机的情况下，向主机发送fly请求
        }
    }

}

void GameMainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(gameMode == GameMainWindow::singelplayer || isServer == true)
        {
            bird1->fly();
        }
        if(gameMode == GameMainWindow::multiplayer && isServer == false)
        {
            MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
            myMain->socket->write("fly");
        }
    }
}

void GameMainWindow::closeEvent(QCloseEvent *event)
{
    emit closed(); //用于关闭游戏窗口后显示菜单窗口
}

void GameMainWindow::createPipes()
{
    //通过水管中间洞的大小和位置确定两个管子的位置
    int holeWidth = QRandomGenerator::global()->bounded(150,300);
    int holeCenter = QRandomGenerator::global()->bounded(200,600);
    pipeUp->initPosition(holeWidth,holeCenter);
    pipeDown->initPosition(holeWidth,holeCenter);
    connect(pipeUp, &Pipe::resetMe, this, &GameMainWindow::resetPipes);
    connect(pipeUp, &Pipe::getScore, [=](){
        score++;
        scoreChanged();
    });
}

void GameMainWindow::initServer()
{
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
    //监听2P消息，接受到fly请求时让2P鸟飞
    connect(myMain->client,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf = myMain->client->readAll();
        QString bufStr;
        bufStr.prepend(buf);
//        qDebug() << bufStr;
        if(bufStr == "fly")
        {
            bird2->fly();
        }
    });

}

void GameMainWindow::initClient()
{
//    qDebug() << "initclient";
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
    //监听主机消息，接收到主机信息时进行处理并同步
    connect(myMain->socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf = myMain->socket->readAll();
        QString bufStr;
        bufStr.prepend(buf);
//        qDebug() << bufStr;
//        if(bufStr == "fly")
//        {
//            bird2->fly();
//            return;
//        }
        //bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeDownX-Score-gameRunning-holePosition
        QStringList data = bufStr.split("~");
        syncWithServer(data);
    });


}

void GameMainWindow::birdMove()//1P鸟每帧的移动
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

void GameMainWindow::checkCrash() //每帧检测碰撞
{
    pipeUp->isCrashed(bird1);
    pipeDown->isCrashed(bird1);
    ground->checkHitGround(bird1);
    if(gameMode == GameMainWindow::multiplayer)
    {
        pipeUp->isCrashed(bird2);
        pipeDown->isCrashed(bird2);
        ground->checkHitGround(bird2);
    }
}

void GameMainWindow::crashed() //发生碰撞
{
    gameRunning = false;
    QSoundEffect *dieSound = new QSoundEffect;
    dieSound->setSource(QUrl::fromLocalFile(":/res/die.wav"));
    dieSound->setLoopCount(1);
    dieSound->setVolume(0.5f);
    dieSound->play();
    connect(dieSound, &QSoundEffect::playingChanged, [=](){
        if(dieSound->isPlaying())
        {
            dieSound->deleteLater();
        }
    });
    gameOver->show();
}

void GameMainWindow::resetPipes() //水管撞到左边边界后重置水管
{
    int holeWidth = QRandomGenerator::global()->bounded(150,300);
    int holeCenter = QRandomGenerator::global()->bounded(200,600);
    pipeUp->reset(holeWidth,holeCenter);
    pipeDown->reset(holeWidth,holeCenter);
}

void GameMainWindow::syncWithServer(QStringList data) //客户机处理主机的同步消息
{
    //bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeUpHeight-pipeDownX-pipeDownY-Score-gameRunning-holePosition-difficulty
    bird1->birdY=data.at(0).toInt();
    bird2->birdY=data.at(1).toInt();
    bird1->flyStatus=data.at(2).toInt();
    bird2->flyStatus=data.at(3).toInt();
    pipeUp->x=data.at(4).toInt();
    pipeUp->height=data.at(5).toInt();
    pipeDown->x=data.at(6).toInt();
    pipeDown->y=data.at(7).toInt();
    if(score != data.at(8).toInt())
    {
        score=data.at(8).toInt();
        scoreChanged();
    }
    gameRunning = (data.at(9).toInt() == 1 ? true : false);
    int holePosition = data.at(10).toInt();
    difficulty = data.at(11).toInt();
    pipeDown->caculatePosition(holePosition,pipeUp);
    pipeUp->caculatePosition(holePosition,pipeDown);
}

void GameMainWindow::syncWithClient() //主机打包消息并向客户机发送
{
    static MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
    QStringList data;
    data << QString::number(bird1->birdY) << "~" << QString::number(bird2->birdY) << "~"
         << QString::number(bird1->flyStatus) << "~" << QString::number(bird2->flyStatus) << "~"
         << QString::number(pipeUp->x) << "~" << QString::number(pipeUp->height) << "~"
         << QString::number(pipeDown->x) << "~" << QString::number(pipeDown->y) << "~"
         << QString::number(score) << "~" << (gameRunning == true ? "1" : "0") << "~"
         << QString::number((pipeDown->y - pipeUp->height)/2 + pipeUp->height) << "~" << QString::number(difficulty);
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

void GameMainWindow::scoreChanged() //分数改变时刷新分数显示并播放音效
{
    QSoundEffect *scoreSound = new QSoundEffect;
    scoreSound->setSource(QUrl::fromLocalFile(":/res/point.wav"));
    scoreSound->setLoopCount(1);
    scoreSound->setVolume(0.5f);
    scoreSound->play();
    connect(scoreSound, &QSoundEffect::playingChanged, [=](){
        if(scoreSound->isPlaying())
        {
            scoreSound->deleteLater();
        }
    });
    ui->scoreLabel->setText(QString::number(score));
}

void GameMainWindow::initMusic() //初始化背景音乐
{
    bkgdMusic->setSource(QUrl::fromLocalFile(":/res/Shooting_Stars.wav"));
    bkgdMusic->setLoopCount(10);
    bkgdMusic->setVolume(0.6);
    bkgdMusic->play();
    gameTimer->start(1000); //初始化音乐的时候再开始计时，以卡点
//    connect(bkgdMusic, &QSoundEffect::playingChanged, [=](){
//        if(bkgdMusic->isPlaying())
//        {
//            bkgdMusic->deleteLater();
//        }
//    });
}

void GameMainWindow::feverTime() //进入feverTime，背景切换到夜晚，难度+1，并且开始水管移动
{
    gameScene = night;
    if(gameMode == GameMainWindow::singelplayer || isServer)
    {
        pipeUp->startUpAndDown();
        pipeDown->startUpAndDown();
        if(difficulty < 3)
        {
            difficulty++;
            ground->difficulty=difficulty;
        }
    }
}

