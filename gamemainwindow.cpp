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
#include "coin.h"
#include <QIcon>

GameMainWindow::GameMainWindow(QWidget *parent,QWidget *mainWindow) :
    QMainWindow(parent)
    ,mainWindow(mainWindow)
    ,ui(new Ui::GameMainWindow)
    ,bird1(new Bird(400,400))
    ,bird2(new Bird(999,999))
    ,background(new Background)
    ,ground(new Ground(0))
    ,bkgdMusic(new QSoundEffect)
    ,gameScene(day)
    ,gameTimer(new QTimer)
    ,willRestart(false)
    ,score(0)
    ,scoreOne(new QLabel(this))
    ,scoreTen(new QLabel(this))
    ,scoreHundred(new QLabel(this))
    ,nums(new QStringList)
    ,coin(new Coin(this,this))
    ,coldDownTime(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Flappy Bird");
    this->setWindowIcon(QIcon(":/res/icon.ico"));
    ui->btnStartGame->setFocus();

    this->setFixedSize(800,800);//初始化游戏结束的字
    gameOver = new QLabel(this);
    gameOver->resize(390,90);
    gameOver->setScaledContents(true);
    gameOver->setPixmap(QPixmap(":/res/gameOver.png"));
    gameOver->move(205,225);
    gameOver->setVisible(false);

    QLabel *pressToStart = new QLabel(this);
    pressToStart->resize(310,190);
    pressToStart->setScaledContents(true);
    pressToStart->setPixmap(QPixmap(":/res/press_space_to_start.png"));
    pressToStart->move(245,100);
    pressToStart->show();

    scoreOne->resize(24,36);
    scoreOne->move(412,40);
    scoreOne->show();
    scoreTen->resize(24,36);
    scoreTen->move(388,40);
    scoreTen->show();
    scoreHundred->resize(24,36);
    scoreHundred->move(364,40);
    scoreHundred->show();
    *nums << QString(":/res/numbers/0.png") << QString(":/res/numbers/1.png") << QString(":/res/numbers/2.png")
         << QString(":/res/numbers/3.png") << QString(":/res/numbers/4.png") << QString(":/res/numbers/5.png")
         << QString(":/res/numbers/6.png") << QString(":/res/numbers/7.png") << QString(":/res/numbers/8.png")
         << QString(":/res/numbers/9.png");
    updateScoreLabel();

//    int *fps = new int(0);
//    *fps = 0;
    gameTime = 0;
    QTimer *timer = new QTimer;
    connect(ui->actionExit, &QAction::triggered, this, [=](){
        this->close();
    });
    connect(ui->btnStartGame,&QPushButton::clicked,this, [=](){
        if(gameOver->isVisible())
        {
            willRestart = true;
            this->close();
        }
        else
        {
            initGame();
            timer->start(25);
            ui->btnStartGame->setDisabled(true);
            ui->btnStartGame->hide();
            pressToStart->hide();
            ui->actionStart->setDisabled(true);
        }

    });
    connect(ui->actionStart,&QAction::triggered,this, [=](){
        if(gameOver->isVisible())
        {
            willRestart = true;
            this->close();
        }
        else
        {
            initGame();
            timer->start(25);
            ui->btnStartGame->setDisabled(true);
            ui->btnStartGame->hide();
            pressToStart->hide();
            ui->actionStart->setDisabled(true);
        }
    });

    connect(timer,&QTimer::timeout,this,[=](){
//        (*fps)++;
        updateFrame();
        if(gameRunning == false) //若游戏结束，开始结束收尾
        {
            timer->stop();
            gameTimer->stop();
            bkgdMusic->stop();
            coin->moveTimer->stop();
            pipeUp->moveTimer->stop();
            pipeUp->stepMoveTimer->stop();
            pipeUp->upAndDownMoveTimer->stop();
            pipeDown->moveTimer->stop();
            pipeDown->stepMoveTimer->stop();
            pipeDown->upAndDownMoveTimer->stop();
            background->timer->stop();
            showHighestScore();

            if(!isServer) crashed(); //用于在2P也能显示出游戏结束
        }

    });

    connect(gameTimer, &QTimer::timeout ,this, [=](){
//        qDebug() << *fps;
//        (*fps) = 0;
        gameTime++;
        if(gameTime == 24) //游戏运行24后卡点音乐进入feverTime
        {
            feverTime(1);
        }
        if(gameTime == 55)
        {
            feverTime(2);
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
    gameScene = static_cast<GameMainWindow::gameScenes>(difficulty);
    score=0;
    pipeUp = new Pipe(0,Pipe::up,this,false);
    pipeDown = new Pipe(0,Pipe::down,this,false);
    coin = new Coin(this,this,false);

    initMusic();

    if(gameMode == GameMainWindow::singelplayer) //单人游戏
    {
        isServer = true;
        pipeUp->isActive = true;
        pipeDown->isActive = true;
        coin->isActive = true;
        resetPipes();
        connect(pipeUp, &Pipe::resetMe, this, &GameMainWindow::resetPipes);
        connect(pipeUp, &Pipe::getScore, this, [=](){
            score++;
            scoreChanged();
        });
        coin->initCoin();
        connect(bird1,&Bird::flyStatusChanged,bird1,&Bird::flapWing); //让鸟飞的时候扇翅膀
        bird1->birdX=400;
        bird1->birdY=400;
        bird1->speed=0;
        connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed); //初始化碰撞检测
        connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
        connect(ground,&Ground::hitGround,this,&GameMainWindow::crashed);
        connect(coin,&Coin::getScore,this,[=](){
            score++;
            scoreChanged();
        });
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
            coin->isActive = true;
            coin->initCoin();
            resetPipes();
            connect(pipeUp, &Pipe::resetMe, this, &GameMainWindow::resetPipes);
            connect(pipeUp, &Pipe::getScore, this, [=](){
                score++;
                scoreChanged();
            });
            connect(pipeUp,&Pipe::crashed,this,&GameMainWindow::crashed);
            connect(pipeDown,&Pipe::crashed,this,&GameMainWindow::crashed);
            connect(ground,&Ground::hitGround,this,&GameMainWindow::crashed);
            connect(coin,&Coin::getScore,this,[=](){
                score++;
                scoreChanged();
            });
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
    if(coldDownTime >= 0) coldDownTime--;
    if(gameMode == GameMainWindow::singelplayer)
    {
        birdMove(bird1); //计算鸟的上下移动和重力加速度
        repaint(); //重绘
        checkCrash(); //检测碰撞
        return;
    }
    if(isServer)
    {
        birdMove(bird1);
        birdMove(bird2);
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
    else if(gameScene == GameMainWindow::city)
    {
        backgroundPainter.drawPixmap(0,0,1800,800,QPixmap(":/res/background_city.png"));
    }

    if(gameRunning == true)
    {
        if(gameScene == GameMainWindow::day)
        {
            QPainter upPipePainter(this);
            upPipePainter.translate(pipeUp->x, pipeUp->y);
            upPipePainter.drawPixmap(0,0,pipeUp->width,pipeUp->height,QPixmap(":/res/pipe_up.png"));
            QPainter downPipePainter(this);
            downPipePainter.translate(pipeDown->x, pipeDown->y);
            downPipePainter.drawPixmap(0,0,pipeDown->width,pipeDown->height,QPixmap(":/res/pipe_down.png"));
        }
        else if(gameScene == GameMainWindow::night)
        {
            QPainter upPipePainter(this);
            upPipePainter.translate(pipeUp->x, pipeUp->y);
            upPipePainter.drawPixmap(0,0,pipeUp->width,pipeUp->height,QPixmap(":/res/pipe_up_night.png"));
            QPainter downPipePainter(this);
            downPipePainter.translate(pipeDown->x, pipeDown->y);
            downPipePainter.drawPixmap(0,0,pipeDown->width,pipeDown->height,QPixmap(":/res/pipe_down_night.png"));
        }
        else if(gameScene == GameMainWindow::city)
        {
            QPainter upPipePainter(this);
            upPipePainter.translate(pipeUp->x, pipeUp->y);
            upPipePainter.drawPixmap(0,0,pipeUp->width,pipeUp->height,QPixmap(":/res/pipe_city.png").copy(0,0,pipeUp->width,pipeUp->height));
            QPainter downPipePainter(this);
            downPipePainter.translate(pipeDown->x, pipeDown->y);
            downPipePainter.drawPixmap(0,0,pipeDown->width,pipeDown->height,QPixmap(":/res/pipe_city.png").copy(0,0,pipeDown->width,pipeDown->height));
        }

    }

    QPainter groundPainter(this);
    groundPainter.translate(ground->x, 700);

    if(gameScene == GameMainWindow::day)
    {
        groundPainter.drawPixmap(0,0,1600,100,QPixmap(":/res/ground.png"));
    }
    else if(gameScene == GameMainWindow::night)
    {
        groundPainter.drawPixmap(0,0,1600,100,QPixmap(":/res/ground_night.png"));
    }
    else if(gameScene == GameMainWindow::city)
    {
        groundPainter.drawPixmap(0,0,1600,100,QPixmap(":/res/ground_city.png"));
    }

    QPainter coinPainter(this);
    coinPainter.translate(coin->x,coin->y);
    if(!coin->eaten)
    {
        coinPainter.drawPixmap(0,0,12,16,QPixmap(":/res/coin.png"));
    }


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
    if(coldDownTime > 0) return;
    if(event->key() == Qt::Key_Space)
    {
        if(gameMode == GameMainWindow::singelplayer || isServer == true)
        {
            coldDownTime = 5;
            bird1->fly();
        }
        if(gameMode == GameMainWindow::multiplayer && isServer == false)
        {
            MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
            coldDownTime = 5;
            myMain->socket->write("fly"); //多人且不是主机的情况下，向主机发送fly请求
        }
    }

}

void GameMainWindow::mousePressEvent(QMouseEvent *event)
{
    if(coldDownTime > 0) return;
    if(event->button() == Qt::LeftButton)
    {
        if(gameMode == GameMainWindow::singelplayer || isServer == true)
        {
            coldDownTime = 5;
            bird1->fly();
        }
        if(gameMode == GameMainWindow::multiplayer && isServer == false)
        {
            MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
            coldDownTime = 5;
            myMain->socket->write("fly");
        }
    }
}

void GameMainWindow::closeEvent(QCloseEvent *event)
{
    emit closed(willRestart); //用于关闭游戏窗口后显示菜单窗口
}

void GameMainWindow::initServer()
{
//    qDebug() << "initServer";
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
    MainWindow *myMain = static_cast<MainWindow *>(mainWindow);
    //监听主机消息，接收到主机信息时进行处理并同步
    connect(myMain->socket,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf = myMain->socket->readAll();
        QString bufStr;
        bufStr.prepend(buf);
//        qDebug() << bufStr;
        /*bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeUpHeight-pipeDownX
    -pipeDownY-Score-gameRunning-holePosition-difficulty-coinX-coinY-coinAeten
    */
        if(bufStr != "")
        {
            QStringList data = bufStr.split("~");
            syncWithServer(data);
        }
    });
}

void GameMainWindow::birdMove(Bird *bird)//鸟每帧的移动
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

void GameMainWindow::checkCrash() //每帧检测碰撞
{
    pipeUp->isCrashed(bird1);
    pipeDown->isCrashed(bird1);
    ground->checkHitGround(bird1);
    if(!coin->eaten) coin->isTouched(bird1);
    if(gameMode == GameMainWindow::multiplayer)
    {
        pipeUp->isCrashed(bird2);
        pipeDown->isCrashed(bird2);
        ground->checkHitGround(bird2);
        if(!coin->eaten) coin->isTouched(bird2);
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
    ui->btnStartGame->show();
    ui->btnStartGame->setEnabled(true);
    ui->btnStartGame->setFocus();
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
    /*bird1Y-bird2Y-b1flystatus-b2flystatus-pipeUpX-pipeUpHeight-pipeDownX
    -pipeDownY-Score-gameRunning-holePosition-difficulty-coinX-coinY-coinAeten
    */
//    qDebug() << "syncWithServer";
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
    if(difficulty != data.at(11).toInt())
    {
        difficulty = data.at(11).toInt();
        difficultyChanged();
    }
    pipeDown->caculatePosition(holePosition,pipeUp);
    pipeUp->caculatePosition(holePosition,pipeDown);
    coin->x = data.at(12).toInt();
    coin->y = data.at(13).toInt();
    coin->eaten = (data.at(14).toInt() == 1 ? true : false);
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
         << QString::number((pipeDown->y - pipeUp->height)/2 + pipeUp->height) << "~" << QString::number(difficulty) << "~"
         << QString::number(coin->x) << "~" << QString::number(coin->y) << "~" << (coin->eaten == true ? "1" : "0");
    QString tmpstr = data.join("");
    QByteArray tmpbytearr = tmpstr.toLocal8Bit();
    myMain->client->write(tmpbytearr);
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
    updateScoreLabel();
}

void GameMainWindow::initMusic() //初始化背景音乐
{
    bkgdMusic->setSource(QUrl::fromLocalFile(":/res/Shooting_Stars.wav"));
    bkgdMusic->setLoopCount(10);
    bkgdMusic->setVolume(0.6f);
    bkgdMusic->play();
    gameTimer->start(1000); //初始化音乐的时候再开始计时，以卡点
}

void GameMainWindow::feverTime(int times) //进入feverTime，背景切换到下一个，难度+1，并且开始水管移动
{
    if(gameMode == GameMainWindow::singelplayer || isServer)
    {
        if(times == 1)
        {
            pipeUp->startUpAndDown();
            pipeDown->startUpAndDown();
        }
        if(difficulty < 3)
        {
            difficulty++;
            ground->difficulty=difficulty;
        }
    }
    gameScene = static_cast<GameMainWindow::gameScenes>(difficulty);
}

void GameMainWindow::updateScoreLabel()
{
    int one = score % 10;
    int ten = (score % 100 - one) / 10;
    int hundred = (score - one - ten * 10) / 100;
    scoreOne->setPixmap(QPixmap((*nums).at(one)));
    if(score > 9) scoreTen->setPixmap(QPixmap((*nums).at(ten)));
    if(score > 99) scoreHundred->setPixmap(QPixmap((*nums).at(hundred)));
}

void GameMainWindow::showHighestScore()
{

    int highScore = static_cast<MainWindow *>(mainWindow)->highestScore;
    static_cast<MainWindow *>(mainWindow)->highestScore = score > highScore ? score : highScore;
    int highScoreOne = highScore % 10;
    int highScoreTen = (highScore % 100 - highScoreOne) / 10;
    int highScoreHundred = (highScore - highScoreOne - highScoreTen * 10) / 100;

    QLabel *highScoreLabel = new QLabel(this);
    highScoreLabel->resize(217,36);
    highScoreLabel->move(292,100);
    highScoreLabel->setPixmap(QPixmap(":/res/highest_score.png"));
    highScoreLabel->show();

    QLabel *one = new QLabel(this);
    QLabel *ten = new QLabel(this);
    QLabel *hundred = new QLabel(this);

    one->setPixmap(QPixmap((*nums).at(highScoreOne)));
    if(ten != 0) ten->setPixmap(QPixmap((*nums).at(highScoreTen)));
    if(hundred != 0) hundred->setPixmap(QPixmap((*nums).at(highScoreHundred)));

    one->resize(24,36);
    one->move(412,140);
    one->show();
    ten->resize(24,36);
    ten->move(388,140);
    ten->show();
    hundred->resize(24,36);
    hundred->move(364,140);
    hundred->show();

    connect(this,&GameMainWindow::closed,one,&QLabel::close);
    connect(this,&GameMainWindow::closed,ten,&QLabel::close);
    connect(this,&GameMainWindow::closed,hundred,&QLabel::close);
}

void GameMainWindow::difficultyChanged()
{
    ground->difficulty=difficulty;
    gameScene = static_cast<GameMainWindow::gameScenes>(difficulty);
}

