#include "client.hpp"

Client::Client(qintptr handle, Server *serv,QWidget *parent):QDialog (parent)
{
    _serv=serv;
    Autorized=false;
    _widget=parent;
    _sok=new QTcpSocket(this);
    _sok->setSocketDescriptor(handle);
    connect(_sok, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(_sok, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(_sok, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onError(QAbstractSocket::SocketError)));
}
Client::Client(QWidget *parent)
{
    _widget=parent;
}
bool Client::retAutorize()
{
    return Autorized;
}
void Client::setAutrorize(bool stat)
{
    Autorized=stat;
}
bool Client::checkStatus()
{
    if (_sok->socketDescriptor()==-1)
        return false;
    return true;
}
int Client::DeskSocket()
{
    return _sok->socketDescriptor();
}
void Client::SetDesk(qintptr des)
{
    _sok->setSocketDescriptor(des);
}
void Client::onConnected()
{

}
void Client::onDisconnected()
{
    emit removeUser(this);
    deleteLater();
}
void Client::onError(QAbstractSocket::SocketError socketError) const
{
    QWidget w;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(&w, "Error", "The host was not found");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(&w, "Error", "The connection was refused by the peer.");
        break;
    default:
        QMessageBox::information(&w, "Error", "The following error occurred: "+_sok->errorString());
    }

}
void Client::onReadyRead()
{
    if (_sok->waitForConnected(1500))
    {
            if (!ComplexData)
            {
                Data=_sok->readAll();
            }
            else
            {
                Data.append(_sok->readAll());
                ComplexData=false;
            }
    JDoc=QJsonDocument::fromJson(Data,&JError);
    if (JError.errorString().toInt()==QJsonParseError::NoError)
    {
           if (JDoc.object().value("Command").toString()=="Autorize")
           {
               QString login,password;
                login=JDoc.object().value("Login").toString();
                password=JDoc.object().value("Password").toString();
                if (emit Autorize(login,password))
                {
                    user=emit user_activate(login,password);
                    Autorized=true;
                    QByteArray command="{\"Command\":\"Autorize\",\"result\":\"true\"}";
                    _sok->write(command);
                    _sok->waitForBytesWritten(500);
                }
                else
                {
                    QByteArray command="{\"Command\":\"Autorize\",\"result\":\"false\"}";
                    _sok->write(command);
                    _sok->waitForBytesWritten(500);
                }
           }
           else if (JDoc.object().value("Command").toString()=="DeAutorize")
           {
               emit AddtoLog(" Пользователь "+user.get_first_name()+" "+user.get_middle_name()+" "+
                            user.get_last_name()+" по дескриптору "+QString::number(_sok->socketDescriptor())+" вышел из системы");
               user.empty();
               Autorized=false;
           }
           else if (JDoc.object().value("Command").toString()=="Flight")
           {
               if (Autorized)
               {
                    if (JDoc.object().value("Param").toString()=="Length")
                    {
                        itog = emit Flights();
                        QByteArray command=("{\"Command\":\"Flight\",\"Length\":\""+QString::number(itog.length())+"\"}").toUtf8();
                        _sok->write(command);
                        _sok->waitForBytesWritten(500);
                    }
                    else if (JDoc.object().value("Param").toString()=="List")
                    {
                        _sok->write(itog);
                        _sok->waitForBytesWritten(500);
                    }
               }
               else
               {
                   QByteArray command="{\"Command\":\"System\",\"Status\":\"NOT_AUTHTORIZED\"}";
                   _sok->write(command);
                   _sok->waitForBytesWritten(500);
               }
           }
           else if (JDoc.object().value("Command").toString()=="User")
           {
               if (JDoc.object().value("Param").toString()=="Length")
               {
                    itog=("{\"Command\":\"Data_User\","
                         "\"Surname\":\""+user.get_first_name()+"\","
                         "\"Name\":\""+user.get_middle_name()+"\","
                         "\"Last_Name\":\""+user.get_last_name()+"\","
                         "\"Login\":\""+user.get_login()+"\","
                         "\"Password\":\""+user.get_pw()+"\","
                         "\"Role\":\""+user.get_role()+"\"}").toUtf8();
                    QByteArray command=("{\"Command\":\"User\",\"Length\":\""+QString::number(itog.length())+"\"}").toUtf8();
                    _sok->write(command);
                    _sok->waitForBytesWritten(500);
               }
               if (JDoc.object().value("Param").toString()=="Data")
               {
                   _sok->write(itog);
                   _sok->waitForBytesWritten(500);
               }
           }
           else if (JDoc.object().value("Command").toString()=="RegisterLength")
           {
               DataSize=JDoc.object().value("Length").toString().toInt();
               QByteArray command="{\"Command\":\"Register\",\"Param\":\"Data\"}";
               _sok->write(command);
               _sok->waitForBytesWritten(500);
           }
           else if (JDoc.object().value("Command").toString()=="Register")
           {
               User user;
               user.set_first_name(JDoc.object().value("Surname").toString());
               user.set_middle_name(JDoc.object().value("Name").toString());
               user.set_last_name(JDoc.object().value("Last_name").toString());
               user.set_login(JDoc.object().value("Login").toString());
               user.set_password(JDoc.object().value("Password").toString());
               user.set_Role(JDoc.object().value("Role").toString());
               if (emit registerUser(user))
               {
                   QByteArray command="{\"Command\":\"Register\",\"Param\":\"True\"}";
                   _sok->write(command);
                   _sok->waitForBytesWritten(500);
               }
               else
               {
                   QByteArray command="{\"Command\":\"Register\",\"Param\":\"False\"}";
                   _sok->write(command);
                   _sok->waitForBytesWritten(500);
               }
           }
           else if (JDoc.object().value("Command").toString()=="Ticket")
           {
               if (Autorized)
               {
                    if (emit Sell(JDoc.object().value("Flight").toString(),JDoc.object().value("Carriage").toString(),JDoc.object().value("Date").toString()))
                    {
                        int num=emit Ticket_number();
                        QByteArray command=("{\"Command\":\"Sell\",\"result\":\"true\",\"Number\":\""+QString::number(num)+"\"}").toUtf8();
                        _sok->write(command);
                        _sok->waitForBytesWritten(500);
                    }
                    else
                    {
                        QByteArray command="{\"Command\":\"Sell\",\"result\":\"false\"}";
                        _sok->write(command);
                        _sok->waitForBytesWritten(500);
                    }
               }
               else
               {
                   QByteArray command="{\"Command\":\"System\",\"Status\":\"NOT_AUTHTORIZED\"}";
                   _sok->write(command);
                   _sok->waitForBytesWritten(500);
               }
           }
           else if (JDoc.object().value("Command").toString()=="Return_ticket")
           {
               if (Autorized)
               {
                    QString number_t,number_f,number_c,date;
                    number_t=JDoc.object().value("Ticket").toString();
                    number_f=JDoc.object().value("Flight").toString();
                    number_c=JDoc.object().value("Carriage").toString();
                    date=JDoc.object().value("Date").toString();
                    if (emit Return_Ticket(number_t,number_f,number_c,date))
                    {
                        QByteArray command="{\"Command\":\"Return\",\"result\":\"true\"}";
                        _sok->write(command);
                        _sok->waitForBytesWritten(500);
                    }
                    else
                    {
                        QByteArray command="{\"Command\":\"Return\",\"result\":\"false\"}";
                        _sok->write(command);
                        _sok->waitForBytesWritten(500);
                    }
               }
               else
               {
                   QByteArray command="{\"Command\":\"System\",\"Status\":\"NOT_AUTHTORIZED\"}";
                   _sok->write(command);
                   _sok->waitForBytesWritten(500);
               }

           }
           else
           {
                ComplexData=true;
                onReadyRead();
           }
        }

    }
}
void Client::Set_User(User _user)
{
    user=_user;
}
User Client::Return_User()
{
    return user;
}
