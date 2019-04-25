#include "database.hpp"

Database::Database(QWidget *parent)
{
    _widget=parent;
}

bool Database::createConnection(QString Server)
{
    db = QSqlDatabase::addDatabase("QODBC3");
    db.setDatabaseName("DRIVER={SQL Server};SERVER="+Server+";DATABASE=Railway_Station;Trusted_Connection=yes;");
    if (!db.open()) {
        emit TableStat(" Ошибка: Невозможно открыть базу данных "+db.lastError().text());
        return false;
    }
    else
    {
        emit TableStat(" Установлено соединение с базой данных");
        if (db.open())
        return true;
        else return false;
    };
};

bool Database::CheckTableCarriages()
{
    QStringList dbtables = db.tables(QSql::Tables);
    if (dbtables.contains("Carriages",Qt::CaseInsensitive))
    {
        emit TableStat(" Таблица Сarriages уже существует");
        return true;
    }
    else
    {
        emit TableStat(" Таблица Сarriages не найдена. Создание таблицы");
        QSqlQuery query= QSqlQuery();
        return query.exec(
               "create table Carriages ( "
                  "  Number integer PRIMARY KEY, "
                   " Type varchar(255) not null, "
                   " Place_all integer not null, "
                   " Place_free integer not null, "
                   " Number_flight integer  null, "
                   " Day_of_Week integer null, "
                   " Date_of_Flight date null,"
                   " Value integer not null "
                   ") ");
    }
}

bool Database::CheckTableUsers()
{
    QStringList dbtables = db.tables(QSql::Tables);
    if (dbtables.contains("Users",Qt::CaseInsensitive))
    {
        emit TableStat(" Таблица Users уже существует");
        return true;
    }
    else
    {
        emit TableStat(" Таблица Users не найдена. Создание таблицы");
        QSqlQuery query= QSqlQuery();
        if ( query.exec(
               "create table users ( "
                  " Number integer PRIMARY KEY IDENTITY(1,1), "
                  " Surname varchar(255) not null, "
                  " Name varchar(255) not null, "
                  " Last_name varchar(255) not null, "
                  " Login varchar(255) not null, "
                  " Password varchar(255) not null, "
                  " Role varchar(10) not null"
                  ") "))
        {
            QString str("Insert into users(Surname,Name,Last_name,Login,Password,Role)"
                        "Values(%1,%2,%3,%4,%5,%6)");
            QCryptographicHash hash(QCryptographicHash::Md5);
            hash.addData("Admin");
            QString hashStr(hash.result().toHex());
            str=str.arg("'Admin'")
                    .arg("'Admin'")
                    .arg("'Admin'")
                    .arg("'Admin'")
                    .arg("'"+hashStr+"'")
                    .arg("'Админ'");
            return query.exec(str);
        }
    }
}

bool Database::CheckTableFlights()
{
        QStringList dbtables = db.tables(QSql::Tables);
        if (dbtables.contains( "Flights", Qt::CaseInsensitive))
        {
            emit TableStat(" Таблица Flights уже существует");
            return true;
        }
        else
        {
            emit TableStat(" Таблица Flights не найдена. Создание таблицы");
            QSqlQuery query= QSqlQuery();
            return query.exec(
                   "create table flights ( "
                      "  Number integer PRIMARY KEY, "
                       "  Start varchar(255) not null, "
                       "  Finish varchar(255) not null, "
                       "  Days varchar(13) not null, "
                      "  Time time not null "
                   ") ");
        }
}

bool Database::CheckTableTickets()
{
        QStringList dbtables = db.tables(QSql::Tables);
        if (dbtables.contains( "Tickets", Qt::CaseInsensitive))
        {
            emit TableStat(" Таблица Tickets уже существует");
            return true;
        }
        else
        {
            emit TableStat(" Таблица Tickets не найдена. Создание таблицы");
            QSqlQuery query= QSqlQuery();
            return query.exec(
                   "create table Tickets ( "
                      "  Number integer PRIMARY KEY IDENTITY(1,1), "
                       " Flight integer not null, "
                       " Carriage integer not null, "
                       " Date_of_Flight Date not null, "
                       " Enable BIT not null"
                        ") ");
        }
}

bool Database::AddToDataBase(const Flight fli)
{
    QSqlQuery query= QSqlQuery();
    QString str("Insert into flights(Number,Start,Finish,Days,Time)"
                " Values(%1,%2,%3,%4,%5)");
    str=str.arg("'"+QString::number(fli.Number())+"'")
            .arg("'"+fli.Start()+"'")
            .arg("'"+fli.Finish()+"'")
            .arg("'"+fli.Days()+"'")
            .arg("'"+fli.Time().toString()+"'");
    return query.exec(str);
}

bool Database::AddToDataBase(Carriage carri)
{
    QSqlQuery query= QSqlQuery();
    QString str("Insert into Carriages(Number,Type,Place_all,Place_free,Value)"
                " Values(%1,%2,%3,%4,%5,%6)");
    str=str.arg("'"+QString::number(carri.Number_carriage())+"'")
            .arg("'"+carri.type()+"'")
            .arg("'"+QString::number(carri.place_all())+"'")
            .arg("'"+QString::number(carri.place_free())+"'")
            .arg("'"+QString::number(carri.Value())+"'");
    return query.exec(str);
}

bool Database::AddToDataBase(User user)
{
    QSqlQuery query= QSqlQuery();
    QString str("Insert into users(Surname,Name,Last_name,Login,Password,Role)"
                " Values(%1,%2,%3,%4,%5,%6)");
    str=str.arg("'"+user.get_first_name()+"'")
            .arg("'"+user.get_middle_name()+"'")
            .arg("'"+user.get_last_name()+"'")
            .arg("'"+user.get_login()+"'")
            .arg("'"+user.get_pw()+"'")
            .arg("'"+user.get_role()+"'");
    return query.exec(str);
}

bool Database::DeleteFlight(int num)
{
    QSqlQuery query= QSqlQuery();
    QString str("Delete From flights "
                "Where Number = %1");
    str=str.arg(QString::number(num));
    if (query.exec(str))
    {
        str="Update Carriages"
            " SET Number_flight = NULL,"
            " Day_of_Week = NULL,"
            " Date_of_Flight = NUll"
            " WHERE Number_flight =%1";
        str=str.arg("'"+QString::number(num)+"'");
        return query.exec(str);
    }
    return false;
}

bool Database::DeleteCarraige(int num)
{
    QSqlQuery query= QSqlQuery();
    QString str("Delete From Carriages "
                "Where Number = %1");
    str=str.arg(QString::number(num));

    return query.exec(str);
}

bool Database::DeleteUser(QString login)
{
    QSqlQuery query= QSqlQuery();
    QString check("Select Role From users"
                  " Where Login='%1'");
    check=check.arg(login);
    query.exec(check);
    if (query.size()!=0)
    {
        query.next();
        if (query.value(0).toString()=="Admin")
        {
            QString check_adm("Select Count(*)"
                          " From users"
                          " Where Role='Admin'");
            query.exec(check_adm);
            query.next();
            if (query.value(0).toInt()==1)
            {
                return false;
            }
        }
    }

    QString str("Delete From users "
                " Where Login = '%1'");
    str=str.arg(login);
    return query.exec(str);
}

bool Database::EditData(Flight fli)
{
    QSqlQuery query= QSqlQuery();
    QString str("Update flights"
                " SET Start = %2,"
                "Finish = %3,"
                "Days = %4,"
                "Time= %5"
                " WHERE Number =%1");
    str=str.arg("'"+QString::number(fli.Number())+"'")
            .arg("'"+fli.Start()+"'")
            .arg("'"+fli.Finish()+"'")
            .arg("'"+fli.Days()+"'")
            .arg("'"+fli.Time().toString()+"'");
    return query.exec(str);
}
bool Database::EditData(Carriage carri)
{
    QSqlQuery query= QSqlQuery();
    QString str("Update Carriages"
                " SET Type = %2,"
                "Place_all = %3,"
                "Place_free = %4,"
                "Value= %5"
                " WHERE Number =%1");
    str=str.arg("'"+QString::number(carri.Number_carriage())+"'")
            .arg("'"+carri.type()+"'")
            .arg("'"+QString::number(carri.place_all())+"'")
            .arg("'"+QString::number(carri.place_free())+"'")
            .arg("'"+QString::number(carri.Value())+"'");
    return query.exec(str);
}
bool Database::EditData(User user)
{
    QSqlQuery query= QSqlQuery();
    QString str("Update users"
                " SET Surname = %1,"
                "Name = %2,"
                "Last_name = %3,"
                "Password= %5,"
                "Role = %6"
                " WHERE Login =%4");
    str=str.arg("'"+user.get_first_name()+"'")
            .arg("'"+user.get_middle_name()+"'")
            .arg("'"+user.get_last_name()+"'")
            .arg("'"+user.get_login()+"'")
            .arg("'"+user.get_pw()+"'")
            .arg("'"+user.get_role()+"'");
    return query.exec(str);
}
bool Database::sell(QString flight,QString carriage,QString date)
{
    int x;
    QSqlQuery query= QSqlQuery();
    QString str("Select Place_free x "
                "From Carriages "
                "Where Number = %1 AND Number_flight=%2 ");
    str=str.arg("'"+carriage+"'")
            .arg("'"+flight+"'");
    query.exec(str);
    query.next();
    x = query.value(0).toInt();
    if (x>0)
    {
        QString stri("Update Carriages "
                     "SET Place_free = %1 "
                     "WHERE Number =%2 AND Number_flight=%3");
        stri=stri.arg("'"+QString::number(x-1)+"'")
                .arg("'"+carriage+"'")
                .arg("'"+flight+"'");
        if (query.exec(stri))
        {
            QString strin("Insert into Tickets(Flight,Carriage,Date_of_Flight,Enable)"
                        " Values(%1,%2,%3,'true')");
            strin=strin.arg("'"+flight+"'")
                    .arg("'"+carriage+"'")
                    .arg("'"+date+"'");
            return query.exec(strin);
        }
        else
        return false;
    }
    return false;
}
bool Database::AddToFlight(QString flight,QString carriage,QString day,QDate date)
{
    QSqlQuery query= QSqlQuery();
    QString str("Update Carriages"
                " SET Number_flight = %2, "
                " Day_of_Week = %3,"
                " Date_of_Flight = %4 "
                " WHERE Number = %1");
    str=str.arg("'"+carriage+"'")
            .arg("'"+flight+"'")
            .arg("'"+day+"'")
            .arg("'"+date.toString("dd.MM.yyyy")+"'");
    return query.exec(str);
}
bool Database::RemoveFromFlight(QString carriage)
{
    QSqlQuery query= QSqlQuery();
    QString str("Update Carriages"
                " SET Number_flight = NULL, "
                " Day_of_Week = NULL, "
                " Date_of_Flight = NULL"
                " WHERE Number = %1");
    str=str.arg("'"+carriage+"'");
    return query.exec(str);
}
int Database::maxTicket()
{
    int x;
    QSqlQuery query= QSqlQuery();
    QString str("Select Max(Number) x "
                "From Tickets ");
    query.exec(str);
    query.next();
    x = query.value(0).toInt();
    return x;
}
bool Database::return_ticket(QString tnumber,QString fnumber,QString cnumber,QString date)
{
    QSqlQuery query= QSqlQuery();
    int x,y;
    QString count("Select Count(*) x"
                  " From Tickets");
    if (query.exec(count))
    {
        query.next();
        x = query.value(0).toInt();
        if (x==0) return false;
        QString str("Delete From Tickets "
                "Where Number = %1 AND "
                "Flight = %2 AND "
                "Carriage = %3 AND "
                "Date_of_Flight = %4 AND "
                "Enable = 'True'");
        str=str.arg("'"+tnumber+"'")
                .arg("'"+fnumber+"'")
                .arg("'"+cnumber+"'")
                .arg("'"+date+"'");
        if (query.exec(str))
        {
            count="Select Count(*) y"
               " From Tickets";
            if (query.exec(count))
            {
                query.next();
                y = query.value(0).toInt();
                if (x-y==1) return true;
                else return false;
            }
            else return false;
        }
        else return false;
    }
    else return false;
}
void Database::Time()
{
    QSqlQuery query= QSqlQuery();
    QString str("Select Number "
                "From flights "
                "WHERE Time<%1 OR Charindex(%2,Days)>0");
    str=str.arg("'"+QTime::currentTime().toString("hh:mm:ss")+"'")
            .arg("'"+QString::number(QDate::currentDate().dayOfWeek())+"'");
    if ( query.exec(str))
    {
        while(query.next())
        {
            QSqlQuery update=QSqlQuery();
            int x=query.value(0).toInt();
            str="Update Carriages"
                " SET Number_flight = NULL,"
                " Day_of_Week = NULL,"
                " Date_of_Flight = NUll"
                " WHERE Number_flights =%1 AND Date_of_Flight<=%2";
            str=str.arg("'"+QString::number(x)+"'")
                    .arg("'"+QDate::currentDate().toString("dd.MM.yyyy")+"'");
            update.exec(str);

            str="Update Tickets "
                "SET Enable = 'False' "
                "WHERE Flight = %1 AND Date_of_Flight <= %2";
            str=str.arg("'"+QString::number(x)+"'")
                    .arg("'"+QDate::currentDate().toString("dd.MM.yyyy")+"'");
            update.exec(str);
        }
        str="Update Carriages"
             " SET Place_free=Place_all"
             " WHERE Number_flight IS NULL";
        query.exec(str);
    }
}
