#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QAction>
#include <QPainter>
#include <QTimer>
#include <QPoint>
#include <QRandomGenerator>
#include "pipe.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bird(new Bird)
{
    ui->setupUi(this);

    initGame();

    QTimer *timer = new QTimer;

    timer->start(50);

    connect(timer,&QTimer::timeout,this,[=](){
        if(gameRunning == false) timer->stop();
        updateFrame();
    });

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
    bird->x=400;
    bird->y=400;
    bird->speed=0;
    gameRunning = true;
    emit this->continueGame();
}

void MainWindow::updateFrame()
{
    bird->speed += gravity;
    bird->y += bird->speed;
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.translate(bird->x,bird->y);
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

//    int length;
//    switch (pipeUp->position) {
//    case Pipe::up:
//        length = pipeUp->y;
//        painter.translate(pipeUp->x,0);
//        painter.drawRect(0,0,pipeUp->width,length);
//        break;
//    case Pipe::down:
//        length = 800 - pipeUp->y;
//        painter.translate(pipeUp->x,pipeUp->y);
//        painter.drawRect(0,0,pipeUp->width,length);
//    default:
//        break;
//    }

//    switch (pipeDown->position) {
//    case Pipe::up:
//        length = pipeDown->y;
//        painter.translate(pipeDown->x,0);
//        painter.drawRect(0,0,pipeDown->width,length);
//        break;
//    case Pipe::down:
//        length = 800 - pipeDown->y;
//        painter.translate(pipeDown->x,pipeDown->y);
//        painter.drawRect(0,0,pipeDown->width,length);
//    default:
//        break;
//    }

}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        bird->fly();
    }

}

void MainWindow::createPipes()
{
    int holeWidth = QRandomGenerator::global()->bounded(200,300);
    int holeCenter = QRandomGenerator::global()->bounded(300,600);
    pipeUp = new Pipe(holeCenter - holeWidth/2, Pipe::up, this);
    pipeDown = new Pipe(holeCenter + holeWidth/2, Pipe::down, this);
}


