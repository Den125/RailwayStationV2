#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"
#include "user.hpp"
#include <QMessageBox>
#include <QCryptographicHash>
#include <QObject>
#include <QDialog>
#include <QTcpSocket>
#include <QThreadPool>
#include <QtGui>
#include <QRegExp>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

class Server;

class Client:public QDialog
{
    Q_OBJECT
public:
    explicit Client(QWidget *parent = 0);
    explicit Client(qintptr handle,Server *serv,QWidget *parent = 0);
    bool checkStatus();
    int DeskSocket();
    void SetDesk(qintptr des);
    bool retAutorize();
    void setAutrorize(bool stat);
    void Set_User(User _user);
    User Return_User();
signals:
    void readyRead();
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    bool Autorize(QString login,QString password);
    QByteArray Flights();
    User user_activate(QString login,QString password);
    void removeUser(Client *cli);
    bool registerUser(User user);
    bool Sell(QString flight,QString carriage,QString date);
    int Ticket_number();
    bool Return_Ticket(QString Number_T,QString Number_F,QString Number_C,QString date);
    void AddtoLog(QString string);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError) const;

private:
    QTcpSocket *_sok;
    QWidget *_widget;
    Server *_serv;
    QByteArray Data,itog;
    int DataSize,p,n,q;
    bool ComplexData=false;
    bool Autorized;
    QJsonDocument JDoc;
    QJsonParseError JError;
    User user;
};
#endif // CLIENT_HPP
