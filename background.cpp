#include "background.h"
#include <QTimer>

Background::Background() : timer(new QTimer)
{
    x=0;

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
