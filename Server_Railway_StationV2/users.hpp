#ifndef USERS_HPP
#define USERS_HPP

#include "user.hpp"
#include <QAbstractTableModel>

namespace Ui
{
class Users;
}

class Users :public QAbstractTableModel
{
    Q_OBJECT

public:
    Users();
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
    void add(const User &user);
    int size() const;
    void set(std::vector<User> users);
    const User &operator[](int idx) const;
    bool UserChange(const QModelIndex &index,const User &user);
    bool erase(int idx);
    void write(QDataStream &stat);
    bool Check(int idx);
    void erase_activity();
    bool check_login(QString login);

private:
    Ui::Users *ui;
    std::vector<User> User_all;
};

#endif // USERS_HPP
