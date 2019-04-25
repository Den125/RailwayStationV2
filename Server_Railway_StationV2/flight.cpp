#include "flight.hpp"

Flight::Flight()
{
}

Flight::Flight( int Number, QString Start, QString Finish, QString Days, QTime Time )
    : ft_Number( Number )
    , ft_Days( Days )
    , ft_Time( Time )
    , ft_Start( Start )
    , ft_Finish( Finish )
{
}

void Flight::set_Number( int num )
{
    ft_Number = num;
}

void Flight::set_Start( QString start )
{
    ft_Start = start;
}

void Flight::set_Finish( QString finish )
{
    ft_Finish = finish;
}

void Flight::set_Days( QString days )
{
    ft_Days = days;
}

void Flight::set_Time( QTime time )
{
    ft_Time = time;
}

int Flight::Number() const
{
    return ft_Number;
}

QString Flight::Start() const
{
    return ft_Start;
}

QString Flight::Finish() const
{
    return ft_Finish;
}

QString Flight::Days() const
{
    return ft_Days;
}

QTime Flight::Time() const
{
    return ft_Time;
}

QVector<Carriage> Flight::carriage(QString key) const
{
    return day_carriage[key];
}
QMap<QString,QVector<Carriage>> Flight::return_all() const
{
    return day_carriage;
}
void Flight::set_Carriage(QString key, QVector<Carriage> carri )
{
    day_carriage.insert(key,carri);
}

int Flight::carriage_value(QString key)
{
  return day_carriage[key].size();
}

bool Flight::operator <( Flight temp )
{
    if ( this->Number() < temp.Number() ) return true;
    else return false;
}

