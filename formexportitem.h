#ifndef FORMEXPORTITEM_H
#define FORMEXPORTITEM_H

#include <QWidget>
#include "treemodel.h"

namespace Ui {
class FormExportItem;
}

class FormExportItem : public QWidget
{
    Q_OBJECT

public:
    explicit FormExportItem(QModelIndex root, QWidget *parent = 0);
    ~FormExportItem();

private:
    Ui::FormExportItem *ui;
};

#endif // FORMEXPORTITEM_H
