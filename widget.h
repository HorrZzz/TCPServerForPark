#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include "dialog.h"
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void Send(QString s);
    void If_Dialog_exec(QString s);

private slots:

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    void on_btnClear_clicked();

    void on_newConnection();

    void onConnected();

    void onDisConnected();

    void onStateChanged(QAbstractSocket::SocketState socketState);

    void onReadyRead();

    void on_btnSend_clicked();

    void on_lineEdit_returnPressed();

    void on_pushButton_4_clicked();

    void on_test_clicked();

private:
    Ui::Widget *ui;

    QTcpServer *m_tcpServer = nullptr;
    QTcpSocket *m_tcpSocket = nullptr;
    bool CanConnect = false;

    void sleep(unsigned int msec);

    bool isDigitStr(QString src);

    bool isOK = false;

    bool isReadyReadOk = 0;

    QString ReadLine;

    void AutoSend();

};
#endif // WIDGET_H
