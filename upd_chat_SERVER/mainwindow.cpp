#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QHostAddress>
#include <QDateTime>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    setIp();
    socket = new QUdpSocket(this);

    socket -> bind(localAddr,26000);
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    message = new QStringListModel(this);//dodanie do listy
    users = new QStringListModel(this);
    ListM<<"Message from: ";
    ListU<<"Users:";
    message->setStringList(ListM);
    users->setStringList(ListU);
    ui->messageView->setModel(message);
    ui->userView->setModel(users);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setIp()
{
    QTcpSocket socketTest;

    socketTest.connectToHost("8.8.8.8", 53); // google DNS, or something else reliable
    if (socketTest.waitForConnected()) {
        qDebug()
            << "local IPv4 address for Internet connectivity is"
            << socketTest.localAddress();
    } else {
        qWarning()
            << "could not determine local IPv4 address:"
            << socketTest.errorString();
    }
    localAddr=socketTest.localAddress();
}
void MainWindow::readyRead()
{
    QByteArray buffer;
    buffer.resize(socket->pendingDatagramSize());//ustawienie jako pierwszy pakiet przychodzacy udp wielkosc

    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(buffer.data(), buffer.size(),&sender, &senderPort);

    QJsonObject docObj = (QJsonDocument::fromJson(buffer)).object();//convert to json data
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    addMessage(docObj,message,sender);
    if (typeVal.isNull() || !typeVal.isString())
        return; // not serialized data
    QString typeMessage = typeVal.toString();
    if(typeMessage.compare(QLatin1String("connect"), Qt::CaseInsensitive) == 0)
    {
        qDebug() << "its called LIST CREATOR";
        listCreatorServer(docObj,sender);
        sendList(sender,senderPort);
    }
    if(typeMessage.compare(QLatin1String("list"), Qt::CaseInsensitive) == 0)//server
    {
        qDebug() << "list request";
        sendList(sender,senderPort);
    }
    qDebug()<<"addMESSAGE";


    qDebug() << "Message from: " << sender.toString();
    qDebug() << "Message port: " << senderPort;
    qDebug() << "Message: " << buffer;
    //*******************************************************************Stream
    qDebug()<<"JsonObjcet"<<typeVal.toString();
}



void MainWindow::dataSender(const QJsonObject &message, const QHostAddress addr, quint16 port)
{
    socket->writeDatagram(QJsonDocument(message).toJson(QJsonDocument::Compact),addr,port);
}

void MainWindow::addMessage(const QJsonObject message,QStringListModel *temp,QHostAddress add)
{
    int row = temp->rowCount();

        // Enable add one or more rows
        temp->insertRows(row,1);

        // Get the row for Edit mode
        QModelIndex index = temp->index(row);

        // Enable item selection and put it edit mode
        ui->messageView->setCurrentIndex(index);
        temp->setData(temp->index(row, 0), message.value("type").toString()+" "+add.toString() +" "+QDateTime::currentDateTime().toString());
        qDebug() << "addMessage work " << message;
}


void MainWindow::listCreatorServer(const QJsonObject &message,QHostAddress senderL)//server function "connect"
{
    int i=0;
    //QJsonValue bb=(listUsers.at(i)).toObject().value("ip");
    while(i<listUsers.count())
    {
        if((listUsers.at(i)).toObject().value("ip").toString().compare(senderL.toString(),Qt::CaseInsensitive)==0)
        {
          break;
        }
            i++;
    }
    qDebug()<<"WARTOS WIELKOSCI"<<listUsers.count();
    qDebug()<<"WARTOS I"<<i;

    QJsonObject tempObj;
    tempObj["type"]=QStringLiteral("userslist");
    tempObj["index"]=i;
    tempObj["ip"]=senderL.toString();
    tempObj["username"]=message.value("username");
    tempObj["time"]=QDateTime::currentDateTime().toString();
    if(i<listUsers.count())
    {listUsers.replace(i,tempObj);
    qDebug()<<"replace ip";
    }
    else {



        listUsers.append(tempObj);
        qDebug()<<"append";
    }
    qDebug()<<"DISPLAY ARR: "<<listUsers;
    // Enable add one or more rows

    int row = users->rowCount();

        // Enable add one or more rows
        users->insertRows(row,1);

        // Get the row for Edit mode
        QModelIndex index = users->index(row);

        // Enable item selection and put it edit mode
        ui->userView->setCurrentIndex(index);
        users->setData(users->index(row, 0), senderL.toString()+"/"+message.value("username").toString()+":"+QDateTime::currentDateTime().toString());
}

void MainWindow::sendList(QHostAddress senderL,quint16 portSend)//server function
{
    int i =0;
    QJsonObject temp;
    while(i<listUsers.count())
    {
        qDebug()<<"WGAT RETURN "<<listUsers[i];
        dataSender(listUsers[i].toObject(),senderL,portSend);
        i++;
    }
    temp["type"]=QStringLiteral("refresh");
    dataSender(temp,senderL,portSend);
}
