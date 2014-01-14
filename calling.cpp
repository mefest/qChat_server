#include "calling.h"
#include "ui_calling.h"

calling::calling(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::calling)
{
    ui->setupUi(this);

    //setAttribute(Qt::WA_TranslucentBackground );

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setWindowOpacity(0.5);
    ui->pushButton->setWindowOpacity(1);
}

calling::~calling()
{
    delete ui;
}
