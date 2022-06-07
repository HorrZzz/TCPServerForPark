#include "widget.h"
#include "ui_widget.h"
#include <QHostInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QtTest/QTest>
#include <QFile>
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->frame->hide();
    ui->widget_6->hide();


    //发送按钮 默认不可按
    ui->btnSend->setEnabled(false);

    //本地主机名
    QString hostName = QHostInfo::localHostName();

    //本机IP地址
    QHostInfo hostInfo = QHostInfo::fromName(hostName);

    //IP地址列表
    QList<QHostAddress> addrList = hostInfo.addresses();
    for(int i=0;i<addrList.count();i++)
    {
        QHostAddress host = addrList.at(i);

        if(QAbstractSocket::IPv4Protocol == host.protocol())
        {
            QString ip = host.toString();
            ui->comboBox->addItem(ip);
        }
    }

    //m_tcpServer指针指向新创建的QTcpServer
    m_tcpServer = new QTcpServer(this);

    //newConnection信号槽连接
    connect(m_tcpServer,&QTcpServer::newConnection,this,&Widget::on_newConnection);


    //停止按钮变灰
    ui->btnStop->setEnabled(false);


}

Widget::~Widget()
{
    delete ui;
}

void Widget::Send(QString s)
{//发送命令 函数
    if(/*m_tcpSocket->state() == QAbstractSocket::ConnectedState*/    CanConnect)
    {
        QString msg = s;

        //添加日志时间戳
        ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]")+"[SEND]");
        ui->plainTextEdit->appendPlainText(msg);
        ui->plainTextEdit->appendPlainText("---------------------------------------");

        QByteArray str = msg.toUtf8();
        m_tcpSocket->write(str);
    }
    else
        QMessageBox::warning(this,"错误","客户端未连接");
}

void Widget::on_btnStart_clicked()
{


    //当前选择的ip
    QString ip = ui->comboBox->currentText();

    //端口
    int port = ui->lineEditPort->text().toInt();

    QHostAddress addr(ip);

    //监听
    bool listening = m_tcpServer->listen(addr,port);
    if(listening)
    {
        ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]"));

        ui->plainTextEdit->appendPlainText("**开始监听...");

        ui->plainTextEdit->appendPlainText("**服务器地址: "+m_tcpServer->serverAddress().toString());

        ui->plainTextEdit->appendPlainText("**服务器端口: "+QString::number(m_tcpServer->serverPort()));

        ui->plainTextEdit->appendPlainText("---------------------------------------");

        ui->btnStart->setEnabled(false);

        ui->btnStop->setEnabled(true);

        ui->lbListen->setText("正在监听");
    }
    else
    {
        QMessageBox::warning(this,"监听启动失败","监听启动失败!\n请检查是否有其他软件冲突!");
    }


}

void Widget::on_btnStop_clicked()
{
    if(m_tcpServer->isListening())
    {//判断如果服务端还在监听状态，那么关闭监听
        m_tcpServer->close();
        if(CanConnect)
            m_tcpSocket->close();
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);

        //添加日志时间戳
        ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]"));
        ui->lbListen->setText("停止监听");
        ui->plainTextEdit->appendPlainText("**停止监听**");
    }
}

void Widget::on_btnClear_clicked()
{//清空日志
    ui->plainTextEdit->clear();
}

void Widget::on_newConnection()
{//新连接 槽函数

    CanConnect = true;

    m_tcpSocket = m_tcpServer->nextPendingConnection();
    //    connect(m_tcpSocket,&QTcpSocket::connected,this,&Widget::onConnected);
    onConnected();
    connect(m_tcpSocket,&QTcpSocket::disconnected,this,&Widget::onDisConnected);
    connect(m_tcpSocket,&QTcpSocket::stateChanged,this,&Widget::onStateChanged);
    onStateChanged(m_tcpSocket->state());
    connect(m_tcpSocket,&QTcpSocket::readyRead,this,&Widget::onReadyRead);
}

void Widget::onConnected()
{//连接成功
    ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]"));
    ui->plainTextEdit->appendPlainText("** client socket connected");
    ui->plainTextEdit->appendPlainText("** peer address: "+m_tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText("** peer port: "+QString::number(m_tcpSocket->peerPort()));
    ui->plainTextEdit->appendPlainText("---------------------------------------");

    //发送按钮 可按
    ui->btnSend->setEnabled(true);
}

void Widget::onDisConnected()
{//连接断开
    //添加日志时间戳
    ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]"));
    ui->plainTextEdit->appendPlainText("** client socket disconnected");

    //状态检测
    onStateChanged(m_tcpSocket->state());

    m_tcpSocket->deleteLater();

    //发送按钮 不可按
    ui->btnSend->setEnabled(false);

    CanConnect = false;

}

void Widget::onStateChanged(QAbstractSocket::SocketState socketState)
{//状态检测 槽函数
    switch (socketState)
    {
    case QAbstractSocket::UnconnectedState:
        ui->lbListen_2->setText("UnconnectedState");break;
    case QAbstractSocket::HostLookupState:
        ui->lbListen_2->setText("HostLookupState");break;
    case QAbstractSocket::ConnectedState:
        ui->lbListen_2->setText("ConnectedState");break;
    case QAbstractSocket::ConnectingState:
        ui->lbListen_2->setText("ConnectingState");break;
    case QAbstractSocket::BoundState:
        ui->lbListen_2->setText("BoundState");break;
    case QAbstractSocket::ClosingState:
        ui->lbListen_2->setText("ClosingState");break;
    case QAbstractSocket::ListeningState:
        ui->lbListen_2->setText("ListeningState");break;
    default:
        break;
    }

}

void Widget::onReadyRead()
{
    isReadyReadOk = 0;
    //添加日志时间戳
    ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]")+"[RECV]");
    ReadLine = QString(m_tcpSocket->readLine());
    qDebug()<<"onReadyRead: "+ReadLine;
    isReadyReadOk = 1;
    ui->plainTextEdit->appendPlainText(ReadLine);
    ui->plainTextEdit->appendPlainText("---------------------------------------");
}

void Widget::on_btnSend_clicked()
{
    QString msg =ui->lineEdit->text();

    //发送命令
    Send(msg);
    //    str.append('\n');

}

//X,Y,Z坐标Dialog输入插入到QString里，函数
void Widget::If_Dialog_exec(QString s)
{
    Dialog q;
    if(q.exec() == Dialog::Accepted)
    {
        q.setX();
        q.setY();
        q.setZ();

        int x,y,z;
        x = s.indexOf("X:");
        if(x>0)
            s = s.insert(x+2,q.GetX());

        y = s.indexOf("Y:");
        if(y>0)
            s = s.insert(y+2,q.GetY());

        z = s.indexOf("Z:");
        if(z>0)
            s = s.insert(z+2,q.GetZ());
        qDebug() << s ;


        //发送命令
        Send(s);
    }
    else
        qDebug() << "取消设定，未生效";
}



//槽：LineEdit监控回车键，绑定发送命令
void Widget::on_lineEdit_returnPressed()
{
    Send(ui->lineEdit->text());
    ui->lineEdit->clear();
}

void Widget::sleep(unsigned int msec) //延时函数
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}



bool Widget::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
    const char *s = ba.data();

    while(*s && *s>='0' && *s<='9')
    {
        s++;
    }
    if (*s)
    { //不是纯数字
        return false;
    }
    else
    { //纯数字
        return true;
    }
}

void Widget::AutoSend()
{
    if(CanConnect){
        if(isReadyReadOk)
            ReadLine = "";
        QFileDialog* fdialog = new QFileDialog(this);
        QString path = QCoreApplication::applicationDirPath();
        QString file_name = fdialog->getOpenFileName(this,"选择文件",path,"TXT files(*.txt)");
        QFile file(file_name);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text) && CanConnect)
        {
            QStringList txt;
            QTextStream in(&file);
            while (!in.atEnd()) {
                txt.append(in.readLine());
            }
            Send(txt.at(0));
            for(int i = 1 ; i < txt.size() ; i++){
                bool Next = true;
                while (Next) {
                    if(!ReadLine.isEmpty())
                        Next = false;
                    QTest::qWait(500);
                }
                if(CanConnect){
                    if(txt.at(i) == "D|Car")
                        QTest::qWait(500);
                    Send(txt.at(i));
                    if(isReadyReadOk)
                        ReadLine = "";
                }
            }
            ui->plainTextEdit->appendPlainText("文件执行完毕!");
            ui->plainTextEdit->appendPlainText("---------------------------------------");
            if(isReadyReadOk){
                ReadLine = "";
                qDebug()<<"3: "+ReadLine;
            }
        }
        file.close();
        delete fdialog;
        fdialog = NULL;
    }
}

void Widget::on_pushButton_4_clicked()   //根据返回值来判断
{
    if(CanConnect)
    {
        if(isReadyReadOk){
            ReadLine = "";
            qDebug()<<"1: "+ReadLine;
        }

        QString path1 = QCoreApplication::applicationDirPath();
        QString path = path1+"/1.txt";
        QFileDialog* fdialog = new QFileDialog(this);
        QString file_name = fdialog->getOpenFileName(this,"选择文件",path,"Txt files(*.txt)");

        QFile file(file_name);

        int l = 1; //显示行数
        if(file.open(QIODevice::ReadOnly | QIODevice::Text) && CanConnect)
        {
            ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]"));
            ui->plainTextEdit->appendPlainText("[文件加载成功!]");
            QString line;
            QTextStream in(&file);
            line = in.readLine();

            QString msg = line;
            ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]")+"[SEND]");
            ui->plainTextEdit->appendPlainText("第["+QString::number(l)+"]行: "+msg);
            ui->plainTextEdit->appendPlainText("---------------------------------------");
            QByteArray str = msg.toUtf8();
            m_tcpSocket->write(str);

            line = in.readLine();
            qDebug() <<"文件准备就绪";

            while (!line.isNull() && CanConnect) {
                if(!ReadLine.isEmpty()){
                    isOK = true;
                }
                else{
                    isOK = false;
                }
                if(isReadyReadOk){
                    ReadLine = "";
                    qDebug()<<"2: "+ReadLine;
                }
                if(isOK)
                {
                    QString msg = line;
                    //添加日志时间戳
                    ui->plainTextEdit->appendPlainText(QDateTime::currentDateTime().toString("[yy-MM-dd hh:mm:ss]")+"[SEND]");
                    ui->plainTextEdit->appendPlainText("第["+QString::number(++l)+"]行: "+msg);
                    ui->plainTextEdit->appendPlainText("---------------------------------------");
                    QByteArray str = msg.toUtf8();
                    m_tcpSocket->write(str);
                    line = in.readLine();
                    isOK = false;
                }
                else
                    QTest::qWait(5);
            }
            ui->plainTextEdit->appendPlainText("文件执行完毕!");
            ui->plainTextEdit->appendPlainText("---------------------------------------");
            if(isReadyReadOk){
                ReadLine = "";
                qDebug()<<"3: "+ReadLine;
            }
        }
        file.close();
        delete fdialog;
        fdialog = NULL;
    }
    else
        QMessageBox::warning(this,"错误","客户端未连接");
}


void Widget::on_test_clicked()
{
    AutoSend();
}

