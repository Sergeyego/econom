#include "formcost.h"
#include "ui_formcost.h"

FormCost::FormCost(int id_unload, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCost)
{
    ui->setupUi(this);
    id=id_unload;

    ui->pushButtonSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));

    modelNomGr = new ModelRo(this);
    ui->comboBoxNomGr->setModel(modelNomGr);

    modelCost = new ModelRo(this);
    ui->comboBoxCost->setModel(modelCost);

    modelInfo = new ModelRo(this);
    ui->tableViewCost->setModel(modelInfo);

    refreshRel();

    connect(ui->comboBoxNomGr,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshInfo()));
    connect(ui->comboBoxCost,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshInfo()));
    connect(ui->pushButtonSave,SIGNAL(clicked(bool)),this,SLOT(saveXlsx()));

    refreshInfo();
}

FormCost::~FormCost()
{
    delete ui;
}

int FormCost::getId(QComboBox *c, int column)
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

void FormCost::refreshRel()
{
    QSqlQuery queryNomGroup;
    queryNomGroup.prepare("select id, nam from econom_nom_group order by nam");
    if (modelNomGr->execQuery(queryNomGroup)){
        ui->comboBoxNomGr->setModelColumn(1);
        if(ui->comboBoxNomGr->model()->rowCount()){
            ui->comboBoxNomGr->setCurrentIndex(0);
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

void FormCost::refreshInfo()
{
    int id_nom_gr=getId(ui->comboBoxNomGr);
    int id_cost=getId(ui->comboBoxCost);
    QSqlQuery query;
    query.prepare("select nam, sum(kvo), sum(norm), avg(price), sum(val) from ( "
                  "select eci.nam as nam, sum(euicd.kvo) as kvo, coalesce(sum(en.norm/en.kvo*eui.kvo),0.0) as norm, "
                  "avg(euicd.price) as price, sum(euicd.val) as val from econom_unload_items eui "
                  "inner join econom_unload_items_cost euic on euic.id_unload = eui.id "
                  "inner join econom_unload_items_cost_data euicd on euicd.id_item = euic.id "
                  "inner join econom_cost_item eci on eci.id =euicd.id_cost_item "
                  "inner join econom_production ep on ep.id = eui.id_prod "
                  "left join econom_norm en on en.id_prod=ep.id and en.id_nom_group = ep.id_nom_group and en.id_cost = euic.id_cost and en.id_cost_item = euicd.id_cost_item "
                  "where eui.id_unload = :id and ep.id_nom_group = :idng and euic.id_cost = :idcost "
                  "group by eci.nam, en.norm, en.kvo "
                  "order by eci.nam "
                  ") as z "
                  "group by z.nam "
                  "order by z.nam");
    query.bindValue(":id",id);
    query.bindValue(":idng",id_nom_gr);
    query.bindValue(":idcost",id_cost);
    if (modelInfo->execQuery(query)){
        modelInfo->setHeaderData(0,Qt::Horizontal,QString::fromUtf8("Наименование компонента"));
        modelInfo->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("Кол-во"));
        modelInfo->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("Кол-во по норме"));
        modelInfo->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("Сред.цена"));
        modelInfo->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("Стоим."));
        ui->tableViewCost->resizeColumnsToContents();
    }
}

void FormCost::saveXlsx()
{
    DbXlsx d(ui->tableViewCost,ui->comboBoxNomGr->currentText()+". "+ui->comboBoxCost->currentText());
    d.saveToFile();
}
