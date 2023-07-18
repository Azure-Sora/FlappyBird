#include "coin.h"
#include <QDebug>
#include <QRandomGenerator>

Coin::Coin(QWidget *parent, QWidget *myParent, bool active)
    : QWidget{parent}, isActive(active), eaten(false), moveTimer(new QTimer), myParent(myParent)
{

    connect(moveTimer,&QTimer::timeout,this,[=](){
        move();
    });
}

void Coin::move()
{
    x -= ( 5 * static_cast<GameMainWindow *>(myParent)->difficulty);
    if(x < -50)
    {
        resetMe();
    }
}

void Coin::initCoin()
{
    y = QRandomGenerator::global()->bounded(200,600);
    x = 1200;
    if(isActive && moveTimer->isActive() == false)
    {
        moveTimer->start(25);
    }
}

void Coin::resetMe()
{
    y = QRandomGenerator::global()->bounded(200,600);
    this->x = 800;
    eaten = false;
}

void Coin::isTouched(Bird *bird)
{
    if(bird->birdX > (x - 40) && bird->birdX < (x + 12))
    {
        if(bird->birdY > (y - 40) && bird->birdX < (x + 16))
        {
            eaten = true;
            emit getScore();
        }
    }
}
