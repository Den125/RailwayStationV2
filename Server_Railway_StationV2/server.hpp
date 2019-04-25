#ifndef SERVER_HPP
#define SERVER_HPP

#include <QTcpServer>
#include "client.hpp"
#include "gui.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

class Client;

class Server: public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QWidget *widget = 0, QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);
    QVector<User> list_users();

signals:
    bool Check(QString login,QString Password);
    void addLogToGui(QString str);
    QByteArray Filght_data();
    User DUser(QString login,QString password);
    bool UserReg(User user);
    bool Accept_Sell(QString flight,QString carriage,QString);
    int Number_Ticket();
    bool return_ticket(QString Number_T,QString Number_F,QString Number_C,QString date);

public slots:
    void ClientReady();
    void ClientDisc();
    bool CheckUser(QString login,QString Password);
    QByteArray Data_Flights();
    User User_Data(QString login,QString password);
    void RemoveUser(Client *cli);
    bool RegisterUser(User user);
    bool Check_Sell(QString flight,QString carriage,QString date);
    int Number();
    bool Return_Tick(QString Number_T,QString Number_F,QString Number_C,QString date);
    void ToLog(QString string);
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
