#ifndef COIN_H
#define COIN_H

#include <QWidget>
#include <QTimer>
#include "gamemainwindow.h"

class Coin : public QWidget
{
    Q_OBJECT
public:
    Coin(QWidget *parent = nullptr, QWidget *myParent = nullptr, bool active = false);

    int x;
    int y;
    bool isActive;
    bool eaten;
    QTimer *moveTimer;
    QWidget *myParent;

    void move();
    void initCoin();
    void resetMe();
    void isTouched(Bird *bird);


signals:
    void getScore();
};

#endif // COIN_H
