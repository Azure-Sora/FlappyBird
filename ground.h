#ifndef GROUND_H
#define GROUND_H

#include <QWidget>
#include <QTimer>

class Ground : public QWidget
{
    Q_OBJECT
public:
    Ground(int difficulty);
    int x;
    int difficulty;

    void move();
    void resetMe();
    void checkHitGround();
};

#endif // GROUND_H
