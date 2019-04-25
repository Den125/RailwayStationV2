#ifndef STATION_H
#define STATION_H

#include <QVector>
#include <QAbstractTableModel>
#include <QString>
#include <QDataStream>
#include "flight.hpp"
#include "carriage.hpp"


class Station:public QAbstractTableModel
{
    Q_OBJECT

public:
    Station();
    const Flight &operator[](int idx) const;
    void add(const Flight &flight);
    int size() const;
    void erase(int idx);
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
    QVector<Carriage> carriage(int idx);
    void FlightChange(const QModelIndex &index,const Flight &flight);
    void add_carriage(int idx, std::vector<Carriage> carri);
    const QVector<Flight> return_flights();
    void setFlights(QVector<Flight> Fli);
    void erase_all();
    void write(QDataStream &stat);
    void load(QDataStream &stat);
    bool number(int Number);
    int count_carriage(int idx);
    int free_place(int idx);

private:
    QVector<Flight> sFlights;
};

#endif // STATION_H
