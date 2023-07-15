#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "gamemainwindow.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    isMultiplayer = false;


    connect(ui->btnStartServer,&QPushButton::clicked,this,&MainWindow::initServer);

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

void MainWindow::initServer()
{
    server = new QTcpServer(this);
    int port = ui->serverPortEdit->text().toInt();
    server->listen(QHostAddress::Any,port);
    ui->serverPortEdit->setText(QString("已在%1端口启动").arg(QString::number(port)));
    ui->serverPortEdit->setReadOnly(true);
    connect(server,&QTcpServer::newConnection,this,[=](){
        client = server->nextPendingConnection();
        ui->connectionStatus->setText("已作为1P连接");
        ui->btnConnect->setDisabled(true);
        isMultiplayer = true;
        isServer = true;
    });
    ui->btnStartServer->setDisabled(true);
    ui->btnConnect->setDisabled(true);
}

void MainWindow::initClient()
{
    socket = new QTcpSocket(this);
    QString ip = ui->ipEdit->text();
    int port = ui->portEdit->text().toInt();
    socket->connectToHost(ip,port);
    if(socket->waitForConnected(5000))
    {
        ui->connectionStatus->setText("已作为2P连接");
        ui->btnStartServer->setDisabled(true);
        ui->btnConnect->setDisabled(true);
        isMultiplayer = true;
        isServer = false;
    }
    else
    {
        ui->connectionStatus->setText("连接失败");
    }


}
