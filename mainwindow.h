#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStyle>
#include <QMdiSubWindow>
#include "import1c.h"
#include "formsrcreport.h"
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

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

private slots:
    void import();
};

#endif // MAINWINDOW_H
