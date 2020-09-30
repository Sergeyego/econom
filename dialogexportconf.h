#ifndef DIALOGEXPORTCONF_H
#define DIALOGEXPORTCONF_H

#include <QDialog>
#include "treemodel.h"
#include "formexportitem.h"

namespace Ui {
class DialogExportConf;
}

class DialogExportConf : public QDialog
{
    Q_OBJECT

public:
    explicit DialogExportConf(TreeModel *m, QWidget *parent = 0);
    ~DialogExportConf();
    bool isEnabled(QModelIndex ind);
    QString onlyProd(QModelIndex ind);
    bool shortView();

private:
    Ui::DialogExportConf *ui;
    QMap<QModelIndex, FormExportItem *> items;
};

#endif // DIALOGEXPORTCONF_H
