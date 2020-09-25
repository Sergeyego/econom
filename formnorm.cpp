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
