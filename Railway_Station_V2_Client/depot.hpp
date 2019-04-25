#ifndef DEPOT_HPP
#define DEPOT_HPP

#include <vector>
#include <QAbstractTableModel>
#include <QString>
#include "carriage.hpp"

class Depot:public QAbstractTableModel
{
    Q_OBJECT

public:
    Depot();
    Carriage &operator[](int idx);
    void add(Carriage &carriage);
    int size() const;
    void erase(int idx);
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
    QVector<Carriage> all_carriage();
    void setCarriage(QVector<Carriage> carri);
    void CarriageChange(const QModelIndex &index,const Carriage &carriage);
    void SetDataFlight(QMap<QString,QVector<Carriage>>);
    QMap<QString,QVector<Carriage>> return_data();

private:
    QVector<Carriage> sCarriage;
    QMap<QString ,QVector<Carriage>> dCarriage;
};

#endif // STATION_H
