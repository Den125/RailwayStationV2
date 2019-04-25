#ifndef CARRIAGE_HPP
#define CARRIAGE_HPP

#include <QString>

class Carriage
{
public:
    Carriage();
    Carriage(int num,QString type, int place_all, int Value);
    void set_Type(QString type);
    void set_place_all(int place);
    bool set_place_free(int place);
    void set_Value(int Value);
    void set_Number(int num);
    QString type() const;
    int place_all() const;
    int place_free() const;
    int Value() const;
    int Number_carriage() const;
    void reset();

private:
    int c_Number;
    QString c_Type;
    int c_place_all;
    int c_place_free;
    int c_value;
};

#endif // CARRIAGE_HPP
