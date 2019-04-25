#include "user.hpp"

User::User()
{

}
User::User( QString first, QString middle, QString last, QString login, QString pass, QString role ):
    First_name( first ),
    Middle_name( middle ),
    Last_name( last ),
    Login( login ),
    Password( pass ),
    Role( role )
{
    active = false;
}

QString User::get_login() const
{
    return Login;
}

QString User::get_pw() const
{
    return Password;
}

QString User::get_first_name() const
{
    return First_name;
}

QString User::get_middle_name() const
{
    return Middle_name;
}

QString User::get_last_name() const
{
    return Last_name;
}

QString User::get_role() const
{
    return Role;
}

void User::set_password( QString pass )
{
    Password = pass;
}

void User::set_login( QString log )
{
    Login = log;
}

void User::set_first_name( QString name )
{
    First_name = name;
}

void User::set_middle_name( QString name )
{
    Middle_name = name;
}

void User::set_last_name( QString name )
{
    Last_name = name;
}

void User::set_Role( QString role )
{
    Role = role;
}

void User::set_active( bool flag )
{
    active = flag;
}
void User::empty()
{
    First_name.clear();
    Middle_name.clear();
    Last_name.clear();
    Login.clear();
    Password.clear();
    Role.clear();
}
bool User::get_active()
{
    return active;
}

void User::load( QDataStream &ist )
{
    ist >> First_name >> Middle_name >> Last_name >> Login >> Password >> Role;
}

void User::save( QDataStream &ost )
{
    ost << First_name << Middle_name << Last_name << Login << Password << Role;
}
