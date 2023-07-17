#ifndef STARTBUTTON_H
#define STARTBUTTON_H

#include <QLabel>
#include <QMouseEvent>

class StartButton : public QLabel
{
    Q_OBJECT
public:
    explicit StartButton(QWidget *parent = nullptr);

    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

signals:
    void clicked();
};

#endif // STARTBUTTON_H
