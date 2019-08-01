#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QStringListModel>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDateTime>
#include <QTcpSocket>
class QJsonDocument;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void HelloUDP();
private slots:

    void setIp();
    void readyRead();
    void dataSender(const QJsonObject &message,const QHostAddress,quint16 port);
    void addMessage(QJsonObject,QStringListModel *temp,QHostAddress);

    void listCreatorServer(const QJsonObject &message,QHostAddress);
    void sendList(QHostAddress,quint16 portSend);

private:
    Ui::MainWindow *ui;
    QUdpSocket *socket;
    QStringListModel *users;
    QStringListModel *message;
    QStringList ListM;
    QStringList ListU;
    QString userName="none";
    QJsonArray listUsers;
    QHostAddress localAddr=QHostAddress::LocalHost;
};

#endif // MAINWINDOW_H
