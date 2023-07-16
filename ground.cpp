#include "ground.h"

Ground::Ground(int difficulty) : difficulty(difficulty)
{
    x=0;

    QTimer *timer = new QTimer;
    timer->start(25);

    connect(timer, &QTimer::timeout, this, &Ground::move);

}

void Ground::move()
{
    x -= 5 * difficulty;
    if(x <= -800)
    {
        resetMe();
    }
}

void Ground::resetMe()
{
    x=0;
}

void Ground::checkHitGround(Bird *bird)
{
    if(bird->birdY > 660)
    {
        emit hitGround();
    }
}



