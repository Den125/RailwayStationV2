#include "server.hpp"

Server::Server(QWidget *widget, QObject *parent) :QTcpServer(parent)
{
    _widget = widget;
    _blockSize =0;
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
        emit addLogToGui(" Cоединение c дексриптором: "+QString::number(cli->DeskSocket())+" авторизованое как "
                        ""+user.get_first_name()+" "+user.get_middle_name()+" "+user.get_last_name()+" отсоединено");
    }
    else
    {
        emit addLogToGui(" Cоединение c дексриптором: "+QString::number(cli->DeskSocket())+" отсоединено");
    }
    _clients.removeOne(cli);
}
void Server::ClientReady()
{

}
void Server::ClientDisc()
{
    qDebug()<<"Disconnect";
}
bool Server::RegisterUser(User user)
{
    return emit UserReg(user);
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
    connect(client,SIGNAL(disconnect()),this,SLOT(ClientDisc()));
    connect(client,SIGNAL(removeUser(Client*)),this,SLOT(RemoveUser(Client*)));
    connect(client,SIGNAL(registerUser(User)),this,SLOT(RegisterUser(User)));
}
