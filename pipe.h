#ifndef PIPE_H
#define PIPE_H

#include <QWidget>
#include <QPainter>
#include <QEvent>


class Pipe : public QWidget
{
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
    const int width = 50;
    QWidget *parent;

    void move();
    void paintEvent(QPaintEvent *event);
};

#endif // PIPE_H
