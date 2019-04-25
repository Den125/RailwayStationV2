#include "users.hpp"
#include <QDataStream>

Users::Users()
{
}

int Users::rowCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? User_all.size() : 0;
}

int Users::columnCount( const QModelIndex &parent ) const
{
    return !parent.isValid() ? 5 : 0;
}

QVariant Users::data( const QModelIndex &index, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( index.column() == 0 )
        {
           return User_all[index.row()].get_first_name();
        }
        if ( index.column() == 1 )
        {
           return User_all[index.row()].get_middle_name();
        }
        if ( index.column() == 2 )
        {
            return User_all[index.row()].get_last_name();
        }
        if ( index.column() == 3 )
        {
            return User_all[index.row()].get_login();
        }
        if ( index.column() == 4 )
        {
            return User_all[index.row()].get_role();
        }
    }
    return QVariant();
}

QVariant Users::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            if ( section == 0 )
            {
                return tr( "Фамилия" );
            }
            if ( section == 1 )
            {
                return tr( "Имя" );
            }
            if ( section == 2 )
            {
                return tr( "Отчество" );
            }
            if ( section == 3 )
            {
                return tr( "Логин" );
            }
            if ( section == 4 )
            {
                return tr( "Роль" );
            }
        }
    }

    return QVariant();
}

void Users::add( const User &user )
{
    beginInsertRows( QModelIndex(), size(), size() );
    User_all.push_back( user );
    endInsertRows();
}

int Users::size() const
{
    return User_all.size();
}

void Users::set( std::vector<User> users )
{
    beginResetModel();
    beginInsertRows( QModelIndex(), users.size(), users.size() );
    User_all = users;
    endInsertRows();
    endResetModel();
}

const User &Users::operator[]( int idx ) const
{
    return User_all[idx];
}

bool Users::Check( int idx )
{
    if ( User_all[idx].get_role() == "adm" )
    {
        int count = 0;
        for ( unsigned int i = 0 ; i < User_all.size() ; i++ )
        {
            if ( User_all[i].get_role() == "adm" ) count++;
        }
        if ( count==1 ) return false;
    }
    return true;
}

bool Users::UserChange( const QModelIndex &index, const User &user )
{
    if ( User_all[index.row()].get_active() == true ) return false;
    if ( User_all[index.row()].get_role() == "adm" )
    {
        if ( User_all[index.row()].get_role() != user.get_role() )
        {
            if ( Check( index.row() ) == false ) return false;
        }
    }
    beginResetModel();
    User_all[index.row()] = user;
    emit dataChanged( index, index );
    endResetModel();
    return true;
}

bool Users::erase( int idx )
{
    if ( Check( idx ) == false ) return false;
    beginRemoveRows( QModelIndex(), idx, idx );
    User_all.erase( std::next(User_all.begin(), idx ) );
    endRemoveRows();
    return true;
}

void Users::write( QDataStream &stat )
{
    for ( unsigned int i = 0 ; i < User_all.size() ; i++ )
    {
       User_all[i].save( stat );
       if ( stat.status() == QDataStream::WriteFailed )
       {
           throw std::runtime_error( tr( "Write to the stream failed" ).toStdString() );
       }
    }
}

void Users::erase_activity()
{
    for ( unsigned int i = 0 ; i < User_all.size() ; i++ )
    {
        User_all[i].set_active( false );
    }
}

bool Users::check_login( QString login )
{
    for ( unsigned int i = 0 ;i < User_all.size() ; i++ )
    {
        if ( User_all[i].get_login() == login ) return false;
    }
    return true;
}
