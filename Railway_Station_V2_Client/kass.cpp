#include "kass.hpp"
#include "ui_kass.h"

Kass::Kass( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::Kass )
{
    ui->setupUi( this );
    ui->Date_Start->setMinimumDate(QDate::currentDate());
    ui->Date_Start->setMaximumDate(QDate::currentDate().addDays(7));
    QRegExp MASKFIVE  ( "[0-9][0-9][0-9][0-9][0-9]" );
    QRegExpValidator *VALUE = new QRegExpValidator( MASKFIVE, this );
    ui->Value->setValidator(VALUE);
    connect(ui->days,SIGNAL(activated(int)),this,SLOT(SetCarriage(int)));
}

Kass::~Kass()
{
    delete ui;
}
void Kass::SetCarriage(int index)
{
    kDepot->setCarriage(Fli->carriage(ui->days->currentText()));
    QDate date=QDate::currentDate();
    for (;date.dayOfWeek()!=ui->days->currentText().toInt();)
    {
        date=date.addDays(1);
    }
    ui->Date_Start->setDate(date);
}
void Kass::setdata( const Flight &flight )
{
    Fli = &flight;
    kDepot.reset( new Depot );
    ui->Carriage->setModel( kDepot.get() );
    kDepot->SetDataFlight(Fli->return_all());
    ui->Number->setText( QString::number( Fli->Number() ) );
    ui->Place_Start->setText( Fli->Start() );
    ui->Place_Finish->setText( Fli->Finish() );    
    ui->Time_Start->setText( Fli->Time().toString() );
    QStringList days=Fli->Days().split(',');
    ui->days->addItems(days);
    SetCarriage(0);
}
void Kass::Sell()
{
    if ( ui->Date_Start->date()==QDate::currentDate() )
        if ( Fli->Time().toString() <= QTime::currentTime().toString() )
            reject();
    QModelIndexList idc = ui->Carriage->selectionModel()->selectedRows();
    if ( idc.empty() ) return;
    const Carriage *temp;
    temp = &( kDepot.get()->operator[]( idc[0].row() ) );
    if ( temp->place_free() == 0 )
    {
            QMessageBox error( this );
            error.setIcon( QMessageBox::Warning );
            error.setText( tr( "Извините в данном вагоне мест нет!" ) );
            error.exec();
            return;
    }
    if ( ui->first_Name->text() == NULL ||ui->Name->text() == NULL || ui->middle_name->text() == NULL )
    {
            QMessageBox error( this );
            error.setWindowTitle( tr( "Ошибка" ) );
            error.setIcon( QMessageBox::Warning );
            error.setText( tr( "Пожалуйста введите данные пассажира!" ) );
            error.exec();
            return;
    }
    fileName = QFileDialog::getSaveFileName( this, tr( "Сохранить билет как" ), QString(), "Ticket (*.txt)" );
    if ( fileName.isNull() == true ) return;
    QByteArray command(("{\"Command\":\"Ticket\",\"Flight\":\""+QString::number(Fli->Number())+"\",\"Carriage\":"
                                      "\""+QString::number(temp->Number_carriage())+"\",\"Date\":\""+ui->Date_Start->date().toString("dd.MM.yyyy")+"\"}").toUtf8());
    ui->Sell->setEnabled(false);
    ui->Exit->setEnabled(false);
    emit Send(command);
}
void Kass::saveTicket(int num)
{
    QModelIndexList idc = ui->Carriage->selectionModel()->selectedRows();
    const Carriage *temp;
    temp = &( kDepot.get()->operator[]( idc[0].row() ) );
    QFile outf( fileName );
    outf.open( QIODevice::WriteOnly | QIODevice::Text );
    QTextStream ost( &outf );
    ost << tr("Номер: ") << num;
    ost << tr( "\nФамилия: " ) << ui->first_Name->text() << tr( "\nИмя: " ) << ui->Name->text();
    ost << tr( "\nОтчество: " ) << ui->middle_name->text() << tr( "\nНомер рейса: " ) << Fli->Number();
    ost << tr( "\nМесто отправления: " ) << Fli->Start() << tr( "\nМесто назначения: " ) << Fli->Finish();
    ost << tr( "\nДата отпраления: " ) << ui->Date_Start->date().toString("dd.MM.yyyy") << tr( "\nВремя отправления: " ) << Fli->Time().toString();
    ost << tr( "\nВагон: " )<<temp->Number_carriage()<<tr("\nТип: ") << temp->type() << tr( "\nСтоимость: " ) << temp->Value();
    ost << tr( "\nДата и время продажи:" ) << QDateTime::currentDateTime().toString("dd.MM.yyyy");
    if ( ost.status() == QTextStream::WriteFailed )
    {
        throw std::runtime_error( tr( "Write to the stream failed") .toStdString() );
    }
    kDepot->operator [](idc[0].row()).set_place_free( ( kDepot->operator [](idc[0].row()).place_free() ) - 1 );
    ui->Sell->setEnabled(true);
    ui->Exit->setEnabled(true);
    QMessageBox msg( this );
    msg.setWindowTitle( tr( "Операция успешна" ) );
    msg.setIcon( QMessageBox::Information );
    msg.setText( tr( "Билет успешно продан!" ) );
    msg.exec();
}
void Kass::Error_ticket()
{
    QMessageBox error( this );
    error.setWindowTitle( tr( "Ошибка" ) );
    error.setIcon( QMessageBox::Warning );
    error.setText( tr( "Извините! Билет не был продан! Повторите запрос позже" ) );
    error.exec();
    return;
}
void Kass::SetValue( const QModelIndex &index )
{
   ui->Value->setText( ui->Carriage->model()->data(ui->Carriage->model()->index(index.row(),3)).toString());
}
void Kass::reject()
{
    accept();
}
void Kass::setRole(QString Role)
{
    if (Role=="Админ" || Role=="Кассир")
        ui->Sell->setText("Продать");
    else ui->Sell->setText("Купить");
}
