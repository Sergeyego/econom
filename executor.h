#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>
#include "progressreportdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "treemodel.h"

class Executor : public QThread
{
    Q_OBJECT
public:
    explicit Executor(QString mes, QAbstractItemModel *model, QObject *parent = 0);
    ~Executor();
    void run();
    void setQuery(QString q);
    void setTitle(QString s);

signals:
     void sigError(QString);

private:
    QAbstractItemModel *report;
    ProgressReportDialog *pprd;
    QString databaseName;
    QString hostName;
    int port;
    QString userName;
    QString password;
    QString query;

private slots:
    void showError(QString text);
};

#endif // EXECUTOR_H
