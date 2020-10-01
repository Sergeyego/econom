#include "formnorm.h"
#include "ui_formnorm.h"

FormNorm::FormNorm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNorm)
{
    ui->setupUi(this);

    copyIdNomGr=-1;
    copyIdCost=-1;
    copyProd=-1;

    modelProd = new ModelRo(this);
    ui->tableViewProd->setModel(modelProd);

    modelNomGroup = new ModelRo(this);
    ui->comboBoxNomGroup->setModel(modelNomGroup);

    modelCost = new ModelRo(this);
    ui->comboBoxCost->setModel(modelCost);

    modelCostData = new DbRelationalModel(this);
    ui->listView->setModel(modelCostData);

    relIt = new DbRelation(modelCostData,0,1,this);

    modelNorm = new DbTableModel("econom_norm",this);
    modelNorm->addColumn("id_prod","id_prod");
    modelNorm->addColumn("id_nom_group","id_nom_group");
    modelNorm->addColumn("id_cost","id_cost");
    modelNorm->addColumn("id_cost_item",QString::fromUtf8("Элемент затрат"),relIt);
    modelNorm->addColumn("norm",QString::fromUtf8("Норма"));
    modelNorm->addColumn("kvo",QString::fromUtf8("На к-во"));


    modelNorm->setSort("econom_norm.id_cost_item");
    modelNorm->setDefaultValue(5,1000);
    modelNorm->setDecimals(4,2);

    ui->tableViewNorm->setModel(modelNorm);
    for (int i=0; i<3; i++){
        ui->tableViewNorm->setColumnHidden(i,true);
    }
    ui->tableViewNorm->setColumnWidth(3,230);
    ui->tableViewNorm->setColumnWidth(4,90);
    ui->tableViewNorm->setColumnWidth(5,80);

    connect(ui->comboBoxNomGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshProd()));
    connect(ui->comboBoxCost,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshCostData()));
    connect(ui->comboBoxNomGroup,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshCostData()));
    connect(ui->tableViewProd->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(refreshNorm()));
    connect(ui->pushButtonCopy,SIGNAL(clicked(bool)),this,SLOT(copy()));
    connect(ui->pushButtonPaste,SIGNAL(clicked(bool)),this,SLOT(paste()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(save()));

    refreshRels();
}

FormNorm::~FormNorm()
{
    delete ui;
}

int FormNorm::getId(QComboBox *c, int column)
{
    int id=-1;
    if (c->model()){
        if (c->model()->rowCount() && c->currentIndex()>=0){
            QModelIndex ind=c->model()->index(c->currentIndex(),column);
            id=c->model()->data(ind,Qt::EditRole).toInt();
        }
    }
    return id;
}

int FormNorm::currentIdProd()
{
    int id=-1;
    QModelIndex indProd=ui->tableViewProd->model()->index(ui->tableViewProd->currentIndex().row(),0);
    if (indProd.isValid()){
        id=ui->tableViewProd->model()->data(indProd,Qt::EditRole).toInt();
    }
    return id;
}

QString FormNorm::currentProd()
{
    QString prod;
    QModelIndex indProd=ui->tableViewProd->model()->index(ui->tableViewProd->currentIndex().row(),1);
    if (indProd.isValid()){
        prod=ui->tableViewProd->model()->data(indProd,Qt::EditRole).toString();
    }
    return prod;
}

void FormNorm::refreshRels()
{
    QSqlQuery queryNomGroup;
    queryNomGroup.prepare("select id, nam from econom_nom_group order by nam");
    if (modelNomGroup->execQuery(queryNomGroup)){
        ui->comboBoxNomGroup->setModelColumn(1);
        if(ui->comboBoxNomGroup->model()->rowCount()){
            ui->comboBoxNomGroup->setCurrentIndex(0);
        }
    }

    QSqlQuery queryCost;
    queryCost.prepare("select id, nam from econom_cost order by nam");
    if (modelCost->execQuery(queryCost)){
        ui->comboBoxCost->setModelColumn(1);
        if(ui->comboBoxCost->model()->rowCount()){
            ui->comboBoxCost->setCurrentIndex(0);
        }
    }
}

void FormNorm::refreshProd()
{
    int idng=getId(ui->comboBoxNomGroup);
    QSqlQuery query;
    query.prepare("select id, nam from econom_production where id_nom_group = :idng order by nam");
    query.bindValue(":idng",idng);
    if (modelProd->execQuery(query)){
        modelProd->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Наименование"));
        ui->tableViewProd->setColumnHidden(0,true);
        ui->tableViewProd->setColumnWidth(1,200);
        if (modelProd->rowCount()){
            ui->tableViewProd->setCurrentIndex(ui->tableViewProd->model()->index(0,1));
        }
    }
    refreshNorm();
}

void FormNorm::refreshCostData()
{
    int idng=getId(ui->comboBoxNomGroup);
    int idcost=getId(ui->comboBoxCost);
    QString query=QString("select id, nam from econom_cost_item where id_nom_group = %1 and id_cost = %2 order by nam").arg(idng).arg(idcost);
    if (modelCostData->setQuery(query)){
        modelCostData->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Наименование"));
        ui->listView->setModelColumn(1);
    }
    refreshNorm();
}

void FormNorm::refreshNorm()
{
    int id_prod=currentIdProd();
    int idng=getId(ui->comboBoxNomGroup);
    int idcost=getId(ui->comboBoxCost);

    modelNorm->setDefaultValue(0,id_prod);
    modelNorm->setDefaultValue(1,idng);
    modelNorm->setDefaultValue(2,idcost);

    modelNorm->setFilter(QString("econom_norm.id_prod = %1 and econom_norm.id_nom_group = %2 and econom_norm.id_cost = %3").arg(id_prod).arg(idng).arg(idcost));
    modelNorm->select();

    ui->pushButtonPaste->setEnabled((copyIdNomGr==idng) && (copyIdCost==idcost) && (copyProd!=id_prod));
}

void FormNorm::copy()
{
    copyIdNomGr=getId(ui->comboBoxNomGroup);
    copyIdCost=getId(ui->comboBoxCost);
    copyProd=currentIdProd();
    ui->pushButtonPaste->setEnabled(false);
}

void FormNorm::paste()
{
    int id_prod=currentIdProd();
    int idng=getId(ui->comboBoxNomGroup);
    int idcost=getId(ui->comboBoxCost);
    if ((copyIdNomGr==idng) && (copyIdCost==idcost) && (copyProd!=id_prod)){
        QSqlQuery query;
        query.prepare("insert into econom_norm (id_prod, id_nom_group, id_cost, id_cost_item, norm, kvo) "
                      "select :prod, id_nom_group, id_cost, id_cost_item, norm, kvo from econom_norm "
                      "where id_prod = :idprod and id_nom_group = :idng and id_cost = :idcost");
        query.bindValue(":prod",id_prod);
        query.bindValue(":idprod",copyProd);
        query.bindValue(":idng",copyIdNomGr);
        query.bindValue(":idcost",copyIdCost);
        if (query.exec()){
            refreshNorm();
        } else {
            QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
        }
    }
}

void FormNorm::save()
{
    QSqlQuery query;
    query.prepare("select ec.nam, eci.nam, en.norm from econom_norm en "
                  "inner join econom_nom_group eng on eng.id=en.id_nom_group "
                  "inner join econom_cost ec on ec.id=en.id_cost "
                  "inner join econom_cost_item eci on eci.id = en.id_cost_item "
                  "where en.id_prod= :id_prod and en.id_nom_group= :id_ng "
                  "order by en.id_cost_item, eci.nam ");
    query.bindValue(":id_prod",currentIdProd());
    query.bindValue(":id_ng",getId(ui->comboBoxNomGroup));
    if (query.exec()){
        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();
        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        pageSetup.orientation=XlsxPageSetup::portrait;
        ws->setPageSetup(pageSetup);

        XlsxPageMargins margins=ws->pageMargins();
        margins.bottom=0.817361111111111;
        ws->setPageMargins(margins);

        QFont defaultFont("Arial", 10);
        QFont strHeaderFont("Arial", 10);
        QFont headerFont("Arial", 10);
        QFont titleFont("Arial", 12);
        titleFont.setBold(true);
        strHeaderFont.setBold(true);

        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);

        Format strHeaderFormat;
        strHeaderFormat.setBorderStyle(Format::BorderThin);
        strHeaderFormat.setFont(strHeaderFont);

        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);
        QString fmt=QString("#,##0.%1").arg((0),2,'d',0,QChar('0'));
        numFormat.setNumberFormat(fmt);
        Format titleFormat;
        titleFormat.setBorderStyle(Format::BorderNone);
        titleFormat.setFont(titleFont);
        titleFormat.setTextWarp(true);
        titleFormat.setHorizontalAlignment(Format::AlignHCenter);
        titleFormat.setVerticalAlignment(Format::AlignVCenter);

        Format headerFormat;
        headerFormat.setBorderStyle(Format::BorderThin);
        headerFormat.setFont(headerFont);
        headerFormat.setTextWarp(true);
        headerFormat.setHorizontalAlignment(Format::AlignLeft);
        headerFormat.setVerticalAlignment(Format::AlignVCenter);

        ws->setColumnWidth(1,1,60);
        ws->setColumnWidth(2,2,11);

        QString title=QString::fromUtf8("Нормы расхода на производство 1 тонны ")+currentProd();
        int n=1;
        ws->mergeCells(CellRange(n,1,n,2));
        ws->writeString(n,1,title,titleFormat);
        n++;

        QString cost_old;

        while (query.next()){
            QString cost=query.value(0).toString();
            if (cost!=cost_old){
                n++;
                ws->mergeCells(CellRange(n,1,n,2));
                ws->writeString(n,1,cost,titleFormat);
                n++;
                ws->writeString(n,1,QString::fromUtf8("Наименование"),strHeaderFormat);
                ws->writeString(n,2,QString::fromUtf8("Норма"),strHeaderFormat);
            }
            cost_old=cost;
            n++;
            ws->writeString(n,1,query.value(1).toString(),strFormat);
            ws->writeNumeric(n,2,query.value(2).toDouble(),numFormat);
        }

        QDir dir(QDir::homePath());
        QString fnam=title.replace(QRegExp("[^\\w]"), "_");
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                        dir.path()+"/"+fnam+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    } else {
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }

}
