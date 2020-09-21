#ifndef FORMNORM_H
#define FORMNORM_H

#include <QWidget>
#include <QComboBox>
#include "modelro.h"
#include "db/dbtablemodel.h"

namespace Ui {
class FormNorm;
}

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

private slots:
    void refreshRels();
    void refreshProd();
    void refreshCostData();
    void refreshNorm();
};

#endif // FORMNORM_H
