#ifndef AUTORIZATION_HPP
#define AUTORIZATION_HPP

#include "user.hpp"
#include <QDialog>
#include <QString>
#include <QMessageBox>
#include <QCryptographicHash>

namespace Ui {
class Autorization;
}

class Autorization : public QDialog
{
    Q_OBJECT

public:
    explicit Autorization(QWidget *parent = 0);
    ~Autorization();
    bool init();
    User return_User();
    void Set_user(User user);

signals:
    void Autorize(QString Login,QString Password);
    void registerUser(QString Surname,QString Name,QString Last_name,QString Login,QString Password,QString Role);

private slots:
    void accepti();
    void registration();
    void Send_Login();
    void registration_complete();
    void registration_error();
    void registration_accept();
    void registration_cancel();
    void Next_page();
    void Prev_page();
    void Error();

private:
    Ui::Autorization *ui;
    User current,regUser;
    QString user_login,user_password;
};

#endif // AUTORIZATION_HPP
