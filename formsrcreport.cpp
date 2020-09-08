#include "formsrcreport.h"
#include "ui_formsrcreport.h"

FormSrcReport::FormSrcReport(int id_report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSrcReport)
{
    ui->setupUi(this);
    id=id_report;
}

FormSrcReport::~FormSrcReport()
{
    delete ui;
}
