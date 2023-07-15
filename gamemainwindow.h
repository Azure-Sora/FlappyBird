#ifndef GAMEMAINWINDOW_H
#define GAMEMAINWINDOW_H

#include <QMainWindow>
#include "bird.h"
#include <QPainter>
#include <QKeyEvent>
#include "pipe.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include "mainwindow.h"
#include <QStringList>

namespace Ui {
class GameMainWindow;
}

class GameMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameMainWindow(QWidget *parent = nullptr,QWidget *mainWindow = nullptr);
    ~GameMainWindow();

    QTcpServer *server;
    QTcpSocket *socket;

    void initGame();
    void updateFrame();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void createPipes();
    void initServer();
    void initClient();
    void birdMove();
    void checkCrash();
    void crashed();
    void resetPipes();
    void syncWithServer(QStringList data);
    void syncWithClient();
    void bird2Move();
    void scoreChanged();

    enum playerCount {singelplayer , multiplayer};
    playerCount gameMode;

    bool isServer;
    const int gravity = 2;
    int score;
    bool gameRunning;
    Pipe *pipeUp;
    Pipe *pipeDown;
    QLabel *gameOver;
    QWidget *mainWindow;
    int difficulty;


signals:
//    void restartGame();
    void closed();

private:
    Ui::GameMainWindow *ui;
    Bird *bird1;
    Bird *bird2;
};

#endif // GAMEMAINWINDOW_H
