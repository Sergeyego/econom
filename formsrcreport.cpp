#include "formsrcreport.h"
#include "ui_formsrcreport.h"

FormSrcReport::FormSrcReport(int id_report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSrcReport)
{
    ui->setupUi(this);
    id=id_report;

    formCost = new FormCost(id);

    srcModel = new TreeModel(this);
    ui->treeViewSrc->setModel(srcModel);
    exeSrc = new Executor(QString::fromUtf8("Загрузка исходных данных"),srcModel,this);
    exeSrc->setTitle(QString::fromUtf8("Загрузка данных"));
    connect(exeSrc,SIGNAL(finished()),this,SLOT(srcFinished()));

    calcModel = new TreeModel(this);
    ui->treeViewCalc->setModel(calcModel);

    exeRep = new Executor(QString::fromUtf8("Загрузка результата расчетов"),calcModel,this);
    exeRep->setTitle(QString::fromUtf8("Загрузка расчета"));
    connect(exeRep,SIGNAL(finished()),this,SLOT(repFinished()));

    exeCalc = new Executor(QString::fromUtf8("Расчет себестоимости"),nullptr,this);
    connect(exeCalc,SIGNAL(finished()),this,SLOT(refreshCalc()));

    refreshSrc();

    if (existCalc()){
        refreshCalc();
    }
}

FormSrcReport::~FormSrcReport()
{
    delete formCost;
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
    QString query=QString("select cost, kvov, cost_price, kvo, price, val from econom_tree_src( %1 )").arg(id);
    exeSrc->setQuery(query);
    exeSrc->start();
    return true;
}

bool FormSrcReport::refreshCalc()
{
    QString query=QString("select cost, kvov, cost_price, kvo, price, val from econom_tree_calc(%1)").arg(id);
    exeRep->setQuery(query);
    exeRep->start();
    return true;
}

bool FormSrcReport::recalc()
{
    exeCalc->setQuery(QString("select * from  econom_calc(%1)").arg(id));
    exeCalc->start();
    return true;
}

void FormSrcReport::srcFinished()
{
    if (srcModel->columnCount()>=6){
        srcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        srcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        srcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        srcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        srcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        srcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewSrc->resizeColumnToContents(0);
    }
}

void FormSrcReport::repFinished()
{
    if (calcModel->columnCount()>=6){
        calcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        calcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        calcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        calcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        calcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        calcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewCalc->resizeColumnToContents(0);
    }
}

void FormSrcReport::viewFormCost()
{
    formCost->setWindowTitle(QString::fromUtf8("Затраты: ")+this->windowTitle());
    formCost->show();
}

bool FormSrcReport::existCalc()
{
    bool exist=false;
    QSqlQuery query;
    query.prepare("select count(*) from econom_unload_items_cost_data_calc euicdc "
                  "where id_item in "
                  "(select id from econom_unload_items_cost where id_unload in "
                  "(select id from econom_unload_items where id_unload = :id ));");
    query.bindValue(":id",id);
    if (query.exec()){
        while(query.next()){
            exist=query.value(0).toInt()>0;
        }
    } else {
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return exist;
}
