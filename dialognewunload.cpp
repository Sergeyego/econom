#include "dialognewunload.h"
#include "ui_dialognewunload.h"

DialogNewUnload::DialogNewUnload(QString nam, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewUnload)
{
    ui->setupUi(this);    
    ui->lineEditNam->setText(nam);
    QRegExp reg(QString::fromUtf8("^.*\\sза\\s(\\w+)\\s(\\d+)\\sг.*$"));
    QDate d1(QDate::currentDate().year(),QDate::currentDate().month(),1);
    if (reg.indexIn(ui->lineEditNam->text())!=-1){
        QString m=reg.cap(1).toLower();
        int y=reg.cap(2).toInt();
        QStringList ml;
        ml<<QString::fromUtf8("январь")<<QString::fromUtf8("февраль")<<QString::fromUtf8("март")<<QString::fromUtf8("апрель")<<QString::fromUtf8("май")<<QString::fromUtf8("июнь");
        ml<<QString::fromUtf8("июль")<<QString::fromUtf8("август")<<QString::fromUtf8("сентябрь")<<QString::fromUtf8("октябрь")<<QString::fromUtf8("ноябрь")<<QString::fromUtf8("декабрь");
        if (ml.contains(m)){
            d1.setDate(y,ml.indexOf(m)+1,1);
        }
    }
    ui->dateEditBeg->setDate(d1);
    ui->dateEditEnd->setDate(d1.addMonths(1).addDays(-1));
}

DialogNewUnload::~DialogNewUnload()
{
    delete ui;
}

QString DialogNewUnload::name() const
{
    return ui->lineEditNam->text();
}

QDate DialogNewUnload::begDate() const
{
    return ui->dateEditBeg->date();
}

QDate DialogNewUnload::endDate() const
{
    return ui->dateEditEnd->date();
}
