#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));
    mStation.reset(new Station);
    ui->Table_flight->setModel(mStation.get());
    connect(ui->Refresh,SIGNAL(clicked()),this,SLOT(RefreshDate()));
    connect(ui->Sell_or_Buy,SIGNAL(clicked()),this,SLOT(sell_Ticket()));
    connect(ui->Return_Ticket,SIGNAL(clicked()),this,SLOT(Ticket_Return()));
    connect(ui->Exit,SIGNAL(triggered()),this,SLOT(Exit()));
    connect(ui->Logout,SIGNAL(triggered()),this,SLOT(Logout_()));
    log=new Autorization();
    connect(log,SIGNAL(Autorize(QString,QString)),this,SLOT(AutorizeRequest(QString,QString)));
    connect(this,SIGNAL(AutorizeTrue()),log,SLOT(accepti()));
    connect(this,SIGNAL(AutorizeFalse()),log,SLOT(Error()));
    connect(log,SIGNAL(registerUser(QString,QString,QString,QString,QString,QString)),this,SLOT(RegisterRequest(QString,QString,QString,QString,QString,QString)));
    connect(this,SIGNAL(RegisterTrue()),log,SLOT(registration_complete()));
    connect(this,SIGNAL(RegisterFalse()),log,SLOT(registration_error()));
}
bool MainWindow::Login()
{
    log->setWindowTitle( "Вход в систему" );
    log->setModal(true);
    if ( log->exec() != Autorization::Accepted )
    {
        log->close();
        return false;
    }
    else
    {
        user=log->return_User();
        if (user.get_role()=="Админ" || user.get_role()=="Кассир")
            ui->Sell_or_Buy->setText("Продать билет");
        else
            ui->Sell_or_Buy->setText("Купить билет");
        QByteArray command="{\"Command\":\"Flight\",\"Param\":\"Length\"}";
        _sok->write(command);
        _sok->waitForBytesWritten(500);
        this->activateWindow();
        return true;
    }
}
bool MainWindow::Connect()
{
    _sok=new QTcpSocket(this);
    connect(_sok, SIGNAL(readyRead()), this, SLOT(onSokReadyRead()));
    connect(_sok, SIGNAL(connected()), this, SLOT(onSokConnected()));
    connect(_sok, SIGNAL(disconnected()), this, SLOT(onSokDisconnected()));
    connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onSokError(QAbstractSocket::SocketError)));
    QString fileName =("system.conf");
    QFile conf(fileName);
    if (!conf.exists())
    {
        conf.open(QIODevice::WriteOnly);
        QTextStream create(&conf);
        create<<tr("Хост: ")<<"127.0.0.1" ;
        create<<tr("\nПорт:")<<" 1234";
        if ( create.status() == QTextStream::WriteFailed )
        {
            throw std::runtime_error( tr( "Write to the stream failed") .toStdString() );
        }
        conf.close();
    }
    conf.open(QIODevice::ReadOnly);
    QTextStream load(&conf);
    QString str,host,port;
    load>>str>>host>>str>>port;
    conf.close();
    _sok->connectToHost(host, port.toInt());
    if (_sok->waitForConnected(2000))
    {
        return true;
    }
    else
    {
        QMessageBox msg(QMessageBox::Critical,"Ошибка связи","Отсутствует соединение с сервером",QMessageBox::Reset|QMessageBox::Cancel);
        switch (msg.exec())
        {
            case QMessageBox::Reset:
            {
                 Connect();
            }
            case QMessageBox::Cancel:
            {
                return false;
            }
        }
    }
}
void MainWindow::AutorizeRequest(QString login,QString password)
{
    QByteArray command=("{\"Command\":\"Autorize\",\"Login\":\""+login+"\",\"Password\":\""+password+"\"}").toUtf8();
    _sok->write(command);
    _sok->waitForBytesWritten(500);
}
void MainWindow::onSokConnected()
{
}
void MainWindow::onSokDisconnected()
{
    deleteLater();
}
void MainWindow::onSokError(QAbstractSocket::SocketError socketError) const
{
    QWidget w;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(&w, "Ошибка", "Нет соединения с сервером!");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(&w, "Ошибка", "Потеря соединения с узлом или превыышет таймаут ожидания!");
        break;
    default:
        QMessageBox::information(&w, "Ошибка", "Ошибка соединения: "+_sok->errorString());
    }

}
void MainWindow::onSokReadyRead()
{
    if (_sok->waitForConnected(1000))
    {
        if (!complexData)
        {
            Data=_sok->readAll();
        }
        else
        {
            Data.append(_sok->readAll());
            complexData=false;
        }
        JDoc=QJsonDocument::fromJson(Data,&JError);
        if (JError.errorString()=="no error occurred")
        {
            if (JDoc.object().value("Command").toString()=="Autorize")
            {
                if (JDoc.object().value("result").toString()=="true")
                {
                    _sok->write("{\"Command\":\"User\",\"Param\":\"Length\"}");
                }
                else emit AutorizeFalse();
            }
            else if (JDoc.object().value("Command").toString()=="List_Flight")
            {
                while (Data.size()!=data_size)
                {
                    Data.append((_sok->readAll()));
                }
                    QVector<Flight> flights;
                    QJsonArray flight=JDoc.object().value("Flight").toArray();
                    for (int i=0;i<flight.count();i++)
                    {
                        Flight temp;
                        temp.set_Number(flight[i].toObject().value("Number").toString().toInt());
                        temp.set_Start(flight[i].toObject().value("Start").toString());
                        temp.set_Finish(flight[i].toObject().value("Finish").toString());
                        temp.set_Days(flight[i].toObject().value("Days_all").toString());
                        temp.set_Time(QTime::fromString(flight[i].toObject().value("Time").toString()));
                        QStringList days_list=temp.Days().split(",");
                        int count=days_list.length();
                        QJsonArray days=flight[i].toObject().value("On_days").toArray();
                        for (int i=0;i<count;i++)
                        {
                            QJsonArray carriages=days[i].toObject().value("Carriage").toArray();
                            QVector<Carriage> carri;
                            for (int j=0;j<carriages.count();j++)
                            {
                                Carriage carri_temp;
                                carri_temp.set_Number(carriages[j].toObject().value("Num").toString().toInt());
                                carri_temp.set_Type(carriages[j].toObject().value("Type").toString());
                                carri_temp.set_place_all(carriages[j].toObject().value("All").toString().toInt());
                                carri_temp.set_place_free(carriages[j].toObject().value("Free").toString().toInt());
                                carri_temp.set_Value(carriages[j].toObject().value("Value").toString().toInt());
                                carri.push_back(carri_temp);
                            }
                            temp.set_Carriage(days_list[i],carri);
                        }
                        flights.push_back(temp);
                    }
                    mStation->setFlights(flights);


            }
            else if (JDoc.object().value("Command").toString()=="Flight")
            {
                data_size=JDoc.object().value("Length").toString().toInt();
                _sok->write("{\"Command\":\"Flight\",\"Param\":\"List\"}");
            }
            else if (JDoc.object().value("Command").toString()=="User")
            {
                data_size=JDoc.object().value("Length").toString().toInt();
                _sok->write("{\"Command\":\"User\",\"Param\":\"Data\"}");
            }
            else if (JDoc.object().value("Command").toString()=="Data_User")
            {
                while (Data.size()<data_size)
                {
                    Data.append((_sok->readAll()));
                }
                User user;
                user.set_first_name(JDoc.object().value("Surname").toString());
                user.set_middle_name(JDoc.object().value("Name").toString());
                user.set_last_name(JDoc.object().value("Last_Name").toString());
                user.set_login(JDoc.object().value("Login").toString());
                user.set_password(JDoc.object().value("Password").toString());
                user.set_Role(JDoc.object().value("Role").toString());
                log->Set_user(user);
                emit AutorizeTrue();
            }
            else if (JDoc.object().value("Command").toString()=="Register")
            {
                if (JDoc.object().value("Param").toString()=="Data")
                {
                    _sok->write(itog);
                }
                else if (JDoc.object().value("Param").toString()=="True")
                {
                    emit RegisterTrue();
                }
                else if (JDoc.object().value("Param").toString()=="False")
                {
                    emit RegisterFalse();
                }
            }
            else if (JDoc.object().value("Command").toString()=="Sell")
            {
                if (JDoc.object().value("result").toString()=="true")
                    emit SellTrue(JDoc.object().value("Number").toString().toInt());
                else emit SellFalse();
            }
            else if (JDoc.object().value("Command").toString()=="Return")
            {
                if (JDoc.object().value("result").toString()=="true")
                {
                    QFile outf( fileName );
                    outf.remove( fileName );
                    QMessageBox OK( this );
                    OK.setWindowTitle( tr( "Возврат билета" ) );
                    OK.setText( tr( "Проверка пройдена! Этот билет удален из базы и компьютера" ) );
                    OK.exec();
                    RefreshDate();
                }
                else
                {
                    QMessageBox error( this );
                    error.setWindowTitle( tr( "Ошибка" ) );
                    error.setText( tr( "Извините! По данным параметрам не было найдено рейсов или билет уже был использован!" ) );
                    error.exec();
                }
            }
            else if (JDoc.object().value("Command").toString()=="System")
            {
                if (JDoc.object().value("Status").toString()=="NOT_AUTHTORIZED")
                {
                    QMessageBox Error( this );
                    Error.setWindowTitle( tr( "Ошибка" ) );
                    Error.setIcon( QMessageBox::Critical );
                    Error.setText( tr( "Для выполнения данного действия вам необходимо авторизоваться!" ));
                    Error.exec();
                }
            }
            else
            {
                complexData=true;
                onSokReadyRead();
            }

        }
        else
        {
            QMessageBox msg(QMessageBox::Critical,"Ошибка данных",JError.errorString(),QMessageBox::Reset|QMessageBox::Cancel);
            switch (msg.exec())
            {
                case QMessageBox::Reset:
                {
                    onSokReadyRead();
                }
                case QMessageBox::Cancel:
                {
                    return;
                }
            }
        }
    }
}
void MainWindow::RegisterRequest(QString Surname,QString Name,QString Last_name,QString Login,QString Password,QString Role)
{
    itog=("{\"Command\":\"Register\",\"Surname\":\""+Surname+"\",\"Name\":\""+Name+"\",\"Last_name\":\""+Last_name+"\",\"Login\":\""+Login+"\",\"Password\":\""+Password+"\",\"Role\":\""+Role+"\"}").toUtf8();
    QByteArray command=("{\"Command\":\"RegisterLength\",\"Length\":\""+QString::number(itog.size())+"\"}").toUtf8();
    _sok->write(command);
    _sok->waitForBytesWritten(500);
}
void MainWindow::on_About_triggered()
{
    QMessageBox aboutDlg( this );
    aboutDlg.setWindowTitle( tr( "О программе" ) );
    aboutDlg.setIcon( QMessageBox::Information );
    aboutDlg.setText( tr( "Программа: Железнодорожный вокзал v2.5<br>"
        "Автор: Исайкин Александр. 3 курс. ИКИТ СФУ.<br>"
        "Программа сделана с использованием QT Library v. %1" )
        .arg( qVersion() ) );
    aboutDlg.exec();
}
void MainWindow::sell_Ticket()
{
    QModelIndex idc = ui->Table_flight->selectionModel()->currentIndex();
    if ( !idc.isValid() )
    {
        QMessageBox error( this );
        error.setWindowTitle( tr( "Ошибка" ) );
        error.setText( tr( "Пожалуйста, выберите рейс!" ) );
        error.exec();
        return;
    }
    Kass ticket( this );
    connect(&ticket,SIGNAL(Send(QByteArray)),this,SLOT(Send_Request(QByteArray)));
    connect(this,SIGNAL(SellTrue(int)),&ticket,SLOT(saveTicket(int)));
    connect(this,SIGNAL(SellFalse()),&ticket,SLOT(Error_ticket()));
    ticket.setRole(user.get_role());
    if (user.get_role()=="Админ"||user.get_role()=="Кассир")
        ticket.setWindowTitle( "Продажа билетов" );
    else ticket.setWindowTitle("Покупка билета");
    ticket.setdata( mStation->operator []( idc.row() ) );
    if ( ticket.exec() != Kass::Accepted ) return;
    RefreshDate();
}
void MainWindow::Exit()
{
    QCoreApplication::instance()->QCoreApplication::quit();//сделать выход
}
void MainWindow::RefreshDate()
{
    QByteArray command="{\"Command\":\"Flight\",\"Param\":\"Length\"}";
    _sok->write(command);
    _sok->waitForBytesWritten(500);
}
void MainWindow::Send_Request(QByteArray command)
{
    _sok->write(command);
    _sok->waitForBytesWritten(500);
}
void MainWindow::Ticket_Return()
{
    fileName = QFileDialog::getOpenFileName( this, tr("Выбрать билет"), QString(), "Ticket (*.txt)" );
    if ( fileName.isNull() == true ) return;
    QFile outf( fileName );
    outf.open( QIODevice::ReadOnly | QIODevice::Text );
    QTextStream ost( &outf );
    QString number_t,number_f,number_c,date,str;
    ost>>str>>number_t;
    str = ost.readLine(); str = ost.readLine(); str = ost.readLine();
    ost >> str >> str >>str>>str>> number_f;
    str = ost.readLine(); str = ost.readLine(); str = ost.readLine();
    ost >> str >> str >> date;
    str = ost.readLine(); str = ost.readLine();
    ost >> str >> number_c;
    QByteArray command(("{\"Command\":\"Return_ticket\",\"Ticket\":\""+number_t+"\",\"Flight\":\""+number_f+"\",\"Carriage\":"
                                      "\""+number_c+"\",\"Date\":\""+date+"\"}").toUtf8());
    _sok->write(command);
    _sok->waitForBytesWritten(500);
    outf.close();
}
void MainWindow::Logout_()
{
    QByteArray command="{\"Command\":\"DeAutorize\"}";
    _sok->write(command);
    _sok->waitForBytesWritten(500);
    if (Login()!=true)
    {
        Exit();
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
