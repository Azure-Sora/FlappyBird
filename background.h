#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QWidget>

class Background : public QWidget
{
    Q_OBJECT

public:
    Background();

    int x;

    void move();
    void resetMe();
};

#endif // BACKGROUND_H
