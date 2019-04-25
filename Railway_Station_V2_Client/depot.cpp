#include "depot.hpp"

Depot::Depot()
{
}

void Depot::add( Carriage &carriage )
{
    beginInsertRows( QModelIndex(), size(), size() );
    carriage.reset();
    sCarriage.push_back( carriage );
    endInsertRows();
}
void Depot::SetDataFlight(QMap<QString,QVector<Carriage>> map)
{
    dCarriage=map;
}
int Depot::size() const
{
    return sCarriage.size();
}

int Depot::rowCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? sCarriage.size() : 0;
}

int Depot::columnCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? 4 : 0;
}

QVariant Depot::data( const QModelIndex &index, int role ) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0)
        {
           return sCarriage[index.row()].type();
        }
        if (index.column() == 1)
        {
           return sCarriage[index.row()].place_all();
        }
        if (index.column() == 2)
        {
            return sCarriage[index.row()].place_free();
        }
        if (index.column() == 3)
        {
            return sCarriage[index.row()].Value();
        }
    }
    return QVariant();
}

QVariant Depot::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0)
            {
                return tr("Тип вагона");
            }
            if (section == 1)
            {
                return tr("Мест в вагоне");
            }
            if (section == 2)
            {
                return tr("Свободно мест");
            }
            if (section == 3)
            {
                return tr("Стоимость билета");
            }
        }
    }

    return QVariant();
}

void Depot::erase( int idx )
{
    beginRemoveRows( QModelIndex(), idx ,idx );
    sCarriage.erase( std::next( sCarriage.begin(), idx ) );
    endRemoveRows();
}

QVector<Carriage> Depot::all_carriage()
{
    return sCarriage;
}

void Depot::setCarriage( QVector<Carriage> carri )
{
    beginResetModel();
    endResetModel();
    beginInsertRows( QModelIndex(), carri.size(), carri.size() );
    sCarriage = carri;
    endInsertRows();
}

void Depot::CarriageChange( const QModelIndex &index, const Carriage &carriage )
{
    sCarriage[index.row()] = carriage;
    emit dataChanged( QModelIndex(), QModelIndex() );
}

Carriage &Depot::operator[](int idx)
{
    return sCarriage[idx];
}
QMap<QString,QVector<Carriage>> Depot::return_data()
{
    return dCarriage;
}

