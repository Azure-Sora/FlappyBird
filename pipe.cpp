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

void Pipe::move()
{
    x -= 3;
    if(x < -50)
    {
        this->close();
    }
}

void Pipe::paintEvent(QPaintEvent *event)
{
    QPainter painter(parent);



}
