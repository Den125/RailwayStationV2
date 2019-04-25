#include "server.hpp"

Server::Server(QWidget *widget, QObject *parent) :QTcpServer(parent)
{
    _widget = widget;
    _blockSize =0;
}
QVector<User> Server::list_users()
{
    QVector<User> user;
    for (int i=0;i<_clients.size();i++)
    {
        user.append(_clients[i]->Return_User());
    }
    return user;
}
bool Server::doStartServer(QHostAddress addr, qint16 port)
{
    if (!listen(addr, port))
    {
        return false;
    }
    return true;
}
bool Server::CheckUser(QString login,QString password)
{
    if (emit Check(login,password))
        return true;
    return  false;
}
bool Server::Check_Sell(QString flight,QString carriage,QString date)
{
    if (emit Accept_Sell(flight,carriage,date))
        return true;
    return  false;
}
QByteArray Server::Data_Flights()
{
    return emit Filght_data();
}
User Server::User_Data(QString login,QString password)
{
    User user=emit DUser(login,password);
    emit addLogToGui(" Пользователь "+user.get_first_name()+" "+user.get_middle_name()+" "+user.get_last_name()+" был авторизован");
    return user;
}
void Server::RemoveUser(Client *cli)
{
    if (cli->retAutorize())
    {
        User user=cli->Return_User();
        emit addLogToGui(" Cоединение авторизованое как "
                        ""+user.get_first_name()+" "+user.get_middle_name()+" "+user.get_last_name()+" отсоединено");
    }
    else
    {
        emit addLogToGui(" Cоединение c дексриптором: "+QString::number(cli->DeskSocket())+" отсоединено");
    }
    _clients.removeOne(cli);
}
void Server::ToLog(QString string)
{
    emit addLogToGui(string);
}
void Server::ClientReady()
{

}
void Server::ClientDisc()
{
}
bool Server::RegisterUser(User user)
{
    return emit UserReg(user);
}
int Server::Number()
{
    return emit Number_Ticket();
}
bool Server::Return_Tick(QString Number_T,QString Number_F,QString Number_C,QString date)
{
    return emit return_ticket(Number_T, Number_F,Number_C, date);
}
void Server::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle,this);
    emit addLogToGui(" Запрошено соединение c дексриптором: "+QString::number(client->DeskSocket()));
    _clients.append(client);
    emit addLogToGui(" Соединение c дексриптором: "+QString::number(client->DeskSocket())+" добавлено в список");
    connect(client,SIGNAL(Autorize(QString ,QString )),this,SLOT(CheckUser(QString,QString)));
    connect(client,SIGNAL(Flights()),this,SLOT(Data_Flights()));
    connect(client,SIGNAL(user_activate(QString,QString)),this,SLOT(User_Data(QString,QString)));
    connect(client,SIGNAL(onDisconnected()),this,SLOT(ClientDisc()));
    connect(client,SIGNAL(removeUser(Client*)),this,SLOT(RemoveUser(Client*)));
    connect(client,SIGNAL(registerUser(User)),this,SLOT(RegisterUser(User)));
    connect(client,SIGNAL(Sell(QString,QString,QString)),this,SLOT(Check_Sell(QString,QString,QString)));
    connect(client,SIGNAL(Ticket_number()),this,SLOT(Number()));
    connect(client,SIGNAL(Return_Ticket(QString,QString,QString,QString)),this,SLOT(Return_Tick(QString,QString,QString,QString)));
    connect(client,SIGNAL(AddtoLog(QString)),this,SLOT(ToLog(QString)));
}
