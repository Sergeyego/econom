#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    formNorm = new FormNorm;

    ui->actionImport->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight)));

    ui->actionOpen->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton)));
    ui->mainToolBar->addAction(ui->actionOpen);

    ui->actionCalc->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->mainToolBar->addAction(ui->actionCalc);

    ui->actionXlsx->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->mainToolBar->addAction(ui->actionXlsx);

    ui->actionCost->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView)));
    ui->mainToolBar->addAction(ui->actionCost);

    ui->actionDel->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_TrashIcon)));

    ui->actionExit->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)));

    connect(ui->actionImport,SIGNAL(triggered(bool)),this,SLOT(import()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),QApplication::instance(),SLOT(quit()));
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(open()));
    connect(ui->actionDel,SIGNAL(triggered(bool)),this,SLOT(del()));
    connect(ui->actionCalc,SIGNAL(triggered(bool)),this,SLOT(calc()));
    connect(ui->actionNorm,SIGNAL(triggered(bool)),formNorm,SLOT(show()));
    connect(ui->actionCost,SIGNAL(triggered(bool)),this,SLOT(cost()));

    connect(ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(mdiSubActivated(QMdiSubWindow*)));
}

MainWindow::~MainWindow()
{
    delete formNorm;
    delete ui;
}

FormSrcReport *MainWindow::activeMdiChild() const
{
    if (QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow()){
        return qobject_cast<FormSrcReport *>(activeSubWindow->widget());
    }
    return 0;
}

FormSrcReport *MainWindow::newFormSrcReport(int id_report) const
{
    FormSrcReport *fe = new FormSrcReport(id_report);
    fe->setAttribute(Qt::WA_DeleteOnClose);
    ui->mdiArea->addSubWindow(fe);
    fe->show();
    return fe;
}

void MainWindow::mdiSubActivated(QMdiSubWindow */*w*/)
{
    bool active=false;
    FormSrcReport *fsr = activeMdiChild();
    if (fsr){
        active=true;
    }
    ui->actionDel->setEnabled(active);
    ui->actionCalc->setEnabled(active);
    ui->actionXlsx->setEnabled(active);
    ui->actionCost->setEnabled(active);
}

void MainWindow::import()
{
    QSettings settings("szsm", QApplication::applicationName());
    QDir dir(settings.value("openPath",QDir::homePath()).toString());
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"),dir.path(), tr("txt Files (*.txt)"));
    if (!fileName.isEmpty()){
        QFile file(fileName);
        QFileInfo info(file);
        settings.setValue("openPath",info.path());
        Import1C im;
        int id_report=im.importFromFile(fileName);
        if (!im.lastError().isEmpty()){
            QMessageBox::critical(this,QObject::tr("Error"),im.lastError(),QMessageBox::Ok);
        }
        if (id_report!=-1){
            FormSrcReport *r = newFormSrcReport(id_report);
            r->setWindowTitle(im.name());
        }
    }
}

void MainWindow::open()
{
    DialogOpen o;
    if (o.exec()==QDialog::Accepted){
        int id=o.id();
        if (id!=-1){
            FormSrcReport *r = newFormSrcReport(o.id());
            r->setWindowTitle(o.name());
        }
    }
}

void MainWindow::del()
{
    FormSrcReport *r = activeMdiChild();
    if (r){
        int n=QMessageBox::question(this,QString("Подтвердите удаление"),QString("Подтверждаете удаление %1 ?").arg(r->windowTitle()),QMessageBox::Yes,QMessageBox::No);
        if (n==QMessageBox::Yes){
            if (r->deleteReport()){
                ui->mdiArea->closeActiveSubWindow();
            }
        }
    }
}

void MainWindow::calc()
{
    FormSrcReport *c = activeMdiChild();
    if (c){
        c->recalc();
    }
}

void MainWindow::cost()
{
    FormSrcReport *r = activeMdiChild();
    if (r){
        r->viewFormCost();
    }
}
