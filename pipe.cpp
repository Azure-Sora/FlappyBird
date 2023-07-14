#include "pipe.h"
#include <QPainter>
#include <QTimer>

Pipe::Pipe(int y,positions pos,QWidget *parent) : x(800) , y(y) , position(pos) , parent(parent)
{
    QTimer *timer = new QTimer;
    timer->start(50);
    connect(timer,&QTimer::timeout,[=](){
        move();
    });
}

void Pipe::initPosition(int holeWidth, int holePosition)
{
    this->x=400;
    switch (this->position) {
    case Pipe::up:
        this->y = 0;
        this->height = holePosition - holeWidth/2;
        break;
    case Pipe::down:
        this->y = 0;
        this->height = holePosition + holeWidth/2;
        break;
    default:
        break;
    }

    QTimer *timer = new QTimer;
    timer->start(50);
    connect(timer, &QTimer::timeout, [=](){
        move();
    });
}

void Pipe::move()
{
    x -= 3;
    if(x < -50)
    {
        resetMe();
    }
}


void Pipe::resetMe()
{
    this->x = 400;
}
