#include "station.hpp"
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
    return !parent.isValid() ? 5 : 0;
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
            return sFlights[index.row()].Days();
        }
        if ( index.column() == 4 )
        {
            return sFlights[index.row()].Time();
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
                return tr( "Дни следования" );
            }
            if ( section == 4)
            {
                return tr( "Время отправления" );
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

void Station::setFlights( QVector<Flight> Fli )
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

bool Station::number( int Number )
{
    for ( unsigned int i = 0 ; i < sFlights.size() ; i++ )
        if ( Number == sFlights[i].Number() ) return true;
    return false;
}

