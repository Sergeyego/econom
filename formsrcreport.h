#ifndef FORMSRSREPORT_H
#define FORMSRSREPORT_H

#include <QWidget>

namespace Ui {
class FormSrsReport;
}

class FormSrsReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormSrsReport(QWidget *parent = 0);
    ~FormSrsReport();

private:
    Ui::FormSrsReport *ui;
};

#endif // FORMSRSREPORT_H
