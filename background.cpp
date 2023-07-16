#include "background.h"
#include <QTimer>

Background::Background()
{
    x=0;

    QTimer *timer = new QTimer;
    timer->start(25);

    connect(timer, &QTimer::timeout, this, &Background::move);

}

void Background::move()
{
    x -= 5;
    if(x <= -800)
    {
        resetMe();
    }
}

void Background::resetMe()
{
    x = 0;
}
