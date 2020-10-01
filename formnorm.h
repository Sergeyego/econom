#ifndef FORMNORM_H
#define FORMNORM_H

#include <QWidget>
#include <QComboBox>
#include <QFileDialog>
#include "modelro.h"
#include "db/dbtablemodel.h"
#include "xlsx/xlsxdocument.h"

namespace Ui {
class FormNorm;
}

using namespace QXlsx;

class FormNorm : public QWidget
{
    Q_OBJECT

public:
    explicit FormNorm(QWidget *parent = 0);
    ~FormNorm();

private:
    Ui::FormNorm *ui;
    ModelRo *modelProd;
    ModelRo *modelNomGroup;
    ModelRo *modelCost;
    DbRelationalModel *modelCostData;
    DbTableModel *modelNorm;
    int getId(QComboBox *c, int column=0);
    int currentIdProd();
    DbRelation *relIt;
    int copyIdNomGr;
    int copyIdCost;
    int copyProd;
    QString currentProd();

private slots:
    void refreshRels();
    void refreshProd();
    void refreshCostData();
    void refreshNorm();
    void copy();
    void paste();
    void save();
};

#endif // FORMNORM_H
