#include "widget.h"
#include "ui_widget.h"

#include <QNetworkInterface>
#include <QTcpSocket>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->verticalLayout);

    server = new QTcpServer(this);

    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonStopListening->setEnabled(false);

    connect(server, &QTcpServer::newConnection, this, &Widget::on_new_client_connect);
    //connect(server, SIGNAL(newConnection()), this, SLOT(on_new_client_connect()));
    connect(ui->comboBoxChild, &MyComboBox::on_comboBox_clicked, this, &Widget::on_comboBox_refresh);

    //获取主机ip地址并且添加到服务器IP地址后面的下拉框中
    QList<QHostAddress> addressList = QNetworkInterface::allAddresses();
    for(QHostAddress address : addressList)
    {
            if(address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                ui->comboBoxAddress->addItem(address.toString());
            }

    }


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_new_client_connect() //客户端连接的槽函数
{
    /*
    hasPendingConnections()
    hasPendingConnections() 是 QTcpServer 类的一个成员函数，它返回一个布尔值（true 或 false），
    指示服务器是否有待处理的连接。如果有客户端尝试连接到服务器但尚未被服务器应用程序接受
    （即，尚未调用 nextPendingConnection() 来获取这个连接），则此函数返回 true。

    nextPendingConnection()
    nextPendingConnection() 也是 QTcpServer 类的一个成员函数，它用于从服务器获取下一个待处理的连接。
    当服务器有待处理的连接时（即 hasPendingConnections() 返回 true），调用此函数将返回一个新的 QTcpSocket 对象，
    该对象代表了与客户端之间的连接。服务器应用程序可以使用这个 QTcpSocket 对象来与客户端进行通信。
    */
    qDebug() << "new client connect success!";
    if(server->hasPendingConnections())
    {
        QTcpSocket *connection = server->nextPendingConnection();
        connect(connection, &QIODevice::readyRead, this, &Widget::on_readyRead_handler);
        qDebug() << "client addr: " << connection->peerAddress().toString() << ", port: " << connection->peerPort();
        /*
            insertPlainText：在光标当前位置插入文本，适合用户交互时的文本添加。
            append（虽然QTextEdit没有直接的append方法，但可以通过移动光标和insertPlainText模拟）：在文本末尾添加内容，适合日志记录、聊天消息等顺序性内容的添加。
            setText：替换文本区域的所有内容，适合需要重置文本区域内容时使用。
        */
        ui->textEditRecv->insertPlainText("客户端地址：" + connection->peerAddress().toString() + "\n客户端端口号：" + QString::number(connection->peerPort()) + "\n");
        ui->textEditRecv->moveCursor(QTextCursor::EndOfLine);
        ui->textEditRecv->ensureCursorVisible();

        connect(connection, &QAbstractSocket::disconnected, this, &Widget::on_client_disconnect);
        connect(connection, &QAbstractSocket::stateChanged, this, &Widget::on_socket_state_changed);
    }
}

void Widget::on_readyRead_handler() //收到消息时的处理函数
{
    //收到消息时的处理函数
    qDebug() << " Receive Data: ";
    QTcpSocket* tmpSocket = qobject_cast<QTcpSocket *>(sender());
    QByteArray receiveData = tmpSocket->readAll();
    QString receivedString = QString::fromUtf8(receiveData);
    ui->textEditRecv->insertPlainText(receivedString);
    //ui->textEditRecv->moveCursor(QTextCursor::Down);
    ui->textEditRecv->ensureCursorVisible();
}

void Widget::refreshComboBox()
{
    ui->comboBoxChild->clear();

}

void Widget::on_client_disconnect() //客户端断开时打印
{
    QTcpSocket* tmpSock = qobject_cast<QTcpSocket*>(sender());
    ui->textEditRecv->insertPlainText("客户端断开!\n");

    //qDebug() << "refresh combobox";
    tmpSock->deleteLater();
    Widget::refreshComboBox();
}

void Widget::on_socket_state_changed(QAbstractSocket::SocketState socketState) //客户端状态变化时打印
{
    QTcpSocket* tmpSock = qobject_cast<QTcpSocket*>(sender());
    switch(socketState) {
        case QAbstractSocket::UnconnectedState:
            // 套接字未连接
            ui->textEditRecv->insertPlainText("套接字未连接\n");
            tmpSock->deleteLater();
            Widget::refreshComboBox();

            break;
        case QAbstractSocket::HostLookupState:
            // 套接字正在查找主机名
            ui->textEditRecv->insertPlainText("套接字正在查找主机名\n");
            break;
        case QAbstractSocket::ConnectingState:
            // 套接字正在连接
            ui->textEditRecv->insertPlainText("套接字正在连接\n");
            break;
        case QAbstractSocket::ConnectedState:
            // 套接字已连接
            ui->textEditRecv->insertPlainText("套接字已连接\n");
            break;
        case QAbstractSocket::BoundState:
            // 套接字已绑定到地址（对于服务器套接字）
            ui->textEditRecv->insertPlainText("套接字已绑定到地址（对于服务器套接字）\n");
            break;
        case QAbstractSocket::ListeningState:
            // 套接字正在监听传入连接（对于服务器套接字）
            ui->textEditRecv->insertPlainText("套接字正在监听传入连接（对于服务器套接字）\n");
            break;
        case QAbstractSocket::ClosingState:
            // 套接字正在关闭
            ui->textEditRecv->insertPlainText("套接字正在关闭\n");

            break;
        default:
            // 处理未知状态
            ui->textEditRecv->insertPlainText("处理未知状态\n");
            break;
    }
}

void Widget::on_comboBox_refresh() //客户端端口下拉框的刷新控件
{
    ui->comboBoxChild->clear();
    //qDebug() << "现在在on_comboBox_refresh槽函数里面";
    QList<QTcpSocket*>tcpSocketClients = server->findChildren<QTcpSocket *>();
    for(QTcpSocket* tcpSocketClient : tcpSocketClients)
    {
        if(tcpSocketClient->peerPort() != 0 && tcpSocketClient->peerPort() != NULL)
            ui->comboBoxChild->addItem(QString::number(tcpSocketClient->peerPort()));

    }
    ui->comboBoxChild->addItem("全部发送");
}

void Widget::on_pushButtonStartListening_clicked() //点击开始监听的槽函数
{
    //ui->pushButtonSend->setEnabled(true);
    QHostAddress addr(ui->comboBoxAddress->currentText());
    int port = ui->lineEditPort->text().toInt();
    if(!server->listen(addr, port))
    {
        qDebug() << "listen Error";
        ui->textEditRecv->insertPlainText("开启监听失败...\n");
         ui->pushButtonSend->setEnabled(false);
        ui->textEditRecv->moveCursor(QTextCursor::EndOfLine);
        ui->textEditRecv->ensureCursorVisible();
        if(port == 0)
        {
            ui->textEditRecv->insertPlainText("端口号不能为空...\n");
            ui->textEditRecv->moveCursor(QTextCursor::EndOfLine);
            ui->textEditRecv->ensureCursorVisible();
            return;
        }
        if(server->isListening())
         {
            ui->textEditRecv->insertPlainText("服务器已经在监听中...\n");
            ui->pushButtonStartListening->setEnabled(false);
            ui->pushButtonStopListening->setEnabled(true);
            ui->pushButtonDisconnect->setEnabled(true);
            ui->pushButtonSend->setEnabled(true);
         }
        return;
    }
    else
    {
        qDebug() << "listen Success, listening...";
        ui->textEditRecv->insertPlainText("开启监听成功...\n");
        ui->textEditRecv->moveCursor(QTextCursor::EndOfLine);
        ui->textEditRecv->ensureCursorVisible();
        ui->pushButtonStartListening->setEnabled(false);
        ui->pushButtonDisconnect->setEnabled(true);
        ui->pushButtonStopListening->setEnabled(true);
         ui->pushButtonSend->setEnabled(true);
    }
}

void Widget::on_pushButtonSend_clicked() //发送按钮的槽函数
{
    QList<QTcpSocket *> tcpSocketClients = server->findChildren<QTcpSocket *>();
    if(ui->comboBoxChild->currentText() == "")
    {
        ui->textEditRecv->insertPlainText("请选择发送的客户端的端口！\n");
    }
    else if(ui->comboBoxChild->currentText() != "全部发送")
    {
        tcpSocketClients[childIndex]->write(ui->textEditSend->toPlainText().toStdString().c_str());

    }else
    {
        for(QTcpSocket* client : tcpSocketClients)
        {
            int ret = client->write(ui->textEditSend->toPlainText().toStdString().c_str());
            /*
            1. toStdString
            用途：toStdString 通常不是标准C++库中的直接成员函数，而是某些特定库（如Qt框架中的QString类）提供的成员函数。
            它的作用是将特定类型的字符串（如QString）转换为标准C++库中的std::string类型。
            所属类：toStdString 属于提供该函数的类，例如Qt中的QString类。
            转换方向：从特定类型的字符串（如QString）到std::string的转换。
            2. c_str()
            用途：c_str() 是C++标准库中std::string类的一个成员函数。
            它的作用是将std::string对象转换为一个以null结尾的字符数组（C风格字符串），即返回一个指向常量字符的指针（const char*）。
            这允许std::string对象与需要C风格字符串作为参数的C库函数或其他函数接口兼容。
            所属类：c_str() 属于std::string类。
            转换方向：从std::string到C风格字符串（const char*）的转换。
            */
            qDebug() << ret;
        }
    }

}

void Widget::on_comboBoxChild_activated(int index) //把客户端端口号下拉框中的index传到全局变量中，用于send按钮的槽函数的使用
{
    childIndex = index;

}

void Widget::on_pushButtonStopListening_clicked()
{
    QList<QTcpSocket *>tcpSocketClients = server->findChildren<QTcpSocket *>();
    for(QTcpSocket* client:tcpSocketClients)
    {
        client->close();
    }
    server->close();
    ui->pushButtonSend->setEnabled(false);
    ui->pushButtonStartListening->setEnabled(true);
    ui->pushButtonStopListening->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(false);
}

void Widget::on_pushButtonDisconnect_clicked()
{
    this->on_pushButtonStopListening_clicked();
    delete server;
    this->close();
}
