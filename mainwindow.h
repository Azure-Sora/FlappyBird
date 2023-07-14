#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bird.h"
#include <QPainter>
#include <QKeyEvent>
#include "pipe.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTcpServer *server;
    QTcpSocket *socket;

    void initGame();
    void updateFrame();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void createPipes();
    void initServer();
    void initClient();
    void birdMove();
    void checkCrash();
    void crashed();
    void resetPipes();

    const int gravity = 2;
    int score;
    bool gameRunning;
    Pipe *pipeUp;
    Pipe *pipeDown;
    QLabel *gameOver;


signals:
    void restartGame();

private:
    Ui::MainWindow *ui;
    Bird *bird;
};
#endif // MAINWINDOW_H
