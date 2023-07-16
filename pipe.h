#ifndef PIPE_H
#define PIPE_H

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include "bird.h"
#include <QTimer>


class Pipe : public QWidget
{
    Q_OBJECT

public:
    enum positions
    {
        up = 1,
        down = 2
    };
    Pipe(int y,positions pos,QWidget *parent,bool active);
    positions position;
    int x;
    int y;
    int height;
    const int width = 50;
    QWidget *myParent;
    bool isActive;
    QTimer *moveTimer;
    QTimer *upAndDownMoveTimer;
    QTimer *stepMoveTimer;
    int upAndDownMovement;

    void initPosition(int holeWidth, int holePosition);
    void move();
//    void resetMe();
    void isCrashed(Bird *bird);
    void reset(int holeWidth, int holePosition);
    void isScored();
    void caculatePosition(int holePosition, Pipe *another);
    void startUpAndDown();

signals:
    void crashed();
    void resetMe();
    void getScore();
};

#endif // PIPE_H
