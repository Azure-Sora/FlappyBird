#include "pipe.h"
#include <QPainter>
#include <QTimer>
#include <QThread>
#include "gamemainwindow.h"
#include <QRandomGenerator>

Pipe::Pipe(int y,positions pos,QWidget *parent,bool active) : x(800) , y(y) , position(pos) , myParent(parent), isActive(active)
    , upAndDownMoveTimer(new QTimer), stepMoveTimer(new QTimer)
{
    moveTimer = new QTimer;
    connect(moveTimer,&QTimer::timeout,[=](){
        move();
    });
}

void Pipe::initPosition(int holeWidth, int holePosition)
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

    if(isActive && moveTimer->isActive() == false)
    {
        moveTimer->start(25);
    }
}

void Pipe::move()
{
    x -= ( 5 * static_cast<GameMainWindow *>(myParent)->difficulty);
    if(x < -50)
    {
        emit resetMe();
    }
}


//void Pipe::resetMe()
//{
//    this->x = 800;
//}

void Pipe::isCrashed(Bird *bird)
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
    if(static_cast<GameMainWindow *>(myParent)->difficulty >= 2)
    {
        if(this->position == Pipe::up && this->x > (bird->birdX - this->width) && this->x < (bird->birdX - 30))
        {
            emit getScore();
        }
    }
    if(static_cast<GameMainWindow *>(myParent)->difficulty == 1)
    {
        if(this->position == Pipe::up && this->x > (bird->birdX - this->width) && this->x < (bird->birdX - 40))
        {
            emit getScore();
        }
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
}

void Pipe::isScored()
{

}

void Pipe::caculatePosition(int holePosition, Pipe *another)
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

void Pipe::startUpAndDown()
{
    upAndDownMovement = 0;
    upAndDownMoveTimer->start(1000);
    connect(upAndDownMoveTimer, &QTimer::timeout, [=](){
        upAndDownMovement = QRandomGenerator::global()->bounded(-100,100);
//        qDebug() << QString::number(upAndDownMovement);
    });
    stepMoveTimer->start(50);
    connect(stepMoveTimer, &QTimer::timeout,this,[=](){
//        qDebug() << QString::number(upAndDownMovement / 20);
        if(this->position == Pipe::up)
        {
            this->height += upAndDownMovement / 20;
        }
        else
        {
            this->y += upAndDownMovement / 20;
        }
    });
}

