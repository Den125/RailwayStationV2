#include "autorization.hpp"
#include "ui_autorization.h"

Autorization::Autorization( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::Autorization )
{
    ui->setupUi( this );
    ui->reg_1->setVisible(false);
    ui->reg_2->setVisible(false);
    connect(ui->Login_button,SIGNAL(clicked()),this,SLOT(Send_Login()));
    connect(ui->Next,SIGNAL(clicked()),this,SLOT(Next_page()));
    connect(ui->Prev,SIGNAL(clicked()),this,SLOT(Prev_page()));
}
void Autorization::Set_user(User user)
{
    current=user;
}
Autorization::~Autorization()
{
    delete ui;
}

void Autorization::Send_Login()
{
    QString login,password;
    login=ui->Login->text();
    password=ui->Password->text();
    ui->Login_button->setEnabled(false);
    ui->Login_button->setText("Ожидание");
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());
    QString hashStr(hash.result().toHex());
    emit Autorize(login,hashStr);
}

void Autorization::Error()
{
    QMessageBox Wrong;
    Wrong.setWindowTitle( tr( "Ошибка" ) );
    Wrong.setText( tr( "Извините! Логин и/или пароль неверны! Или данный пользователь уже в системе" ) );
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
   ui->Login->clear();
   ui->Password->clear();
   QMessageBox user;
   user.setWindowTitle( tr( "Подтверждение входа" ) );
   user.setIcon( QMessageBox::Information );
   user.setText( tr( "Вы зашли как: %1 %2 %3. "
                     "<br>Хорошего дня" )
                 .arg(current.get_first_name(),
                      current.get_middle_name(),
                      current.get_last_name() ) );
   user.exec();
   QDialog::accept();
}

User Autorization::return_User()
{
    return current;
}

void Autorization::registration()
{
    ui->reg_1->setVisible( true );
    ui->reg_1->setEnabled( true );
    ui->Login_widget->setEnabled(false);
    ui->Login_widget->setVisible(false);
}

void Autorization::registration_accept()
{
    if ( ui->login_user->text() == NULL || ui->password_user->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поля <Логин> и/или <Пароль> не могут быть пустым!" ) );
        error.exec();
        return;
    }
    if ( ui->first_name->text() == NULL || ui->last_name->text() == NULL || ui->name->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Вы должны ввести данные пользователя!" ) );
        error.exec();
        return;
    }
    if ( ui->login_user->text().size() < 5 || ui->password_user->text().size() < 5 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Логин> и/или <Пароль> не могут быть меньше 5 символов!" ) );
        error.exec();
        return;
    }
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData( ui->password_user->text().toUtf8());
    QString hashStr(hash.result().toHex());
    regUser.set_first_name( ui->first_name->text() );
    regUser.set_middle_name( ui->name->text() );
    regUser.set_last_name( ui->last_name->text() );
    regUser.set_login( ui->login_user->text() );
    regUser.set_password(hashStr);
    regUser.set_Role( "Пассажир" );
    ui->reg_accept->setEnabled(false);
    ui->reg_cancel->setEnabled(false);
    emit registerUser(regUser.get_first_name(),regUser.get_middle_name(),regUser.get_last_name(),
                      regUser.get_login(),regUser.get_pw(),regUser.get_role());
}

void Autorization::registration_complete()
{
    QMessageBox complete( this );
    complete.setWindowTitle( tr( "Регистрация" ) );
    complete.setIcon( QMessageBox::Information );
    complete.setText( tr( "Регистрация успешна!" ) );
    complete.exec();
    ui->login_user->clear();
    ui->password_user->clear();
    ui->first_name->clear();
    ui->name->clear();
    ui->last_name->clear();
    ui->reg_accept->setEnabled(true);
    ui->reg_cancel->setEnabled(true);
    ui->reg_2->setVisible(false);
    ui->reg_2->setEnabled( false );
    ui->Login_widget->setEnabled(true);
    ui->Login_widget->setVisible(true);
}

void Autorization::registration_error()
{
    QMessageBox error( this );
    error.setWindowTitle( tr( "Ошибка" ) );
    error.setIcon( QMessageBox::Warning );
    error.setText( tr( "Извините. Пользователь с таким логином уже существует!" ) );
    error.exec();
    ui->reg_accept->setEnabled(true);
    ui->reg_cancel->setEnabled(true);
    return;
}

void Autorization::registration_cancel()
{
    ui->first_name->clear();
    ui->name->clear();
    ui->last_name->clear();
    ui->login_user->clear();
    ui->password_user->clear();
    ui->reg_1->setEnabled( false );
    ui->reg_1->setVisible( false );
    ui->reg_2->setEnabled(false);
    ui->reg_2->setVisible(false);
    ui->Login_widget->setEnabled(true);
    ui->Login_widget->setVisible(true);
}
void Autorization::Next_page()
{
    ui->reg_1->setVisible(false);
    ui->reg_1->setEnabled(false);
    ui->reg_2->setVisible(true);
    ui->reg_2->setEnabled(true);
}
void Autorization::Prev_page()
{
    ui->reg_1->setVisible(true);
    ui->reg_1->setEnabled(true);
    ui->reg_2->setVisible(false);
    ui->reg_2->setEnabled(false);
}
