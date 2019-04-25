#include "carriage.hpp"

Carriage::Carriage()
{
}

Carriage::Carriage(int num, QString type, int place_all, int Value ):
    c_Number(num),
    c_Type( type ),
    c_place_all( place_all ),
    c_place_free( place_all ),
    c_value(Value )
{
}

void Carriage::set_Type( QString type )
{
    c_Type = type;
}
void Carriage::set_Number( int num )
{
    c_Number=num;
}
void Carriage::set_place_all( int place )
{
    c_place_all = place;
}

bool Carriage::set_place_free( int place )
{
    if( place > c_place_all || place < 0 ) return false;
    else
    {
        c_place_free = place;
        return true;
    }
}

void Carriage::set_Value( int Value )
{
    c_value = Value;
}

QString Carriage::type() const
{
    return c_Type;
}

int Carriage::place_all() const
{
    return c_place_all;
}

int Carriage::place_free() const
{
    return c_place_free;
}

int Carriage::Value() const
{
    return c_value;
}
int Carriage::Number_carriage() const
{
    return c_Number;
}
void Carriage::reset()
{
    c_place_free = c_place_all;
}
