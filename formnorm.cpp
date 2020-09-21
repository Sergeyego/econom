#include "formnorm.h"
#include "ui_formnorm.h"

FormNorm::FormNorm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormNorm)
{
    ui->setupUi(this);
    modelProd = new ModelRo(this);
    ui->tableViewProd->setModel(modelProd);

    modelNomGroup = new ModelRo(this);
    ui->comboBoxNomGroup->setModel(modelNomGroup);

    modelCost = new ModelRo(this);
    ui->comboBoxCost->setModel(modelCost);

    modelCostData = new ModelRo(this);
    ui->listView->setModel(modelCostData);

    refreshRels();
}

FormNorm::~FormNorm()
{
    delete ui;
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
}
