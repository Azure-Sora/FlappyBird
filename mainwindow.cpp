#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "gamemainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isMultiplayer = false;

    connect(ui->btnStartGame,&QPushButton::clicked,[=](){
        startAGame();
    });
    connect(ui->btnConnect,&QPushButton::clicked,this,&MainWindow::initClient);



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::startAGame()
{
    GameMainWindow *gameMain = new GameMainWindow(this,this);
    gameMain->setAttribute(Qt::WA_QuitOnClose);
    connect(gameMain,&GameMainWindow::closed,this,&MainWindow::show);
//    connect(gameMain,&GameMainWindow::restartGame,this,&MainWindow::startAGame);
    gameMain->show();
    this->hide();
}

void MainWindow::initClient()
{
    socket = new QTcpSocket(this);
    QString ip = ui->ipEdit->text();
    int port = ui->portEdit->text().toInt();
    socket->connectToHost(ip,port);
    if(socket->waitForConnected(5000))
    {
        ui->connectionStatus->setText("已连接");
        isMultiplayer = true;
    }
    else
    {
        ui->connectionStatus->setText("连接失败");
    }


}
