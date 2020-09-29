#include "dialogexportconf.h"
#include "ui_dialogexportconf.h"

DialogExportConf::DialogExportConf(TreeModel *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExportConf)
{
    ui->setupUi(this);
    for (int i=0; i<m->rowCount(); i++){
        FormExportItem *it = new FormExportItem(m->index(i,0),this);
        ui->verticalLayoutIt->addWidget(it);
        items.push_back(it);
    }
}

DialogExportConf::~DialogExportConf()
{
    delete ui;
}
