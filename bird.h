#ifndef BIRD_H
#define BIRD_H

#include <QString>
#include <QWidget>

class Bird : public QWidget
{
    Q_OBJECT

public:
    Bird();
    int birdX;
    int birdY;
    int speed;
    int flyStatus;
    const int flyPower = 8;

    void fly();
    void flapWing();

signals:
    void flyStatusChanged();

};

#endif // BIRD_H
