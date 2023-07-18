#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QWidget>
#include <QTimer>

class Background : public QWidget
{
    Q_OBJECT

public:
    Background();

    int x;

    QTimer *timer;

    void move();
    void resetMe();
};

#endif // BACKGROUND_H
