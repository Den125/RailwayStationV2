#include "station.hpp"
#include <QtCore>
#include <QString>
#include <QDebug>
#include "flight.hpp"

Station::Station()
{
}

void Station::add( const Flight &flight )
{
    beginInsertRows( QModelIndex(), size(), size() );
    sFlights.push_back( flight );
    std::sort( sFlights.begin(), sFlights.end() );
    endInsertRows();
}

int Station::size() const
{
    return sFlights.size();
}

int Station::rowCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? sFlights.size() : 0;
}

int Station::columnCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? 6 : 0;
}

QVariant Station::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::DisplayRole )
    {
        if ( index.column() == 0 )
        {
           return sFlights[index.row()].Number();
        }
        if ( index.column() == 1 )
        {
           return sFlights[index.row()].Start();
        }
        if ( index.column() == 2 )
        {
            return sFlights[index.row()].Finish();
        }
        if ( index.column() == 3 )
        {
            return sFlights[index.row()].Date();
        }
        if ( index.column() == 4 )
        {
            return sFlights[index.row()].Time();
        }
        if ( index.column() == 5 )
        {
            return sFlights[index.row()].place_free();
        }
    }
    return QVariant();
}

QVariant Station::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            if ( section == 0 )
            {
                return tr( "Номер рейса" );
            }
            if ( section == 1 )
            {
                return tr( "Пункт отправления" );
            }
            if ( section == 2 )
            {
                return tr( "Пункт назначения" );
            }
            if ( section == 3 )
            {
                return tr( "Дата отправления" );
            }
            if ( section == 4)
            {
                return tr( "Время отправления" );
            }
            if ( section == 5 )
            {
                return tr("Свободно мест");
            }
        }
    }

    return QVariant();
}

void Station::erase( int idx )
{
    beginRemoveRows( QModelIndex(), idx, idx );
    sFlights.erase( std::next( sFlights.begin(), idx ) );
    endRemoveRows();
}

std::vector<Carriage> Station::carriage( int idx )
{
    return sFlights[idx].carriage();
}

void Station::FlightChange( const QModelIndex &index, const Flight &flight )
{
    beginResetModel();
    sFlights[index.row()] = flight;
    std::sort( sFlights.begin(), sFlights.end() );
    emit dataChanged( index, index );
    endResetModel();
}

const Flight &Station::operator[]( int idx ) const
{
    return sFlights[idx];
}

int Station::count_carriage( int idx )
{
    return sFlights[idx].carriage_value();
}

void Station::add_carriage( int idx, std::vector<Carriage> carri )
{
    sFlights[idx].set_Carriage( carri );
}

const std::vector<Flight> Station::return_flights()
{
    return sFlights;
}

void Station::setFlights( std::vector<Flight> Fli )
{
    beginResetModel();
    beginInsertRows( QModelIndex(), Fli.size(), Fli.size() );
    sFlights = Fli;
    std::sort( sFlights.begin(), sFlights.end() );
    endInsertRows();
    endResetModel();
}

void Station::erase_all()
{
    beginResetModel();
    sFlights.clear();
    endResetModel();
}

void Station::write(QDataStream &stat)
{
    for ( unsigned int i = 0 ; i < sFlights.size() ; i++ )
    {
       stat << sFlights[i].Number() << sFlights[i].Start();
       stat << sFlights[i].Finish() << sFlights[i].Date() << sFlights[i].Time();
       stat << sFlights[i].carriage_value();
       sFlights[i].write_carriage( stat );
       if ( stat.status() == QDataStream::WriteFailed )
       {
           throw std::runtime_error( tr( "Write to the stream failed" ).toStdString() );
       }
    }
}

void Station::load( QDataStream &stat )
{
    while ( !stat.atEnd() )
    {
       Flight fl; int i,kol;
       QString start,fin;
       QDate date; QTime time;
       stat >> i >> start >> fin >> date >> time;
       if ( stat.status() == QDataStream::ReadCorruptData )
       {
           throw std::runtime_error( tr( "Corrupt data were read from the stream" ).toStdString() );
       }
       fl.set_Number( i );
       fl.set_Start( start );
       fl.set_Finish( fin );
       fl.set_Date( date );
       fl.set_Time( time );
       stat >> kol;
       fl.load_carriage( stat, kol );
       sFlights.push_back( fl );
    }
}

bool Station::compare( Flight flight )
{
    bool flag = false;
    for ( unsigned int i = 0 ; i < sFlights.size() ; i++ )
    {
        if ( flag == true) break;
        if ( flight.Number() == sFlights[i].Number() )
        {
            if ( flight.Start() == sFlights[i].Start() && flight.Finish() == sFlights[i].Finish() )
                {
                    if ( flight.Date() == sFlights[i].Date() )
                    {
                        if ( flight.Time().toString() == sFlights[i].Time().toString() )
                        {
                            Carriage temp = flight.carriage_id( 0 );
                            for ( int j = 0 ; j < sFlights[i].carriage_value() ; j++ )
                            {
                                if ( temp.type() == sFlights[i].carriage_id( j ).type() )
                                {
                                    if ( temp.Value() == sFlights[i].carriage_id( j ).Value() )
                                    {
                                        if ( sFlights[i].carriage_id( j ).set_place_free( sFlights[i].place_free( j ) + 1 ) == true )
                                        {
                                            sFlights[i].set_place( j );
                                            flag = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    return flag;
}

bool Station::number( int Number )
{
    for ( unsigned int i = 0 ; i < sFlights.size() ; i++ )
        if ( Number == sFlights[i].Number() ) return true;
    return false;
}

std::vector<int> Station::check( const QDateTime date )
{
    std::vector<int> num;
    for ( unsigned int i = 0 ; i < sFlights.size() ; i++ )
        if ( sFlights[i].Date() == date.date() )
        {
            if ( sFlights[i].Time().toString() <= date.time().toString() )
            {
               num.push_back( i );
            }
        }
        else if ( sFlights[i].Date() < date.date() )
            {
                num.push_back( i );
            }
    return num;
}

int Station::free_place( int idx)
{
    return sFlights[idx].place_free();
}
