#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"
#include "user.hpp"
#include <QMessageBox>
#include <QObject>
#include <QDialog>
#include <QDebug>
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
    const QString comAuthreq = "Autorization";
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
    int DataSize;
    bool ComplexData=false;
    bool Autorized;
    QJsonDocument JDoc;
    QJsonParseError JError;
    User user;
};
    /* static const QString constNameUnknown;
     explicit Client(int desc, Server *serv, QObject *parent = 0);
signals:
    void addUserToGui(QString name);
    void removeUserFromGui(QString name);
    void removeUser(Client *client);
    void messageToGui(QString message, QString from, const QStringList &users);

*/

   // Server *_serv;

 //   QString _name;
  //  bool _isAutched;

/*class Client:public QObject
{
    friend class Server;
    Q_OBJECT
public:
     static const QString constNameUnknown;
     explicit Client(int desc, Server *serv, QObject *parent = 0);
signals:
    void addUserToGui(QString name);
    void removeUserFromGui(QString name);
    void removeUser(Client *client);
    void messageToGui(QString message, QString from, const QStringList &users);

private slots:
    void onConnect();
    void onDisconnect();
 //   void onReadyRead();
  //  void onError(QAbstractSocket::SocketError socketError) const;
private:
    QTcpSocket *_sok;
    Server *_serv;
    quint16 _blockSize;
    QString _name;
    bool _isAutched;
};*/

#endif // CLIENT_HPP
