#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QFileDialog>
#include <memory>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>
#include <QTextCodec>
#include <station.hpp>
#include "autorization.hpp"
#include "kass.hpp"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool Connect();
    bool Login();
signals:
    void AutorizeTrue();
    void AutorizeFalse();
    void RegisterTrue();
    void RegisterFalse();
    void SellTrue(int num);
    void SellFalse();

private slots:
    void onSokConnected();
    void onSokDisconnected();
    void onSokReadyRead();
    void onSokError(QAbstractSocket::SocketError socketError) const;
    void AutorizeRequest(QString login,QString password);
    void on_About_triggered();
    void RegisterRequest(QString Surname,QString Name,QString Last_name,QString Login,QString Password,QString Role);
    void sell_Ticket();
    void RefreshDate();
    void Exit();
    void Send_Request(QByteArray command);
    void Ticket_Return();
    void Logout_();

private:
    QString fileName;
    QByteArray Data,itog;
    Ui::MainWindow *ui;
    QTcpSocket *_sok;
    int data_size;
    bool complexData=false;
    std::unique_ptr<Station> mStation;
    Autorization *log;
    QJsonDocument JDoc;
    QJsonParseError JError;
    User user;
};

#endif // MAINWINDOW_HPP
