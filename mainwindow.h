#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QMdiSubWindow>
#include "import1c.h"
#include "formsrcreport.h"
#include "dialogopen.h"
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include "formnorm.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    FormSrcReport* activeMdiChild() const;
    FormSrcReport* newFormSrcReport(int id_report) const;
    FormNorm *formNorm;

private slots:
    void mdiSubActivated(QMdiSubWindow *w);
    void import();
    void open();
    void del();
    void calc();
    void cost();
};

#endif // MAINWINDOW_H
