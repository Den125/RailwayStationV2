#ifndef GUI_HPP
#define GUI_HPP

#include <QMainWindow>
#include <QTextCodec>
#include <QFile>
#include "flight.hpp"
#include "autorization.hpp"
#include "carriage.hpp"
#include "server.hpp"
#include "database.hpp"
#include "user.hpp"
#include <QTime>
#include <QSqlTableModel>
#include "QJsonArray"
#include "QJsonDocument"
#include "QJsonParseError"

namespace Ui {
class Gui;
}
class Server;

class Gui : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = nullptr);
    ~Gui();
    void CreateModelUsers(const QStringList &headers);
    void CreateModelFlights(const QStringList &headers);
    void CreateModelCarriages(const QStringList &headers);
    void Enable_Flight_Settings();
    void Enable_Carriage_Settings();
    void Enable_User_Settings();
    void Enable_Connect_Settings();
    bool OpenServer();
    bool Login();
    QTimer *timer;
signals:
    void timeout();
public slots:
    void on_button_StartStop_toggled(bool checked);
    void addToLog(QString text);
    bool Check_User(QString login,QString password);
    QByteArray Data_Flight();
    User Return_User(QString login,QString password);
    bool AddUser(User user);
    void DeleteFlight();
    void DeleteCarriage();
    void DeleteUser();
    bool ConnectDatabase();
    void AddFlight();
    void EditFlight();
    void CancelFlight();
    void AddCarriage();
    void EditCarriage();
    void CancelCarriage();
    void AddUser();
    void EditUser();
    void CancelUser();
    void AddFlightToDatabase();
    void AddCarriageToDatabase();
    void AddUserToDatabase();
    void EditFlightToDatabase();
    void EditCarriageToDatabase();
    void EditUserToDatabase();
    bool Sell(QString flight,QString carriage,QString dates);
    void Connect();
    void AddCarriageToFlight();
    void RemoveCarriageFromFlight();
    void SetCarriage(int);
    void CloseConnectCarriage();
    bool CheckAdm(QString login,QString password);
    void Logout();
    void Exit();
    int Number();
    bool return_Ticket(QString tnumber,QString fnumber,QString fcarriage,QString date);
    void timer_overflow();
    void SetDate(int ind);
private:    
    QString NumberF;
    Ui::Gui *ui;
    Server *_serv;
    Database *db;
    Autorization *autorize;
    QSqlTableModel *model_user,*model_flight,*model_carriage,*model_carriage_flight,*model_carriage_not_flight;
};

#endif // GUI2_HPP
