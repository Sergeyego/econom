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
    bool isEnabled();
    QString onlyProd();

private:
    Ui::FormExportItem *ui;

private slots:
    void refreshState();
};

#endif // FORMEXPORTITEM_H
