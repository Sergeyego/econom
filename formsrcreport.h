#ifndef FORMSRCREPORT_H
#define FORMSRCREPORT_H

#include <QWidget>

namespace Ui {
class FormSrcReport;
}

class FormSrcReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormSrcReport(int id_report, QWidget *parent = 0);
    ~FormSrcReport();

private:
    Ui::FormSrcReport *ui;
    int id;
};

#endif // FORMSRCREPORT_H
