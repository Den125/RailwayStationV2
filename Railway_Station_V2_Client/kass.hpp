#ifndef KASS_HPP
#define KASS_HPP

#include <QDialog>
#include <QByteArray>
#include <memory>
#include "flight.hpp"
#include "carriage.hpp"
#include "station.hpp"
#include "depot.hpp"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

namespace Ui {
class Kass;
}

class Kass : public QDialog{
    Q_OBJECT

public:
    explicit Kass(QWidget *parent = 0);
    ~Kass();
    void setdata(const Flight &flight);
    QVector<Carriage> return_carriage();

public slots:
    void Sell();
    void SetValue(const QModelIndex &index);
    void reject();
    void SetCarriage(int index);
    void saveTicket(int num);
    void Error_ticket();
    void setRole(QString Role);
signals:
    void Send(QByteArray command);

private:
    QString fileName;
    Ui::Kass *ui;
    const Flight *Fli;
    std::unique_ptr<Depot> kDepot;
};

#endif // KASS_HPP
