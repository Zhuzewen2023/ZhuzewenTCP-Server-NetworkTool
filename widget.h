#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include "mycombobox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QTcpServer* server;
    void refreshComboBox();

public slots:
    void on_new_client_connect();
    void on_readyRead_handler();
    void on_client_disconnect();
    void on_socket_state_changed(QAbstractSocket::SocketState socketState);
    void on_comboBox_refresh();

private slots:
    void on_pushButtonStartListening_clicked();

    void on_pushButtonSend_clicked();

    void on_comboBoxChild_activated(int index);

    void on_pushButtonStopListening_clicked();

    void on_pushButtonDisconnect_clicked();

private:
    Ui::Widget *ui;
    int childIndex;


};
#endif // WIDGET_H
