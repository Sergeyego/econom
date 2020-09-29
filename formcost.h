#ifndef FORMCOST_H
#define FORMCOST_H

#include <QWidget>
#include <QComboBox>
#include <QSqlQuery>
#include "modelro.h"
#include "db/dbxlsx.h"

namespace Ui {
class FormCost;
}

class FormCost : public QWidget
{
    Q_OBJECT

public:
    explicit FormCost(int id_unload, QWidget *parent = 0);
    ~FormCost();

private:
    Ui::FormCost *ui;
    ModelRo *modelNomGr;
    ModelRo *modelCost;
    ModelRo *modelInfo;
    int getId(QComboBox *c, int column=0);
    int id;

private slots:
    void refreshRel();
    void refreshInfo();
    void saveXlsx();
};

#endif // FORMCOST_H
