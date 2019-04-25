#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <QObject>
#include <QVariant>
#include <QSql>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QFile>
#include <QCryptographicHash>
#include <QDate>
#include "flight.hpp"
#include "carriage.hpp"
#include "user.hpp"

class Database:public QObject
{
    Q_OBJECT
public:
    bool createConnection(QString Server);
    explicit Database(QWidget *parent = 0);
    bool CheckTableFlights();
    bool CheckTableCarriages();
    bool CheckTableUsers();
    bool CheckTableTickets();
    bool DeleteFlight(int num);
    bool DeleteCarraige(int num);
    bool DeleteUser(QString login);
    bool EditData(Flight fli);
    bool EditData(Carriage carri);
    bool EditData(User user);
    bool AddToDataBase(Flight fli);
    bool AddToDataBase(Carriage carri);
    bool AddToDataBase(User user);
    bool sell(QString flight,QString carriage, QString date);
    bool AddToFlight(QString flight,QString carriage,QString day,QDate date);
    bool RemoveFromFlight(QString carriage);
    int maxTicket();
    bool return_ticket(QString tnumber,QString fnumber,QString cnumber,QString date);
    void Time();
signals:
    void TableStat(QString text);
private:
    QSqlDatabase    db;
    QWidget *_widget;
};

#endif // DATABASE_HPP
