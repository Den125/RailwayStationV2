#ifndef AUTORIZATION_HPP
#define AUTORIZATION_HPP
#include <QMessageBox>
#include <QDialog>
#include <QString>

namespace Ui {
class Autorization;
}

class Autorization : public QDialog
{
    Q_OBJECT

public:
    explicit Autorization(QWidget *parent = 0);
    ~Autorization();

signals:
    bool Autorize(QString Login,QString Password);

private slots:
    void accepti();
    void Send_Login();
    void Error();

private:
    Ui::Autorization *ui;
    QString user_login,user_password;
};

#endif // AUTORIZATION_HPP
