#include "gui.hpp"
#include "ui_gui.h"

Gui::Gui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Gui)
{
    ui->setupUi(this);
    db = new Database(this);
    connect(db,SIGNAL(TableStat(QString)),this,SLOT(addToLog(QString)));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    ui->Settings->setVisible(false);
    ui->Flight_settings->setVisible(false);
    ui->Carriage_settings->setVisible(false);
    ui->User_settings->setVisible(false);
    QRegExp MASKTHREE ( "[0-9][0-9][0-9]" );
    QRegExp MASKFOUR  ( "[0-9][0-9][0-9][0-9]" );
    QRegExp MASKFIVE  ( "[0-9][0-9][0-9][0-9][0-9]" );
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ip = new QRegExpValidator(ipRegex, this);
    QRegExpValidator *VALU  = new QRegExpValidator( MASKTHREE, this );
    QRegExpValidator *VAL   = new QRegExpValidator( MASKFOUR, this );
    QRegExpValidator *VALUE = new QRegExpValidator( MASKFIVE, this );
    ui->Number_flight->setValidator( VAL );
    ui->Number_carriage->setValidator(VALUE);
    ui->Place_carriage->setValidator( VALU );
    ui->Value->setValidator( VALUE );
    ui->Host->setValidator(ip);
    ui->DateTime->setDateTime( QDateTime::currentDateTime() );
    timer = new QTimer;
    timer->start( 30000 );
    QObject::connect( timer, SIGNAL(timeout()), this, SLOT(timer_overflow()) );
    connect(ui->DeleteFlight,SIGNAL(clicked()),this,SLOT(DeleteFlight()));
    connect(ui->DeleteCarriage,SIGNAL(clicked()),this,SLOT(DeleteCarriage()));
    connect(ui->DeleteUser,SIGNAL(clicked()),this,SLOT(DeleteUser()));
    connect(ui->Database_Connect,SIGNAL(triggered()),this,SLOT(ConnectDatabase()));
    connect(ui->AddFlight,SIGNAL(clicked()),this,SLOT(AddFlight()));
    connect(ui->EditFlight,SIGNAL(clicked()),this,SLOT(EditFlight()));
    connect(ui->Cancel_flight,SIGNAL(clicked()),this,SLOT(CancelFlight()));
    connect(ui->AddUser,SIGNAL(clicked()),this,SLOT(AddUser()));
    connect(ui->EditUser,SIGNAL(clicked()),this,SLOT(EditUser()));
    connect(ui->Cancel_user,SIGNAL(clicked()),this,SLOT(CancelUser()));
    connect(ui->AddCarriage,SIGNAL(clicked()),this,SLOT(AddCarriage()));
    connect(ui->EditCarriage,SIGNAL(clicked()),this,SLOT(EditCarriage()));
    connect(ui->Cancel_carriage,SIGNAL(clicked()),this,SLOT(CancelCarriage()));
    connect(ui->add_filght,SIGNAL(clicked()),this,SLOT(AddFlightToDatabase()));
    connect(ui->Add_carriage,SIGNAL(clicked()),this,SLOT(AddCarriageToDatabase()));
    connect(ui->Add_User,SIGNAL(clicked()),this,SLOT(AddUserToDatabase()));
    connect(ui->Update_flight,SIGNAL(clicked()),this,SLOT(EditFlightToDatabase()));
    connect(ui->Update_carriage,SIGNAL(clicked()),this,SLOT(EditCarriageToDatabase()));
    connect(ui->Update_user,SIGNAL(clicked()),this,SLOT(EditUserToDatabase()));
    connect(ui->Connect ,SIGNAL(clicked()),this,SLOT(Connect()));
    connect(ui->add,SIGNAL(clicked()),this,SLOT(AddCarriageToFlight()));
    connect(ui->remove,SIGNAL(clicked()),this,SLOT(RemoveCarriageFromFlight()));
    connect(ui->Days,SIGNAL(activated(int)),this,SLOT(SetCarriage(int)));
    connect(ui->Close,SIGNAL(clicked()),this,SLOT(CloseConnectCarriage()));
    connect(ui->Logout,SIGNAL(triggered()),this,SLOT(Logout()));
    connect(ui->Exit,SIGNAL(triggered()),this,SLOT(Exit()));
    connect(ui->Days ,SIGNAL(activated(int)),this,SLOT(SetDate(int)));
    QString fileName =("system.conf");
    QFile conf(fileName);
    if (!conf.exists())
    {
        conf.open(QIODevice::WriteOnly);
        QTextStream create(&conf);
        create<<tr("База: ")<<"DEN-PC\\IKIT";
        if ( create.status() == QTextStream::WriteFailed )
        {
            throw std::runtime_error( tr( "Write to the stream failed") .toStdString() );
        }
        conf.close();
    }
    autorize=new Autorization();
    connect(autorize,SIGNAL(Autorize(QString,QString)),this,SLOT(CheckAdm(QString,QString)));
}
bool Gui::ConnectDatabase()
{
    QFile conf("system.conf");
    conf.open(QIODevice::ReadOnly);
    QTextStream load(&conf);
    QString str,host;
    load>>str>>host;
    conf.close();
    if (db->createConnection(host)==false)
    {
        ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Ошибка: отсутствует соединение с базой данных");
        QMessageBox real( this );
        real.setWindowTitle( tr( "Ошибка" ) );
        real.setIcon( QMessageBox::Critical );
        real.setText( tr( "Ошбика: Нет соединения с базой данных!" ) );
        real.exec();
        return false;
    }
    ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Проверка таблиц");
    if (db->CheckTableUsers()==false)
    {
        ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Ошибка: Не обнаружена таблица Users");
        QMessageBox real( this );
        real.setWindowTitle( tr( "Ошибка" ) );
        real.setIcon( QMessageBox::Critical );
        real.setText( tr( "Ошбика: Невозможно создать одну или несколько таблиц" ) );
        real.exec();
        return false;
    }
    CreateModelUsers(QStringList() << "id" <<"Фамилия"<< "Имя" << "Отчество" << "Логин"
                       << "Пароль" << "Роль");
    if (db->CheckTableFlights()==false)
    {
        ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Ошибка: Не обнаружена таблица Flights");
        QMessageBox real( this );
        real.setWindowTitle( tr( "Ошибка" ) );
        real.setIcon( QMessageBox::Critical );
        real.setText( tr( "Ошбика: Невозможно создать одну или несколько таблиц" ) );
        real.exec();
        return false;
    }
    CreateModelFlights(QStringList() <<"Номер"<< "Место отправления"<< "Место назначения"
                         << "Дни следования" << "Время отправления");
    if (db->CheckTableCarriages()==false)
    {
        ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Ошибка: Не обнаружена таблица Carriage");
        QMessageBox real( this );
        real.setWindowTitle( tr( "Ошибка" ) );
        real.setIcon( QMessageBox::Critical );
        real.setText( tr( "Ошбика: Невозможно создать одну или несколько таблиц" ) );
        real.exec();
        return false;
    }  CreateModelCarriages(QStringList() << "Номер" << "Тип" << "Всего мест"
                            << "Свободно мест" << "Номер рейса" <<"День следования"<<"Дата следования"<< "Стоимость");

    if (db->CheckTableTickets()==false)
    {
        ui->lwLog->insertItem(0, QTime::currentTime().toString()+" Ошибка: Не обнаружена таблица Tickets");
        QMessageBox real( this );
        real.setWindowTitle( tr( "Ошибка" ) );
        real.setIcon( QMessageBox::Critical );
        real.setText( tr( "Ошбика: Невозможно создать одну или несколько таблиц" ) );
        real.exec();
        return false;
    }
    ui->lwLog->insertItem(0, QTime::currentTime().toString()+" База данных подключена");
    ui->Database_Connect->setEnabled(false);
    return true;
}
void Gui::on_button_StartStop_toggled(bool checked)
{
    if (checked)
    {
        addToLog(" сервер остановлен на "+_serv->serverAddress().toString()+":"+QString::number(_serv->serverPort()));
        _serv->close();
        ui->button_StartStop->setText("Открыть");
    }
    else
    {
        QHostAddress addr;
        if (!addr.setAddress(ui->Host->text()))
        {
            addToLog(" Неверный адрес "+ui->Host->text());
            return;
        }
        if (_serv->doStartServer(addr, ui->Port->text().toInt()))
        {
            addToLog(" сервер запущен на "+ui->Host->text()+":"+ui->Port->text());
            ui->button_StartStop->setText("Остановить");
        }
        else
        {
            addToLog(" сервер не был запущен на "+ui->Host->text()+":"+ui->Port->text());
            ui->button_StartStop->setChecked(true);
        }
    }
}
void Gui::addToLog(QString text)
{
    ui->lwLog->insertItem(0, QTime::currentTime().toString()+text);
}
void Gui::CreateModelUsers(const QStringList &headers)
{
    model_user = new QSqlTableModel;
    model_user->setTable("Users");

    for(int i = 0, j = 0; i < model_user->columnCount(); i++, j++)
            model_user->setHeaderData(i,Qt::Horizontal,headers[j]);
    ui->Database_Users->setModel(model_user);
    ui->Database_Users->setColumnHidden(0,true);
    ui->Database_Users->resizeColumnsToContents();
    model_user->select();
}
void Gui::CreateModelFlights(const QStringList &headers)
{
    model_flight = new QSqlTableModel;
    model_flight->setTable("Flights");

    for(int i = 0, j = 0; i < model_flight->columnCount(); i++, j++)
             model_flight->setHeaderData(i,Qt::Horizontal,headers[j]);
    ui->Database_Flight->setModel( model_flight);
    ui->Database_Flight->resizeColumnsToContents();
    model_flight->select();
}
void Gui::CreateModelCarriages(const QStringList &headers)
{
    model_carriage = new QSqlTableModel;
    model_carriage->setTable("carriages");

    for(int i = 0, j = 0; i < model_carriage->columnCount(); i++, j++)
            model_carriage->setHeaderData(i,Qt::Horizontal,headers[j]);
    ui->Database_carriage->setModel(model_carriage);
    ui->Database_carriage->resizeColumnsToContents();
    model_carriage->select();
}
bool Gui::Check_User(QString login,QString password)
{
    for (int i=0;i<model_user->rowCount();i++)
    {
        const QModelIndex idl=model_user->index(i,4);
        const QModelIndex idp=model_user->index(i,5);
        if (login==model_user->data(idl) && password==model_user->data(idp))
        {
            QVector<User> users=_serv->list_users();
            for (int i=0;i<users.length();i++)
            {
                if (login==users[i].get_login()&&password==users[i].get_pw())
                    return false;

            }
        }
        return true;
    }
    return false;
}
User Gui::Return_User(QString login,QString password)
{
    User user;
    for (int i=0;i<model_user->rowCount();i++)
    {
        const QModelIndex idl=model_user->index(i,4);
        const QModelIndex idp=model_user->index(i,5);
        if (login==model_user->data(idl) && password==model_user->data(idp))
        {
            user.set_first_name((model_user->data(model_user->index(i,1))).toString());
            user.set_middle_name((model_user->data(model_user->index(i,2))).toString());
            user.set_last_name((model_user->data(model_user->index(i,3))).toString());
            user.set_login(login);
            user.set_password(password);
            user.set_Role((model_user->data(model_user->index(i,6))).toString());
        }
    }
    return user;
}
bool Gui::OpenServer()
{
   _serv = new Server(this, this);
   connect(_serv,SIGNAL(Check(QString,QString)),this,SLOT(Check_User(QString,QString)));
   connect(_serv,SIGNAL(addLogToGui(QString)),this,SLOT(addToLog(QString)));
   connect(_serv,SIGNAL(Filght_data()),this,SLOT(Data_Flight()));
   connect(_serv,SIGNAL(DUser(QString,QString)),this,SLOT(Return_User(QString,QString)));
   connect(_serv,SIGNAL(UserReg(User)),this,SLOT(AddUser(User)));
   connect(_serv,SIGNAL(Accept_Sell(QString,QString,QString)),this,SLOT(Sell(QString,QString,QString)));
   connect(_serv,SIGNAL(Number_Ticket()),this,SLOT(Number()));
   connect(_serv,SIGNAL(return_ticket(QString,QString,QString,QString)),this,SLOT(return_Ticket(QString,QString,QString,QString)));
   if (_serv->doStartServer(QHostAddress::LocalHost, 1234))
   {
       ui->lwLog->insertItem(0, QTime::currentTime().toString()+" сервер запущен на "+_serv->serverAddress().toString()+":"+QString::number(_serv->serverPort()));
       ui->button_StartStop->setText("Остановить");
       return true;
   }
   else
   {
       ui->lwLog->insertItem(0, QTime::currentTime().toString()+" сервер не был запущен "+_serv->serverAddress().toString()+":"+QString::number(_serv->serverPort()));
       ui->button_StartStop->setChecked(true);
       ui->button_StartStop->setText("Открыть");
       return false;
   }

}
QByteArray Gui::Data_Flight()
{
    QByteArray DFlight="{\"Command\":\"List_Flight\",\"Flight\":[";
    for (int i=0;i<model_flight->rowCount();i++)
    {
        if (i!=0) DFlight.append(",");
        DFlight.append("{");
        const QModelIndex number=model_flight->index(i,0);
        const QModelIndex start=model_flight->index(i,1);
        const QModelIndex finish=model_flight->index(i,2);
        const QModelIndex date=model_flight->index(i,3);
        const QModelIndex time=model_flight->index(i,4);
        DFlight=DFlight+(("\"Number\":\""+model_flight->data(number).toString()+"\","
                        "\"Start\":\""+model_flight->data(start).toString()+"\","
                        "\"Finish\":\""+model_flight->data(finish).toString()+"\","
                        "\"Days_all\":\""+model_flight->data(date).toString()+"\","
                        "\"Time\":\""+model_flight->data(time).toString()+"\"").toUtf8());
        QStringList days=model_flight->data(date).toString().split(",");
        DFlight.append(",\"On_days\":[");
        for (int i=0,l=0;i<days.length();i++)
        {
            if (l!=0) DFlight.append(",");
            DFlight.append("{\"Day\":\""+days[i]+"\"");
            DFlight.append(",\"Carriage\":[");
            for (int j=0,k=0;j<model_carriage->rowCount();j++)
            {
                const QModelIndex numf=model_carriage->index(j,4);
                const QModelIndex dayf=model_carriage->index(j,5);
                if ((model_carriage->data(numf).toString().toInt()==model_flight->data(number).toInt())&&(model_carriage->data(dayf).toString()==days[i]))
                {
                    if (k!=0) DFlight.append(",");
                    DFlight.append("{");
                    const QModelIndex num=model_carriage->index(j,0);
                    const QModelIndex type=model_carriage->index(j,1);
                    const QModelIndex all=model_carriage->index(j,2);
                    const QModelIndex free=model_carriage->index(j,3);
                    const QModelIndex value=model_carriage->index(j,7);
                    DFlight.append(("\"Num\":\""+model_carriage->data(num).toString()+"\","
                                    "\"Type\":\""+model_carriage->data(type).toString()+"\","
                                    "\"All\":\""+model_carriage->data(all).toString()+"\","
                                    "\"Free\":\""+model_carriage->data(free).toString()+"\","
                                    "\"Value\":\""+QString::number(model_carriage->data(value).toInt())+"\"}").toUtf8());
                    k++;
                }
            }
            l++;
            DFlight.append("]}");
        }
        DFlight.append("]}");
    }
    DFlight.append("]}");
    return DFlight;
}
bool Gui::AddUser(User user)
{
    for (int i=0;i<model_user->rowCount();i++)
        if (model_user->data(model_user->index(i,4))==user.get_login())
            return false;
    if (db->AddToDataBase(user))
    {
       model_user->select();
       return true;
    }
    else
        return false;
}
void Gui::DeleteFlight()
{
    if (ui->Database_Flight->selectionModel()->hasSelection()==false)
        return ;
    QMessageBox real( this );
    real.setWindowTitle( tr( "Подтверждение" ) );
    real.setIcon( QMessageBox::Warning );
    real.setText( tr( "Вы действительно хотите удалить выбранный рейс" ) );    
    real.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    real.setDefaultButton(QMessageBox::No);
    switch(real.exec())
    {
        case QMessageBox::Yes:
        {
            int x=ui->Database_Flight->selectionModel()->currentIndex().row();
            int num=model_flight->data(model_flight->index(x,0)).toInt();
            if (db->DeleteFlight(num))
            {
                addToLog(" Рейс с номером "+QString::number(num)+" был успешно удален");
                model_flight->select();
            }
            else
                addToLog(" Ошибка удаления рейса "+QString::number(num));


        }
        default:
        {
            return;
        }
    }
}
void Gui::DeleteCarriage()
{
    if (ui->Database_carriage->selectionModel()->hasSelection()==false)
        return ;
    QMessageBox real( this );
    real.setWindowTitle( tr( "Подтверждение" ) );
    real.setIcon( QMessageBox::Warning );
    real.setText( tr( "Вы действительно хотите удалить выбранный вагон" ) );
    real.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    real.setDefaultButton(QMessageBox::No);
    switch(real.exec())
    {
        case QMessageBox::Yes:
        {
            int x=ui->Database_carriage->selectionModel()->currentIndex().row();
            int num=model_carriage->data(model_carriage->index(x,0)).toInt();
            if (db->DeleteCarraige(num))
            {
                addToLog(" Вагон с номером "+QString::number(num)+" был успешно удален");
                model_carriage->select();
            }
            else
                addToLog(" Ошибка удаления вагона "+QString::number(num));
        }
        case QMessageBox::No:
        {
            return;
        }
    }
}
void Gui::DeleteUser()
{
    if (ui->Database_Users->selectionModel()->hasSelection()==false)
        return ;
    QMessageBox real( this );
    real.setWindowTitle( tr( "Подтверждение" ) );
    real.setIcon( QMessageBox::Warning );
    real.setText( tr( "Вы действительно хотите удалить выбранного пользователя" ) );
    real.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    real.setDefaultButton(QMessageBox::No);
    switch(real.exec())
    {
        case QMessageBox::Yes:
        {
            int x=ui->Database_Users->selectionModel()->currentIndex().row();
            if (db->DeleteUser(model_user->data(model_user->index(x,4)).toString())==false)
            {
                QMessageBox error( this );
                error.setWindowTitle( tr( "Ошибка" ) );
                error.setIcon( QMessageBox::Critical);
                error.setText( tr( "Ошибка: Невозможно удалить пользователя!" ) );
                error.exec();
                return;
            }
            else
                model_user->select();

        }
        default:
        {
            return;
        }
    }
}
void Gui::AddFlight()
{
    Enable_Flight_Settings();
    ui->Update_flight->setVisible(false);
    ui->add_filght->setVisible(true);
    ui->Number_flight->setEnabled(true);
}
void Gui::AddCarriage()
{
    Enable_Carriage_Settings();
    ui->Update_carriage->setVisible(false);
    ui->Add_carriage->setVisible(true);
    ui->Number_carriage->setEnabled(true);
}
void Gui::AddUser()
{
    Enable_User_Settings();
    ui->Update_user->setVisible(false);
    ui->Add_User->setVisible(true);
    ui->Login_user->setEnabled(true);
}
void Gui::EditFlight()
{
    if (ui->Database_Flight->selectionModel()->hasSelection()==false)
        return ;
    Enable_Flight_Settings();
    ui->add_filght->setVisible(false);
    ui->Update_flight->setVisible(true);
    int x=ui->Database_Flight->selectionModel()->currentIndex().row();
    ui->Number_flight->setText(QString::number(model_flight->data(model_flight->index(x,0)).toInt()));
    ui->Number_flight->setEnabled(false);
    ui->Start_flight->setText(model_flight->data(model_flight->index(x,1)).toString());
    ui->Finish_flight->setText(model_flight->data(model_flight->index(x,2)).toString());
    ui->Time_flight->setTime(model_flight->data(model_flight->index(x,4)).toTime());
    QStringList list=(model_flight->data(model_flight->index(x,3)).toString()).split(',');
    for (int i=0;i<list.length();i++)
    {
        switch (list[i].toInt())
        {
            case 1:
            {
                ui->Monday->setChecked(true);
                break;
            }
            case 2:
            {
                ui->Thursday->setChecked(true);
                break;
            }
            case 3:
            {
                ui->Wensday->setChecked(true);
                break;
            }
            case 4:
            {
                ui->Thirsday->setChecked(true);
                break;
            }
            case 5:
            {
                ui->Friday->setChecked(true);
                break;
            }
            case 6:
            {
                ui->Saturday->setChecked(true);
                break;
            }
            case 7:
            {
                ui->Sunday->setChecked(true);
                break;
            }
        }
    }
}
void Gui::EditCarriage()
{
    if (ui->Database_carriage->selectionModel()->hasSelection()==false)
       return;
    Enable_Carriage_Settings();
    ui->Add_carriage->setVisible(false);
    ui->Update_carriage->setVisible(true);
    ui->Number_carriage->setEnabled(false);
    int x=ui->Database_carriage->selectionModel()->currentIndex().row();
    ui->Number_carriage->setText(model_carriage->data(model_carriage->index(x,0)).toString());
    ui->Place_carriage->setText(QString::number(model_carriage->data(model_carriage->index(x,2)).toInt()));
    ui->Value->setText(QString::number(model_carriage->data(model_carriage->index(x,6)).toInt()));
    ui->Type_carriage->setCurrentText(model_carriage->data(model_carriage->index(x,1)).toString());
}
void Gui::EditUser()
{
    if (ui->Database_Users->selectionModel()->hasSelection()==false)
       return;
    Enable_User_Settings();
    ui->Add_User->setVisible(false);
    ui->Update_user->setVisible(true);
    ui->Login_user->setEnabled(false);
    int x=ui->Database_Users->selectionModel()->currentIndex().row();
    ui->First_name->setText(model_user->data(model_user->index(x,1)).toString());
    ui->Name->setText(model_user->data(model_user->index(x,2)).toString());
    ui->Last_name->setText(model_user->data(model_user->index(x,3)).toString());
    ui->Login_user->setText(model_user->data(model_user->index(x,4)).toString());
    ui->Role_user->setCurrentText(model_user->data(model_user->index(x,6)).toString());
}
void Gui::CancelFlight()
{
    ui->Number_flight->clear();
    ui->Start_flight->clear();
    ui->Finish_flight->clear();
    ui->Time_flight->clear();
    ui->Monday->setChecked(false);
    ui->Thursday->setChecked(false);
    ui->Wensday->setChecked(false);
    ui->Thirsday->setChecked(false);
    ui->Friday->setChecked(false);
    ui->Saturday->setChecked(false);
    ui->Sunday->setChecked(false);
    ui->lwLog->setVisible(true);
    ui->Settings->setVisible(false);
    ui->Flight_settings->setVisible(false);
    ui->Flight_settings->setEnabled(false);
}
void Gui::CancelCarriage()
{
    ui->Number_carriage->clear();
    ui->Place_carriage->clear();
    ui->Value->clear();
    ui->Type_carriage->clear();
    ui->lwLog->setVisible(true);
    ui->Carriage_settings->setVisible(false);
    ui->Carriage_settings->setEnabled(false);
    ui->Settings->setVisible(false);
}
void Gui::CancelUser()
{
    ui->First_name->clear();
    ui->Name->clear();
    ui->Last_name->clear();
    ui->Login_user->clear();
    ui->Password_user->clear();
    ui->Role_user->clear();
    ui->lwLog->setVisible(true);
    ui->User_settings->setVisible(false);
    ui->User_settings->setEnabled(false);
    ui->Settings->setVisible(false);
}
void Gui::Enable_Flight_Settings()
{
    ui->lwLog->setVisible(false);
    ui->Settings->setVisible(true);
    ui->Settings->setCurrentIndex(0);
    ui->Flight_settings->setVisible(true);
    ui->Carriage_settings->setVisible(false);
    ui->User_settings->setVisible(false);
    ui->Carriage_toFlight_settings->setVisible(false);
    ui->Flight_settings->setEnabled(true);
    ui->Carriage_settings->setEnabled(false);
    ui->User_settings->setEnabled(false);
    ui->Carriage_toFlight_settings->setEnabled(false);

}
void Gui::Enable_Carriage_Settings()
{
    ui->lwLog->setVisible(false);
    ui->Settings->setVisible(true);
    ui->Settings->setCurrentIndex(1);
    ui->Flight_settings->setVisible(false);
    ui->Carriage_settings->setVisible(true);
    ui->User_settings->setVisible(false);
    ui->Carriage_toFlight_settings->setVisible(false);
    ui->Flight_settings->setEnabled(false);
    ui->Carriage_settings->setEnabled(true);
    ui->User_settings->setEnabled(false);
    ui->Carriage_toFlight_settings->setEnabled(false);
}
void Gui::Enable_User_Settings()
{
    ui->lwLog->setVisible(false);
    ui->Settings->setVisible(true);
    ui->Settings->setCurrentIndex(2);
    ui->Carriage_settings->setVisible(false);
    ui->User_settings->setVisible(true);
    ui->Carriage_toFlight_settings->setVisible(false);
    ui->Flight_settings->setEnabled(false);
    ui->Carriage_settings->setEnabled(false);
    ui->User_settings->setEnabled(true);
    ui->Carriage_toFlight_settings->setEnabled(false);
}
void Gui::Enable_Connect_Settings()
{
    ui->lwLog->setVisible(false);
    ui->Settings->setVisible(true);
    ui->Settings->setCurrentIndex(3);
    ui->Carriage_settings->setVisible(false);
    ui->User_settings->setVisible(false);
    ui->Carriage_toFlight_settings->setVisible(true);
    ui->Flight_settings->setEnabled(false);
    ui->Carriage_settings->setEnabled(false);
    ui->User_settings->setEnabled(false);
    ui->Carriage_toFlight_settings->setEnabled(true);
    ui->tabWidget->setVisible(false);
}
void Gui::AddFlightToDatabase()
{
    if ( ui->Number_flight->text().toInt() == 0 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поле <Номер> не может быть пустым или равняться 0!" ) );
        error.exec();
        return;
    }
    if ( ui->Start_flight->text() == NULL || ui->Finish_flight->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поля <Место отправления> или/и <Место назначения> не могут быть пустыми" ) );
        error.exec();
        return;
    }
    Flight temp;
    temp.set_Number(ui->Number_flight->text().toInt());
    for (int i=0;i<model_flight->rowCount();i++)
        if (model_flight->data(model_flight->index(i,0)).toString().toInt()==temp.Number())
        {
            QMessageBox error( this );
            error.setWindowTitle( tr( "Ошибка") );
            error.setIcon( QMessageBox::Warning );
            error.setText( tr( "Извините. Рейс с таким номером уже существует!" ) );
            error.exec();
            return;
        }
    temp.set_Start(ui->Start_flight->text());
    temp.set_Finish(ui->Finish_flight->text());
    temp.set_Time(QTime::fromString(ui->Time_flight->text()));
    QString days;
    if (ui->Monday->isChecked()==true)
        days=days+"1,";
    if (ui->Thursday->isChecked()==true)
        days=days+"2,";
    if (ui->Wensday->isChecked()==true)
        days=days+"3,";
    if (ui->Thirsday->isChecked()==true)
        days=days+"4,";
    if (ui->Friday->isChecked()==true)
        days=days+"5,";
    if (ui->Saturday->isChecked()==true)
        days=days+"6,";
    if (ui->Sunday->isChecked()==true)
        days=days+"7,";
    if (days.length()==0)
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка") );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Выберите хотя бы один день следования!" ) );
        error.exec();
        return;
    }
    days.remove(days.length()-1,1);
    temp.set_Days(days);
    if (db->AddToDataBase(temp)==true)
    {
        addToLog(" Рейс с номером "+QString::number(temp.Number())+" был добавлен в базу данных");
        model_flight->select();
        CancelFlight();
    }
    else
    {
        addToLog(" Ошибка добавления рейса "+QString::number(temp.Number()));
        CancelFlight();
    }
}
void Gui::AddCarriageToDatabase()
{
    if ( ui->Number_carriage->text().toInt() == 0 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поле <Номер> не может быть пустым или равняться 0!" ) );
        error.exec();
        return;
    }
    if ( ui->Place_carriage->text() == NULL || ui->Value->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Количество мест> и/или <Стоимость> не могут быть пустыми!") );
        error.exec();
        return;
    }

    if ( ui->Place_carriage->text().toInt() == 0 || ui->Value->text().toInt() == 0 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Количество мест> и/или <Стоимость> не могут равняться 0!" ) );
        error.exec();
        return;
    }
    Carriage carri;
    carri.set_Number(ui->Number_carriage->text().toInt());
    carri.set_Type(ui->Type_carriage->currentText());
    if ( carri.type() == "Ресторан" )
    {
        carri.set_place_all(0);
        carri.set_place_free(0);
        carri.set_Value(0);
    }
    else
    {
        carri.set_place_all(ui->Place_carriage->text().toInt());
        carri.set_place_free(ui->Place_carriage->text().toInt());
        carri.set_Value(ui->Value->text().toInt());
    }
    if (db->AddToDataBase(carri))
    {
        addToLog(" Вагон с номером "+QString::number(carri.Number_carriage())+" был добавлен в базу данных");
        model_carriage->select();
        CancelCarriage();
    }
    else
    {
        addToLog(" Ошибка добавления вагона "+QString::number(carri.Number_carriage()));
        CancelCarriage();
    }
}
void Gui::AddUserToDatabase()
{
    if ( ui->Login_user->text() == NULL || ui->Password_user->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поля <Логин> и/или <Пароль> не могут быть пустым!" ) );
        error.exec();
        return;
    }
    if ( ui->First_name->text() == NULL || ui->Last_name->text() == NULL || ui->Name->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Вы должны ввести данные пользователя!" ) );
        error.exec();
        return;
    }
    if ( ui->Login_user->text().size() < 5 || ui->Password_user->text().size() < 5 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Логин> и/или <Пароль> не могут быть меньше 5 символов!" ) );
        error.exec();
        return;
    }
    User user;
    user.set_first_name(ui->First_name->text());
    user.set_middle_name(ui->Name->text());
    user.set_last_name(ui->Last_name->text());
    user.set_login(ui->Login_user->text());
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(ui->Password_user->text().toUtf8());
    QString hashStr(hash.result().toHex());
    user.set_password(hashStr);
    user.set_Role(ui->Role_user->currentText());
    if (db->AddToDataBase(user)==true)
    {
        addToLog(" Пользователь с логином "+user.get_login()+" был добавлен в базу данных");
        model_user->select();
        CancelUser();
    }
    else
    {
        addToLog(" Ошибка добавления пользователя  "+user.get_login());
        CancelUser();
    }
}
void Gui::EditFlightToDatabase()
{
    if ( ui->Start_flight->text() == NULL || ui->Finish_flight->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поля <Место отправления> или/и <Место назначения> не могут быть пустыми" ) );
        error.exec();
        return;
    }
    Flight temp;
    temp.set_Number(ui->Number_flight->text().toInt());
    temp.set_Start(ui->Start_flight->text());
    temp.set_Finish(ui->Finish_flight->text());
    temp.set_Time(QTime::fromString(ui->Time_flight->text()));
    QString days;
    if (ui->Monday->isChecked()==true)
        days=days+"1,";
    if (ui->Thursday->isChecked()==true)
        days=days+"2,";
    if (ui->Wensday->isChecked()==true)
        days=days+"3,";
    if (ui->Thirsday->isChecked()==true)
        days=days+"4,";
    if (ui->Friday->isChecked()==true)
        days=days+"5,";
    if (ui->Saturday->isChecked()==true)
        days=days+"6,";
    if (ui->Sunday->isChecked()==true)
        days=days+"7,";
    if (days.length()==0)
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка") );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Выберите хотя бы один день следования!" ) );
        error.exec();
        return;
    }
    days.remove(days.length()-1,1);
    temp.set_Days(days);
    if (db->EditData(temp))
    {
        addToLog(" Рейс с номером "+QString::number(temp.Number())+" был обновлен");
        model_flight->select();
        CancelFlight();
    }
    else
    {
        addToLog(" Ошибка обновления данных рейса "+QString::number(temp.Number()));
        CancelFlight();
    }

}
void Gui::EditCarriageToDatabase()
{
    if ( ui->Place_carriage->text() == NULL || ui->Value->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Количество мест> и/или <Стоимость> не могут быть пустыми!") );
        error.exec();
        return;
    }

    if ( ui->Place_carriage->text().toInt() == 0 || ui->Value->text().toInt() == 0 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Количество мест> и/или <Стоимость> не могут равняться 0!" ) );
        error.exec();
        return;
    }
    Carriage carri;
    carri.set_Number(ui->Number_carriage->text().toInt());
    carri.set_Type(ui->Type_carriage->currentText());
    if ( carri.type() == "Ресторан" )
    {
        carri.set_place_all(0);
        carri.set_place_free(0);
        carri.set_Value(0);
    }
    else
    {
        carri.set_place_all(ui->Place_carriage->text().toInt());
        carri.set_place_free(ui->Place_carriage->text().toInt());
        carri.set_Value(ui->Value->text().toInt());
    }

    if (db->EditData(carri))
    {
        addToLog(" Вагон с номером "+QString::number(carri.Number_carriage())+" был обновлен");
        model_carriage->select();
        CancelCarriage();
    }
    else
    {
        addToLog(" Ошбика обновления данных вагона "+QString::number(carri.Number_carriage()));
        CancelCarriage();
    }
}
void Gui::EditUserToDatabase()
{
    if ( ui->Login_user->text() == NULL || ui->Password_user->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините.  Поля <Логин> и/или <Пароль> не могут быть пустым!" ) );
        error.exec();
        return;
    }
    if ( ui->First_name->text() == NULL || ui->Last_name->text() == NULL || ui->Name->text() == NULL )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Вы должны ввести данные пользователя!" ) );
        error.exec();
        return;
    }
    if ( ui->Login_user->text().size() < 5 || ui->Password_user->text().size() < 5 )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setIcon( QMessageBox::Warning );
        error.setText( tr( "Извините. Поля <Логин> и/или <Пароль> не могут быть меньше 5 символов!" ) );
        error.exec();
        return;
    }
    User user;
    user.set_first_name(ui->First_name->text());
    user.set_middle_name(ui->Name->text());
    user.set_last_name(ui->Last_name->text());
    user.set_login(ui->Login_user->text());
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(ui->Password_user->text().toUtf8());
    QString hashStr(hash.result().toHex());
    user.set_password(hashStr);
    user.set_Role(ui->Role_user->currentText());
    if (db->EditData(user))
    {
        addToLog(" Данные пользователя с логином "+user.get_login()+" были успешно обновлены");
        model_user->select();
        CancelUser();
    }
    else
    {
        addToLog(" Ошибка обновления данных пользователя "+user.get_login());
        CancelUser();
    }
}
bool Gui::Sell(QString flight,QString carriage,QString date)
{
    if (db->sell(flight,carriage,date))
    {
        addToLog(" На рейс "+flight+" в вагон "+carriage+" был продан билет");
        return true;
    }
    else
    {
        addToLog(" Ошибка продажи билета");
        return false;
    }
}
void Gui::Connect()
{
    if (ui->Database_Flight->selectionModel()->hasSelection()==false)
        return ;
    Enable_Connect_Settings();
    model_carriage_flight = new QSqlTableModel;
    model_carriage_flight->setTable("Carriages");
    model_carriage_not_flight =new QSqlTableModel;
    model_carriage_not_flight->setTable("Carriages");
    QStringList headers,days;
    headers << "Номер" << "Тип" << "Всего мест"
            << "Свободно мест" << "Номер рейса" <<"День следования"<<"Дата следования"<< "Стоимость";
    int x=ui->Database_Flight->selectionModel()->currentIndex().row();
    days=model_flight->data(model_flight->index(x,3)).toString().split(",");
    ui->Days->addItems(days);
    NumberF=model_flight->data(model_flight->index(x,0)).toString();
    SetDate(0);
    model_carriage_flight->setFilter("Number_flight="+NumberF+" AND Day_of_Week="+ui->Days->currentText());
    model_carriage_not_flight->setFilter("Number_flight is NULL");
    for(int i = 0, j = 0; i < model_carriage->columnCount(); i++, j++)
    {
        model_carriage_flight->setHeaderData(i,Qt::Horizontal,headers[j]);
        model_carriage_not_flight->setHeaderData(i,Qt::Horizontal,headers[j]);
    }
    ui->Carriage_on_Flight->setModel(model_carriage_flight);
    ui->Carriage_no_Flight->setModel(model_carriage_not_flight);
    ui->Carriage_on_Flight->resizeColumnsToContents();
    ui->Carriage_no_Flight->resizeColumnsToContents();
    model_carriage_flight->select();
    model_carriage_not_flight->select();
}
void Gui::AddCarriageToFlight()
{
    if (ui->Carriage_no_Flight->selectionModel()->hasSelection()==false)
        return ;
    int x=ui->Carriage_no_Flight->selectionModel()->currentIndex().row();
    if (db->AddToFlight(NumberF,model_carriage_not_flight->data(model_carriage_not_flight->index(x,0)).toString(),ui->Days->currentText(),ui->Date_of_Flight->date()))
    {
        model_carriage_flight->select();
        model_carriage_not_flight->select();
    }
}
void Gui::RemoveCarriageFromFlight()
{
    if (ui->Carriage_on_Flight->selectionModel()->hasSelection()==false)
        return ;
    int x=ui->Carriage_on_Flight->selectionModel()->currentIndex().row();
    if (db->RemoveFromFlight(model_carriage_flight->data(model_carriage_flight->index(x,0)).toString()))
    {
        model_carriage_flight->select();
        model_carriage_not_flight->select();
    }
}
void Gui::SetCarriage(int index)
{
    model_carriage_flight->setFilter("Number_flights="+NumberF+" AND Day_of_Week="+ui->Days->currentText());
    model_carriage_flight->select();
}
void Gui::CloseConnectCarriage()
{
    ui->lwLog->setVisible(true);
    ui->tabWidget->setVisible(true);
    ui->Carriage_toFlight_settings->setVisible(false);
    ui->Carriage_toFlight_settings->setEnabled(true);
    ui->Settings->setVisible(false);
    model_carriage_flight->clear() ;
    model_carriage_not_flight->clear();
    ui->Days->clear();
    model_carriage->select();
}
bool Gui::Login()
{
    autorize->setWindowTitle( "Вход в систему" );
    autorize->setModal(true);
    if ( autorize->exec() != Autorization::Accepted )
    {
        autorize->close();
        return false;
    }
    else
    {
        this->activateWindow();
        OpenServer();
        return true;
    }
}
bool Gui::CheckAdm(QString login,QString password)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());
    QString hashStr(hash.result().toHex());
    for (int i=0;i<model_user->rowCount();i++)
    {
        const QModelIndex idl=model_user->index(i,4);
        const QModelIndex idp=model_user->index(i,5);
        const QModelIndex idr=model_user->index(i,6);
        if (login==model_user->data(idl) && hashStr==model_user->data(idp) && model_user->data(idr)=="Админ")
        {
            addToLog(" Подтвержден вход в систему");
            return true;
        }
    }
    addToLog(" Попытка входа в систему: Отклонено!");
    return false;
}
int Gui::Number()
{
    return db->maxTicket();
}
bool Gui::return_Ticket(QString tnumber,QString fnumber,QString cnumber,QString date)
{
    return db->return_ticket(tnumber,fnumber,cnumber,date);
}
void Gui::timer_overflow()
{
    ui->DateTime->setDateTime( QDateTime::currentDateTime() );
    db->Time();
    model_flight->select();
    model_carriage->select();
}
void Gui::Logout()
{
    QMessageBox real( this );
    real.setWindowTitle( tr( "Подтверждение" ) );
    real.setIcon( QMessageBox::Warning );
    real.setText( tr( "Вы действительно хотите выйти из системы?" ) );
    real.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    real.setDefaultButton(QMessageBox::No);
    switch(real.exec())
    {
        case QMessageBox::Yes:
        {
            ui->button_StartStop->setChecked(true);
            if (Login()!=true)
                Exit();
        }
        default:
        {
            return;
        }
    }
}
void Gui::SetDate(int index)
{
    QDate date=QDate::currentDate();
    for (;date.dayOfWeek()!=ui->Days->currentText().toInt();)
    {
        date=date.addDays(1);
    }
    ui->Date_of_Flight->setDate(date);
    model_carriage_flight->setFilter("Number_flight="+NumberF+" AND Day_of_Week="+ui->Days->currentText());
    ui->Carriage_on_Flight->setModel(model_carriage_flight);
    model_carriage_flight->select();
}
void Gui::Exit()
{
    QCoreApplication::instance()->QCoreApplication::quit();
}
Gui::~Gui()
{
    delete ui;
}
