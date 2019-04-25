#ifndef STATION_H
#define STATION_H

#include <vector>
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
    std::vector<Carriage> carriage(int idx);
    void FlightChange(const QModelIndex &index,const Flight &flight);
    void add_carriage(int idx, std::vector<Carriage> carri);
    const std::vector<Flight> return_flights();
    void setFlights(std::vector<Flight> Fli);
    void erase_all();
    void write(QDataStream &stat);
    void load(QDataStream &stat);
    bool compare(Flight flight);
    bool number(int Number);
    int count_carriage(int idx);
    std::vector<int> check(const QDateTime date);
    int free_place(int idx);

private:
    std::vector<Flight> sFlights;
};

#endif // STATION_H
