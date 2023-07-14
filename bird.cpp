#include "bird.h"
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QDebug>

Bird::Bird()
{
    flyStatus = 1;

}

void Bird::fly()
{
    speed -= flyPower;
    flyStatus = 3;
    emit flyStatusChanged();
}

void Bird::flapWing()
{
    QTimer *timer = new QTimer;
    timer->start(200);
    connect(timer,&QTimer::timeout,[=](){
        if(flyStatus>3||flyStatus<1)
        {
            flyStatus = 3;
        }
        this->flyStatus-=1;
        if(this->flyStatus==1) timer->stop();
    });

}


