#include "autorization.hpp"
#include "ui_autorization.h"

Autorization::Autorization( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::Autorization )
{
    ui->setupUi( this );
    connect(ui->Login_button,SIGNAL(clicked()),this,SLOT(Send_Login()));
}
void Autorization::Send_Login()
{
    QString login,password;
    login=ui->Login->text();
    password=ui->Password->text();
    ui->Login_button->setEnabled(false);
    ui->Login_button->setText("Ожидание");
    if (emit Autorize(login,password))
        accepti();
    else Error();
}
void Autorization::Error()
{
    QMessageBox Wrong;
    Wrong.setWindowTitle( tr( "Ошибка" ) );
    Wrong.setText( tr( "Извините! Логин и/или пароль неверны или вы не имеете достаточно прав" ) );
    Wrong.setIcon( QMessageBox::Warning );
    Wrong.exec();
    ui->Login_button->setEnabled(true);
    ui->Login_button->setText("Вход");
    return;
}
void Autorization::accepti()
{
   ui->Login_button->setEnabled(true);
   ui->Login_button->setText("Вход");
   QMessageBox user;
   user.setWindowTitle( tr( "Подтверждение входа" ) );
   user.setIcon( QMessageBox::Information );
   user.setText( tr( "Получен доступ к серверу"));
   user.exec();
   QDialog::accept();
}

Autorization::~Autorization()
{
    delete ui;
}
