#ifndef USER_HPP
#define USER_HPP

#include <QString>

class User
{
public:
    User();
    User(QString first, QString middle, QString last, QString login, QString pass, QString role);
    QString get_login() const;
    QString get_pw() const;
    QString get_first_name() const;
    QString get_middle_name() const;
    QString get_last_name() const;
    QString get_role() const;
    void set_password(QString pass);
    void set_login(QString log);
    void set_first_name(QString name);
    void set_middle_name(QString name);
    void set_last_name(QString name);
    void set_Role(QString role);
    void set_active(bool flag);
    bool get_active();

private:
    QString First_name,Middle_name,Last_name, Login,Password,Role;
    bool active=false;
};

#endif // USER_HPP
