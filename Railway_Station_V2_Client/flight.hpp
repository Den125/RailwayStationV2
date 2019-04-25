#ifndef FLIGHT_H
#define FLIGHT_H

#include <QString>
#include <QMap>
#include <QVector>
#include <QDateTime>
#include "carriage.hpp"

class Flight
{
public:
    Flight();
    Flight(int Number, QString Start, QString Finish,  QString Days, QTime Time);
    void set_Number(int num);
    void set_Start(QString start);
    void set_Finish(QString finish);
    void set_Days(QString days);
    void set_Time(QTime time);
    QString Start() const;
    int Number() const;
    QString Finish() const;
    int carriage_value(QString key);
    QString Days() const;
    QTime Time() const;
    QVector<Carriage> carriage(QString key) const;
    QMap<QString,QVector<Carriage>> return_all() const;
    void set_Carriage(QString key,QVector<Carriage> carri);
    Carriage carriage_id(int idx) const;
    bool operator <(Flight temp);

private:
    int ft_Number;
    QString ft_Days;
    QTime ft_Time;
    QString ft_Start;
    QString ft_Finish;
    QMap<QString,QVector<Carriage>> day_carriage;
};

#endif // FLIGHT_H
