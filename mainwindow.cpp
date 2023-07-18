#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "gamemainwindow.h"
#include <QString>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(new QTcpServer(this))
    , socket(new QTcpSocket(this))
    , client(nullptr)
    , highestScore(0)
{
    ui->setupUi(this);

    this->setWindowTitle("主菜单");
    isMultiplayer = false;
    difficulty = 0;
    ui->difficultySelector->setCurrentIndex(0);

    connect(ui->difficultySelector,&QComboBox::currentIndexChanged,this,[=](){
        difficulty = ui->difficultySelector->currentIndex();
        switch (difficulty) {
        case 0:
            ui->backgroundPic->setPixmap(QPixmap(":/res/background_day.png"));
            break;
        case 1:
            ui->backgroundPic->setPixmap(QPixmap(":/res/background_night.png"));
            break;
        case 2:
            ui->backgroundPic->setPixmap(QPixmap(":/res/background_city.png"));
            break;
        default:
            break;
        }
    });

    connect(ui->btnStartServer,&QPushButton::clicked,this,&MainWindow::initServer);

    connect(ui->btnStartGame,&StartButton::clicked,this,[=](){
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
    connect(gameMain,&GameMainWindow::closed,this,[=](bool restart){
        this->show();
        if(restart) startAGame();
    });
//    connect(gameMain,&GameMainWindow::restartGame,this,&MainWindow::startAGame);
    gameMain->show();
    this->hide();
}

void MainWindow::initServer()
{
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

