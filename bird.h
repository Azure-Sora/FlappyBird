#ifndef BIRD_H
#define BIRD_H

#include <QString>
#include <QWidget>

class Bird : public QWidget
{
    Q_OBJECT

public:
    Bird();
    int x;
    int y;
    int speed;
    int flyStatus;
    const int flyPower = 20;

    void fly();
    void flapWing();

signals:
    void flyStatusChanged();

};

#endif // BIRD_H
