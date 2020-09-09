#include "dialogopen.h"
#include "ui_dialogopen.h"

DialogOpen::DialogOpen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOpen)
{
    ui->setupUi(this);

    model = new QSqlQueryModel(this);
    model->setQuery("select id, nam, d_beg, d_end from econom_unload order by id");
    if (model->lastError().isValid()){
        QMessageBox::critical(this,QObject::tr("Error"),model->lastError().text(),QMessageBox::Ok);
    } else {
        ui->tableView->setModel(model);
        model->setHeaderData(0,Qt::Horizontal,QString("Id"));
        model->setHeaderData(1,Qt::Horizontal,QString("Название"));
        for (int i=2; i<model->columnCount();i++){
            ui->tableView->setColumnHidden(i,true);
        }
        ui->tableView->resizeColumnsToContents();
    }
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(accept()));
}

DialogOpen::~DialogOpen()
{
    delete ui;
}

int DialogOpen::id() const
{
    return currentData(0).toInt();
}

QString DialogOpen::name() const
{
    return currentData(1).toString();
}

QVariant DialogOpen::currentData(int col) const
{
    QModelIndex ind=ui->tableView->currentIndex();
    return ind.isValid()? ui->tableView->model()->data(ui->tableView->model()->index(ind.row(),col),Qt::EditRole) : QVariant();
}
