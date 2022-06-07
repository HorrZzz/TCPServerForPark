#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include "widget.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);


}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::setX(){
    X = ui->lineEdit->text();
    qDebug()<<"setX  运行："<<X;
}

void Dialog::setY(){
    Y = ui->lineEdit_2->text();
    qDebug()<<"setY  运行："<<Y;
}

void Dialog::setZ(){
    Z = ui->lineEdit_3->text();
    qDebug()<<"setZ  运行："<<Z;
}



