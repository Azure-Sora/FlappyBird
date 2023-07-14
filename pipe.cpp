#include "pipe.h"
#include <QPainter>
#include <QTimer>
#include <QThread>

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

    QTimer *timer = new QTimer;
    timer->start(50);
    connect(timer, &QTimer::timeout, [=](){
        move();
    });
}

void Pipe::move()
{
    x -= 10;
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
    if(this->position == Pipe::up && this->x > (bird->birdX - this->width) && this->x < (bird->birdX - 30))
    {
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
}

void Pipe::isScored()
{

}
