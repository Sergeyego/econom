#include "formexportitem.h"
#include "ui_formexportitem.h"

FormExportItem::FormExportItem(QModelIndex root, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormExportItem)
{
    ui->setupUi(this);
    const TreeModel *m = qobject_cast<const TreeModel *>(root.model());
    if (m){
        qDebug()<<"ok";
    }
}

FormExportItem::~FormExportItem()
{
    delete ui;
}
