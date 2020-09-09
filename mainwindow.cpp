#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionImport->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight)));

    ui->actionOpen->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOpenButton)));
    ui->mainToolBar->addAction(ui->actionOpen);

    ui->actionCalc->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->mainToolBar->addAction(ui->actionCalc);

    ui->actionXlsx->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->mainToolBar->addAction(ui->actionXlsx);


    ui->actionExit->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton)));

    connect(ui->actionImport,SIGNAL(triggered(bool)),this,SLOT(import()));
    connect(ui->actionExit,SIGNAL(triggered(bool)),QApplication::instance(),SLOT(quit()));
    connect(ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(open()));

}

MainWindow::~MainWindow()
{
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
    //connect(fe,SIGNAL(saveEnableChanged(bool)),this,SLOT(saveEnable(bool)));
    fe->show();
    return fe;
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
        FormSrcReport *r = newFormSrcReport(o.id());
        r->setWindowTitle(o.name());
    }
}
