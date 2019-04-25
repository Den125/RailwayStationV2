#ifndef SERVER_HPP
#define SERVER_HPP

#include <QTcpServer>
#include <QDebug>
#include "client.hpp"
#include "user.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

class Client;

class Server: public QTcpServer
{
    Q_OBJECT
public:
   // Server();
    explicit Server(QWidget *widget = 0, QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);

signals:
    bool Check(QString login,QString Password);
    void addLogToGui(QString str);
    QByteArray Filght_data();
    User DUser(QString login,QString password);
    bool UserReg(User user);

public slots:
    void ClientReady();
    void ClientDisc();
    bool CheckUser(QString login,QString Password);
    QByteArray Data_Flights();
    User User_Data(QString login,QString password);
    void RemoveUser(Client *cli);
    bool RegisterUser(User user);

protected:
    void incomingConnection(qintptr handle);

private:
    QList<Client *> _clients;
    QWidget *_widget;
    QTcpServer *server;
    quint32 _blockSize;
    int desc;
    QJsonDocument JDoc;
    QJsonParseError JError;
};
#endif // SERVER_HPP
