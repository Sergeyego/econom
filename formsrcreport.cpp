#include "formsrcreport.h"
#include "ui_formsrcreport.h"

FormSrcReport::FormSrcReport(int id_report, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormSrcReport)
{
    ui->setupUi(this);
    id=id_report;

    formCost = new FormCost(id);

    srcModel = new TreeModel(this);
    ui->treeViewSrc->setModel(srcModel);
    exeSrc = new Executor(QString::fromUtf8("Загрузка исходных данных"),srcModel,this);
    exeSrc->setTitle(QString::fromUtf8("Загрузка данных"));
    connect(exeSrc,SIGNAL(finished()),this,SLOT(srcFinished()));

    calcModel = new TreeModel(this);
    ui->treeViewCalc->setModel(calcModel);

    exeRep = new Executor(QString::fromUtf8("Загрузка результата расчетов"),calcModel,this);
    exeRep->setTitle(QString::fromUtf8("Загрузка расчета"));
    connect(exeRep,SIGNAL(finished()),this,SLOT(repFinished()));

    exeCalc = new Executor(QString::fromUtf8("Расчет себестоимости"),nullptr,this);
    connect(exeCalc,SIGNAL(finished()),this,SLOT(refreshCalc()));

    refreshSrc();

    if (existCalc()){
        refreshCalc();
    }
}

FormSrcReport::~FormSrcReport()
{
    delete formCost;
    delete ui;
}

bool FormSrcReport::deleteReport()
{
    QSqlQuery query;
    query.prepare("select * from econom_delete_unload(:id)");
    query.bindValue(":id",id);
    bool ok=query.exec();
    if (!ok){
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return ok;
}

bool FormSrcReport::refreshSrc()
{
    QString query=QString("select cost, kvov, cost_price, kvo, price, val from econom_tree_src( %1 )").arg(id);
    exeSrc->setQuery(query);
    exeSrc->start();
    return true;
}

bool FormSrcReport::refreshCalc()
{
    QString query=QString("select cost, kvov, cost_price, kvo, price, val from econom_tree_calc(%1)").arg(id);
    exeRep->setQuery(query);
    exeRep->start();
    return true;
}

bool FormSrcReport::recalc()
{
    exeCalc->setQuery(QString("select * from  econom_calc(%1)").arg(id));
    exeCalc->start();
    return true;
}

void FormSrcReport::srcFinished()
{
    if (srcModel->columnCount()>=6){
        srcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        srcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        srcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        srcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        srcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        srcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewSrc->resizeColumnToContents(0);
    }
}

void FormSrcReport::repFinished()
{
    if (calcModel->columnCount()>=6){
        calcModel->setHeaderData(0,Qt::Horizontal,QString("Затраты"),Qt::DisplayRole);
        calcModel->setHeaderData(1,Qt::Horizontal,QString("К-во вып."),Qt::DisplayRole);
        calcModel->setHeaderData(2,Qt::Horizontal,QString("Себестоим.ед."),Qt::DisplayRole);
        calcModel->setHeaderData(3,Qt::Horizontal,QString("К-во"),Qt::DisplayRole);
        calcModel->setHeaderData(4,Qt::Horizontal,QString("Цена"),Qt::DisplayRole);
        calcModel->setHeaderData(5,Qt::Horizontal,QString("Сумма"),Qt::DisplayRole);
        ui->treeViewCalc->resizeColumnToContents(0);
    }
}

void FormSrcReport::viewFormCost()
{
    formCost->setWindowTitle(QString::fromUtf8("Затраты. ")+this->windowTitle());
    formCost->show();
}

void FormSrcReport::exportXlsx()
{
    DialogExportConf d(calcModel);
    if (d.exec()!=QDialog::Accepted){
        return;
    }

    Document xlsx;
    Worksheet *ws=xlsx.currentWorksheet();
    XlsxPageSetup pageSetup;
    pageSetup.fitToPage=true;
    pageSetup.fitToWidth=1;
    pageSetup.fitToHeight=0;
    pageSetup.orientation=XlsxPageSetup::portrait;
    ws->setPageSetup(pageSetup);

    XlsxPageMargins margins=ws->pageMargins();
    margins.bottom=0.817361111111111;
    ws->setPageMargins(margins);

    QFont defaultFont("Arial", 10);
    QFont strHeaderFont("Arial", 10);
    QFont headerFont("Arial", 12);
    QFont titleFont("Arial", 14);
    titleFont.setBold(true);
    strHeaderFont.setBold(true);

    Format strFormat;
    strFormat.setBorderStyle(Format::BorderThin);
    strFormat.setFont(defaultFont);

    Format strHeaderFormat;
    strHeaderFormat.setBorderStyle(Format::BorderThin);
    strHeaderFormat.setFont(strHeaderFont);

    Format numFormat;
    numFormat.setBorderStyle(Format::BorderThin);
    numFormat.setFont(defaultFont);
    QString fmt=QString("#,##0.%1").arg((0),2,'d',0,QChar('0'));
    numFormat.setNumberFormat(fmt);
    Format titleFormat;
    titleFormat.setBorderStyle(Format::BorderNone);
    titleFormat.setFont(titleFont);
    titleFormat.setTextWarp(true);
    titleFormat.setHorizontalAlignment(Format::AlignHCenter);
    titleFormat.setVerticalAlignment(Format::AlignVCenter);

    Format headerFormat;
    headerFormat.setBorderStyle(Format::BorderThin);
    headerFormat.setFont(headerFont);
    headerFormat.setTextWarp(true);
    headerFormat.setHorizontalAlignment(Format::AlignLeft);
    headerFormat.setVerticalAlignment(Format::AlignVCenter);

    ws->setColumnWidth(1,1,60);
    ws->setColumnWidth(2,2,11);
    ws->setColumnWidth(3,3,11);
    ws->setColumnWidth(4,4,11);
    ws->setColumnWidth(5,5,11);

    int n=1;
    ws->mergeCells(CellRange(n,1,n,5));
    ws->writeString(n,1,windowTitle(),titleFormat);
    n++;

    for (int i=0; i<calcModel->rowCount(); i++){//подразделение
        QModelIndex ind=calcModel->index(i,0);
        QModelIndex indval=calcModel->index(i,5);
        bool existel=false;
        for (int p=0; p<calcModel->rowCount(ind); p++){
            QModelIndex inde=calcModel->index(p,0,ind);
            existel=existel || d.isEnabled(inde);
        }
        if (!existel){
            continue;
        }
        n++;
        n++;
        int startp=n+1;
        titleFormat.setPatternBackgroundColor(calcModel->data(ind,Qt::BackgroundColorRole).value<QColor>());
        numFormat.setPatternBackgroundColor(calcModel->data(ind,Qt::BackgroundColorRole).value<QColor>());
        strFormat.setPatternBackgroundColor(calcModel->data(ind,Qt::BackgroundColorRole).value<QColor>());
        ws->setRowHeight(n,n,30);
        ws->writeString(n,1,QString::fromUtf8("Подразделение ")+calcModel->data(ind,Qt::EditRole).toString(),titleFormat);
        ws->mergeCells(CellRange(n,2,n,4));
        ws->writeString(n,2,QString::fromUtf8("Затрачено итого"),strFormat);
        ws->writeNumeric(n,5,calcModel->data(indval,Qt::EditRole).toDouble(),numFormat);
        for (int j=0; j<calcModel->rowCount(ind); j++){//номенклатурная группа
            QModelIndex ind1=calcModel->index(j,0,ind);
            QModelIndex indval1=calcModel->index(j,5,ind);
            if (!d.isEnabled(ind1)){
                continue;
            }
            n++;
            n++;
            int startng=n+1;
            headerFormat.setPatternBackgroundColor(calcModel->data(ind1,Qt::BackgroundColorRole).value<QColor>());
            numFormat.setPatternBackgroundColor(calcModel->data(ind1,Qt::BackgroundColorRole).value<QColor>());
            strFormat.setPatternBackgroundColor(calcModel->data(ind1,Qt::BackgroundColorRole).value<QColor>());
            ws->setRowHeight(n,n,30);
            ws->writeString(n,1,QString::fromUtf8("Номенклатурная группа ")+calcModel->data(ind1,Qt::EditRole).toString(),headerFormat);
            ws->mergeCells(CellRange(n,2,n,4));
            ws->writeString(n,2,QString::fromUtf8("Затрачено итого"),strFormat);
            ws->writeNumeric(n,5,calcModel->data(indval1,Qt::EditRole).toDouble(),numFormat);
            for (int k=0; k<calcModel->rowCount(ind1); k++){//продукция
                QModelIndex ind2=calcModel->index(k,0,ind1);
                QModelIndex indval2=calcModel->index(k,5,ind1);
                QModelIndex indkvo2=calcModel->index(k,1,ind1);
                QModelIndex indcost2=calcModel->index(k,2,ind1);
                if (!d.onlyProd(ind1).isEmpty() && d.onlyProd(ind1)!=calcModel->data(ind2,Qt::EditRole).toString()){
                    continue;
                }
                n++;
                n++;
                int startprod=n+1;
                headerFormat.setPatternBackgroundColor(calcModel->data(ind2,Qt::BackgroundColorRole).value<QColor>());
                numFormat.setPatternBackgroundColor(calcModel->data(ind2,Qt::BackgroundColorRole).value<QColor>());
                strFormat.setPatternBackgroundColor(calcModel->data(ind2,Qt::BackgroundColorRole).value<QColor>());
                strHeaderFormat.setPatternBackgroundColor(calcModel->data(ind2,Qt::BackgroundColorRole).value<QColor>());
                ws->writeString(n,1,calcModel->data(ind2,Qt::EditRole).toString(),headerFormat);
                ws->mergeCells(CellRange(n,2,n,4));
                ws->writeString(n,2,QString::fromUtf8("Затрачено итого"),strFormat);
                ws->writeNumeric(n,5,calcModel->data(indval2,Qt::EditRole).toDouble(),numFormat);
                n++;
                strFormat.setPatternBackgroundColor(Qt::white);
                numFormat.setPatternBackgroundColor(Qt::white);
                ws->writeString(n,1,QString::fromUtf8("Количество выпуска"),strFormat);
                ws->writeNumeric(n,2,calcModel->data(indkvo2,Qt::EditRole).toDouble(),numFormat);
                n++;
                ws->writeString(n,1,QString::fromUtf8("Себестоимость единицы"),strFormat);
                ws->writeNumeric(n,2,calcModel->data(indcost2,Qt::EditRole).toDouble(),numFormat);
                n++;
                ws->mergeCells(CellRange(n,1,n,2));
                ws->setRowHeight(n,n,20);
                ws->writeString(n,1,QString::fromUtf8("Затраты"),strHeaderFormat);
                ws->writeString(n,3,QString::fromUtf8("Количество"),strHeaderFormat);
                ws->writeString(n,4,QString::fromUtf8("Цена"),strHeaderFormat);
                ws->writeString(n,5,QString::fromUtf8("Сумма"),strHeaderFormat);
                for (int l=0; l<calcModel->rowCount(ind2); l++){//группа затрат
                    QModelIndex ind3=calcModel->index(l,0,ind2);
                    QModelIndex indval3=calcModel->index(l,5,ind2);
                    n++;
                    int startgr=n+1;
                    strFormat.setPatternBackgroundColor(calcModel->data(ind3,Qt::BackgroundColorRole).value<QColor>());
                    numFormat.setPatternBackgroundColor(calcModel->data(ind3,Qt::BackgroundColorRole).value<QColor>());
                    ws->mergeCells(CellRange(n,1,n,2));
                    ws->writeString(n,1,calcModel->data(ind3,Qt::EditRole).toString(),strFormat);
                    ws->writeBlank(n,3,numFormat);
                    ws->writeBlank(n,4,numFormat);
                    ws->writeNumeric(n,5,calcModel->data(indval3,Qt::EditRole).toDouble(),numFormat);
                    for (int m=0; m<calcModel->rowCount(ind3); m++){//статья затрат
                        QModelIndex ind4=calcModel->index(m,0,ind3);
                        QModelIndex indval4=calcModel->index(m,5,ind3);
                        n++;
                        int startst=n+1;
                        strFormat.setPatternBackgroundColor(calcModel->data(ind4,Qt::BackgroundColorRole).value<QColor>());
                        numFormat.setPatternBackgroundColor(calcModel->data(ind4,Qt::BackgroundColorRole).value<QColor>());
                        ws->mergeCells(CellRange(n,1,n,2));
                        ws->writeString(n,1,calcModel->data(ind4,Qt::EditRole).toString(),strFormat);
                        ws->writeBlank(n,3,numFormat);
                        ws->writeBlank(n,4,numFormat);
                        ws->writeNumeric(n,5,calcModel->data(indval4,Qt::EditRole).toDouble(),numFormat);
                        for (int e=0; e<calcModel->rowCount(ind4); e++){//элемент статьи затрат
                            QModelIndex ind5=calcModel->index(e,0,ind4);
                            QModelIndex indkvo5=calcModel->index(e,3,ind4);
                            QModelIndex indprice5=calcModel->index(e,4,ind4);
                            QModelIndex indval5=calcModel->index(e,5,ind4);
                            n++;
                            strFormat.setPatternBackgroundColor(calcModel->data(ind5,Qt::BackgroundColorRole).value<QColor>());
                            numFormat.setPatternBackgroundColor(calcModel->data(ind5,Qt::BackgroundColorRole).value<QColor>());
                            ws->mergeCells(CellRange(n,1,n,2));
                            ws->writeString(n,1,calcModel->data(ind5,Qt::EditRole).toString(),strFormat);
                            ws->writeNumeric(n,3,calcModel->data(indkvo5,Qt::EditRole).toDouble(),numFormat);
                            ws->writeNumeric(n,4,calcModel->data(indprice5,Qt::EditRole).toDouble(),numFormat);
                            ws->writeNumeric(n,5,calcModel->data(indval5,Qt::EditRole).toDouble(),numFormat);
                        }
                        ws->groupRows(startst,n,d.shortView());
                    }
                    ws->groupRows(startgr,n,false);
                }
                ws->groupRows(startprod,n,false);
            }
            ws->groupRows(startng,n,false);
        }
        ws->groupRows(startp,n,false);
    }

    QDir dir(QDir::homePath());
    QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить файл"),
                                                    dir.path()+"/"+windowTitle()+".xlsx",
                                                    QString::fromUtf8("Documents (*.xlsx)") );
    if (!filename.isEmpty()){
        xlsx.saveAs(filename);
    }
}

bool FormSrcReport::existCalc()
{
    bool exist=false;
    QSqlQuery query;
    query.prepare("select count(*) from econom_unload_items_cost_data_calc euicdc "
                  "where id_item in "
                  "(select id from econom_unload_items_cost where id_unload in "
                  "(select id from econom_unload_items where id_unload = :id ));");
    query.bindValue(":id",id);
    if (query.exec()){
        while(query.next()){
            exist=query.value(0).toInt()>0;
        }
    } else {
        QMessageBox::critical(this,QObject::tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
    return exist;
}
