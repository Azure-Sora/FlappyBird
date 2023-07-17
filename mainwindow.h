#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamemainwindow.h"
#include "startbutton.h"

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
    QTcpSocket *client;

    bool isMultiplayer;
    bool isServer;
    int difficulty;
    int highestScore;

    void startAGame();
    void initServer();
    void initClient();


signals:


private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
