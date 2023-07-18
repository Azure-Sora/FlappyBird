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
#include "background.h"
#include "ground.h"
#include <QSoundEffect>

class Coin;

namespace Ui {
class GameMainWindow;
}

class GameMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameMainWindow(QWidget *parent = nullptr,QWidget *mainWindow = nullptr);
    ~GameMainWindow();

    void initGame();
    void updateFrame();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void createPipes();
    void initServer();
    void initClient();
    void birdMove(Bird *bird);
    void checkCrash();
    void crashed();
    void resetPipes();
    void syncWithServer(QStringList data);
    void syncWithClient();
    void scoreChanged();
    void initMusic();
    void feverTime(int times);
    void updateScoreLabel();
    void showHighestScore();
    void difficultyChanged();

    enum playerCount {singelplayer , multiplayer};
    playerCount gameMode;

    enum gameScenes {day = 1 , night , city};
    gameScenes gameScene = day;

    bool isServer;
    const int gravity = 1;
    int score;
    bool gameRunning;
    Pipe *pipeUp;
    Pipe *pipeDown;
    Coin *coin;
    QLabel *gameOver;
    QWidget *mainWindow;
    int difficulty;
    Background *background;
    Ground *ground;
    QSoundEffect *bkgdMusic;
    int gameTime;
    QTimer *gameTimer;
    bool willRestart;
    QLabel *scoreOne;
    QLabel *scoreTen;
    QLabel *scoreHundred;
    QStringList *nums;


signals:
    void closed(bool restart);

private:
    Ui::GameMainWindow *ui;
    Bird *bird1;
    Bird *bird2;
};

#endif // GAMEMAINWINDOW_H
