#include "pipe.h"
#include <QPainter>
#include <QTimer>
#include <QThread>
#include "gamemainwindow.h"
#include <QRandomGenerator>

Pipe::Pipe(int y,positions pos,QWidget *parent,bool active) : x(800) , y(y) , position(pos) , myParent(parent), isActive(active)
    , upAndDownMoveTimer(new QTimer), stepMoveTimer(new QTimer), moveTimer(new QTimer), readyGetScore(false)
{
    connect(moveTimer,&QTimer::timeout,this,[=](){
        move();
    });
}

void Pipe::move()
{
    x -= ( 5 * static_cast<GameMainWindow *>(myParent)->difficulty);
    if(x < -50)
    {
        emit resetMe();
    }
}

void Pipe::isCrashed(Bird *bird) //判定碰撞
{
    if(this->x > (bird->birdX - this->width) && this->x < (bird->birdX + 40))
    {
        if(this->position == Pipe::down)
        {
            if(bird->birdY + 40 >= this->y)
            {
                emit crashed();
                return;
            }
        }
        if(this->position == Pipe::up)
        {
            if(bird->birdY <= this->height)
            {
                emit crashed();
                return;
            }
        }
    }
    //通过检测管子通过鸟的前后判定得分
    if(!readyGetScore && this->position == Pipe::up && this->x > (bird->birdX + 40) && this->x < (bird->birdX + 40 + width))
    {
        readyGetScore = true;
    }
    if(readyGetScore && this->position == Pipe::up && this->x > (bird->birdX - this->width) && this->x < (bird->birdX - 30))
    {
        readyGetScore = false;
        emit getScore();
    }
}

void Pipe::reset(int holeWidth, int holePosition)
{
    this->x=800;
    switch (this->position) {
    case Pipe::up:
        this->y = 0;
        this->height = holePosition - holeWidth/2;
        break;
    case Pipe::down:
        this->y = holePosition + holeWidth/2;
        this->height = 800 - y;
        break;
    default:
        break;
    }
    if(isActive && moveTimer->isActive() == false) //如果是主机的管子且没在动，则开始自行移动
    {
        moveTimer->start(25);
    }
}

void Pipe::caculatePosition(int holePosition, Pipe *another) //计算自己的位置，供2P使用
{
    if(this->position == Pipe::down)
    {
        this->height = 800 - y;
    }
    if(this->position == Pipe::up)
    {
        this->height = holePosition - (another->y - holePosition);
    }
}

void Pipe::startUpAndDown() //开始上下运动
{
    //每隔一秒随即出下一秒要动的量
    upAndDownMovement = 0;
    upAndDownMoveTimer->start(1000);
    connect(upAndDownMoveTimer, &QTimer::timeout, this, [=](){
        upAndDownMovement = QRandomGenerator::global()->bounded(-100,100);
//        qDebug() << QString::number(upAndDownMovement);
    });

    //把每秒运动分成20份进行
    stepMoveTimer->start(25);
    connect(stepMoveTimer, &QTimer::timeout,this,[=](){
//        qDebug() << QString::number(upAndDownMovement / 20);
        if(this->position == Pipe::up)
        {
            this->height += upAndDownMovement / 50;
        }
        else
        {
            this->y += upAndDownMovement / 50;
        }
    });
}

