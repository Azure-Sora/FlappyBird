#ifndef PIPE_H
#define PIPE_H

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include "bird.h"


class Pipe : public QWidget
{
    Q_OBJECT

public:
    enum positions
    {
        up = 1,
        down = 2
    };
    Pipe(int y,positions pos,QWidget *parent);
    positions position;
    int x;
    int y;
    int height;
    const int width = 50;
    QWidget *myParent;

    void initPosition(int holeWidth, int holePosition);
    void move();
//    void resetMe();
    void isCrashed(Bird *bird);
    void reset(int holeWidth, int holePosition);
    void isScored();

signals:
    void crashed();
    void resetMe();
    void getScore();
};

#endif // PIPE_H
