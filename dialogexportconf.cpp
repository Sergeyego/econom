#include "dialogexportconf.h"
#include "ui_dialogexportconf.h"

DialogExportConf::DialogExportConf(TreeModel *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExportConf)
{
    ui->setupUi(this);
    for (int i=0; i<m->rowCount(); i++){
        QModelIndex root=m->index(i,0);
        for (int j=0; j<m->rowCount(root); j++){
            FormExportItem *it = new FormExportItem(m->index(j,0,root),this);
            ui->scrollAreaWidgetContents->layout()->addWidget(it);
            items.insert(m->index(j,0,root),it);
        }
    }
}

DialogExportConf::~DialogExportConf()
{
    delete ui;
}

bool DialogExportConf::isEnabled(QModelIndex ind)
{
    bool en=true;
    if (items.contains(ind)){
        en=items.value(ind)->isEnabled();
    }
    return en;
}

QString DialogExportConf::onlyProd(QModelIndex ind)
{
    QString nam;
    if (items.contains(ind)){
        nam=items.value(ind)->onlyProd();
    }
    return nam;
}

bool DialogExportConf::shortView()
{
    return !ui->checkBoxShort->isChecked();
}
