#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    QString GetX(){
        qDebug()<<"return X  运行："<<X;
        return X;
    }

    QString GetY(){
        qDebug()<<"return Y  运行："<<Y;
        return Y;
    }

    QString GetZ(){
        qDebug()<<"return Z  运行："<<Z;
        return Z;
    }

    void setX();
    void setY();
    void setZ();

private slots:


private:
    Ui::Dialog *ui;
    QString X,Y,Z;
};

#endif // DIALOG_H




