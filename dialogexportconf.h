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

private:
    Ui::DialogExportConf *ui;
    QVector<FormExportItem *> items;
};

#endif // DIALOGEXPORTCONF_H
