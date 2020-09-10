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

    QSqlQuery query;
    query.prepare("select * from econom_tree_src(:id)");
    query.bindValue(":id",id_report);
    srcModel->refresh(query);
}

FormSrcReport::~FormSrcReport()
{
    delete ui;
}
