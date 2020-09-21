#ifndef FORMNORM_H
#define FORMNORM_H

#include <QWidget>
#include "modelro.h"

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
    ModelRo *modelCostData;

private slots:
    void refreshRels();
};

#endif // FORMNORM_H
