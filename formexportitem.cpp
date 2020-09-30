#include "formexportitem.h"
#include "ui_formexportitem.h"

FormExportItem::FormExportItem(QModelIndex root, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormExportItem)
{
    ui->setupUi(this);
    const TreeModel *m = qobject_cast<const TreeModel *>(root.model());
    if (m){
        ui->checkBoxName->setText(m->data(root,Qt::EditRole).toString());
        for (int i=0; i<m->rowCount(root); i++){
            ui->comboBoxProd->addItem(m->data(m->index(i,0,root),Qt::EditRole).toString());
        }
    }
    connect(ui->checkBoxName,SIGNAL(clicked(bool)),this,SLOT(refreshState()));
    connect(ui->radioButtonAll,SIGNAL(clicked(bool)),this,SLOT(refreshState()));
    connect(ui->radioButtonOne,SIGNAL(clicked(bool)),this,SLOT(refreshState()));
}

FormExportItem::~FormExportItem()
{
    delete ui;
}

bool FormExportItem::isEnabled()
{
    return ui->checkBoxName->isChecked();
}

QString FormExportItem::onlyProd()
{
    QString nam;
    if (ui->checkBoxName->isChecked() && ui->radioButtonOne->isChecked()){
        nam=ui->comboBoxProd->currentText();
    }
    return nam;
}

void FormExportItem::refreshState()
{
    if (ui->checkBoxName->isChecked()){
        ui->radioButtonAll->setEnabled(true);
        ui->radioButtonOne->setEnabled(true);
        ui->comboBoxProd->setEnabled(ui->radioButtonOne->isChecked());
    } else {
        ui->radioButtonAll->setEnabled(false);
        ui->radioButtonOne->setEnabled(false);
        ui->comboBoxProd->setEnabled(false);
    }
}
