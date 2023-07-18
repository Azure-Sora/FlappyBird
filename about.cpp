#include "about.h"
#include "ui_about.h"
#include "gamemainwindow.h"
#include <QDesktopServices>

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    setWindowTitle("关于");
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        QDesktopServices::openUrl(QUrl(QString("https://github.com/Azure-Sora/FlappyBird")));
    });
    this->setWindowModality(Qt::ApplicationModal);
}

About::~About()
{
    delete ui;
}
