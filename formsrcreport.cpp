#include "formsrcreport.h"
#include "ui_formsrcreport.h"

FormSrcReport::FormSrcReport(int id_report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSrcReport)
{
    ui->setupUi(this);
    id=id_report;
    srcModel = new TreeModel(this);
    ui->treeViewSrc->setModel(srcModel);
    refreshSrc();

    calcModel = new TreeModel(this);
    ui->treeViewCalc->setModel(calcModel);
}

FormSrcReport::~FormSrcReport()
{
    delete ui;
}

bool FormSrcReport::deleteReport()
{
    QSqlQuery query;
    query.prepare("select * from econom_delete_unload(:id)");
    query.bindValue(":id",id);
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

bool FormSrcReport::refreshSrc()
{
    QSqlQuery query;
    query.prepare("select cost, kvov, cost_price, kvo, price, val from econom_tree_src(:id)");
    query.bindValue(":id",id);
    bool ok = query.exec();
    if (ok){
        srcModel->refresh(query);
        srcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        srcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        srcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        srcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        srcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        srcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewSrc->resizeColumnToContents(0);
    } else {
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

bool FormSrcReport::refreshCalc()
{
    QSqlQuery query;
    query.prepare("select * from econom_tree_src(:id)");
    query.bindValue(":id",id);
    bool ok = query.exec();
    if (ok){
        calcModel->refresh(query);
        calcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        calcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        calcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        calcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        calcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        calcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewCalc->resizeColumnToContents(0);
    } else {
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}
