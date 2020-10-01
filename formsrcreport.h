#ifndef FORMSRCREPORT_H
#define FORMSRCREPORT_H

#include <QWidget>
#include "treemodel.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "executor.h"
#include "formcost.h"
#include "xlsx/xlsxdocument.h"
#include "dialogexportconf.h"

namespace Ui {
class FormSrcReport;
}

using namespace QXlsx;

class FormSrcReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormSrcReport(int id_report, QWidget *parent = 0);
    ~FormSrcReport();

public slots:
    bool deleteReport();
    bool refreshSrc();
    bool refreshCalc();
    bool recalc();
    void srcFinished();
    void repFinished();
    void viewFormCost();
    void exportXlsx();

private:
    Ui::FormSrcReport *ui;
    int id;
    TreeModel *srcModel;
    TreeModel *calcModel;
    Executor *exeSrc;
    Executor *exeRep;
    Executor *exeCalc;
    bool existCalc();
    FormCost *formCost;
};

#endif // FORMSRCREPORT_H
