#include "startbutton.h"
#include <QPixmap>

StartButton::StartButton(QWidget *parent)
    : QLabel{parent}
{
    this->resize(108,64);
    this->setPixmap(QPixmap(":/res/play.png"));
}

void StartButton::enterEvent(QEnterEvent *event)
{
    this->setPixmap(QPixmap(":/res/play_touched.png"));
}

void StartButton::leaveEvent(QEvent *event)
{
    this->setPixmap(QPixmap(":/res/play.png"));
}

void StartButton::mousePressEvent(QMouseEvent *ev)
{
    this->setPixmap(QPixmap(":/res/play_pushed.png"));
    emit clicked();
}

void StartButton::mouseReleaseEvent(QMouseEvent *ev)
{
    this->setPixmap(QPixmap(":/res/play_touched.png"));
}
