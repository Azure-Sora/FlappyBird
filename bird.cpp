#include "bird.h"
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <QSoundEffect>

Bird::Bird(int x, int y)
{
    flyStatus = 1;
    this->birdX = x;
    this->birdY = y;

}

void Bird::fly()
{
    if(speed - flyPower < maxFlySpeed)
    {
        speed = maxFlySpeed;
    }
    else
    {
        speed -= flyPower;
    }
    flyStatus = 3;

    QSoundEffect *flySound = new QSoundEffect;
    flySound->setSource(QUrl::fromLocalFile(":/res/wing.wav"));
    flySound->setLoopCount(1);
    flySound->setVolume(0.4f);
    flySound->play();
    connect(flySound, &QSoundEffect::playingChanged, [=](){
        if(flySound->isPlaying())
        {
            flySound->deleteLater();
        }
    });

    emit flyStatusChanged();//用于扇翅膀动画
}

void Bird::flapWing()
{
    //用flyStatus播放扇翅膀动画
    QTimer *timer = new QTimer;
    timer->start(100);
    connect(timer,&QTimer::timeout,this,[=](){
        if(flyStatus>3||flyStatus<1)
        {
            flyStatus = 3;
        }
        this->flyStatus-=1;
        if(this->flyStatus==1) timer->stop();
    });

}


