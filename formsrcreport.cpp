#include "formsrsreport.h"
#include "ui_formsrsreport.h"

FormSrsReport::FormSrsReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSrsReport)
{
    ui->setupUi(this);
}

FormSrsReport::~FormSrsReport()
{
    delete ui;
}
