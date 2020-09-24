#ifndef PROGRESSREPORTDIALOG_H
#define PROGRESSREPORTDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ProgressReportDialog;
}

class ProgressReportDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProgressReportDialog(QWidget *parent = 0);
    ~ProgressReportDialog();
    void closeEvent(QCloseEvent *pe);
    void setMessage(QString mes);
    
private:
    Ui::ProgressReportDialog *ui;
};

#endif // PROGRESSREPORTDIALOG_H
