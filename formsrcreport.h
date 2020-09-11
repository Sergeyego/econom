#ifndef FORMSRCREPORT_H
#define FORMSRCREPORT_H

#include <QWidget>
#include "treemodel.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class FormSrcReport;
}

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

private:
    Ui::FormSrcReport *ui;
    int id;
    TreeModel *srcModel;
    TreeModel *calcModel;
};

#endif // FORMSRCREPORT_H
