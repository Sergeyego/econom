#include "executor.h"

Executor::Executor(QString mes, QAbstractItemModel *model, QObject *parent) :
    QThread(parent)
{
    report=model;
    pprd = new ProgressReportDialog();
    pprd->setMessage(mes);

    QSqlDatabase db=QSqlDatabase::database();
    databaseName=db.databaseName();
    hostName=db.hostName();
    port=db.port();
    userName=db.userName();
    password=db.password();

    connect(this,SIGNAL(started()),pprd,SLOT(show()));
    connect(this,SIGNAL(finished()),pprd,SLOT(hide()));
    connect(this,SIGNAL(sigError(QString)),this,SLOT(showError(QString)));
}

Executor::~Executor()
{
    delete pprd;
}

void Executor::run()
{  
    int randInt=0;
    QString baseName=QString("CONN");
    QString name=baseName+QString::number(randInt);
    while (QSqlDatabase::contains(name)){
        randInt++;
        name=baseName+QString::number(randInt);
    }
    //qDebug() << name;
    {
        QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL",name);
        db.setDatabaseName(databaseName);
        db.setHostName(hostName);
        db.setPort(port);
        db.setUserName(userName);
        db.setPassword(password);
        if (db.open()){
            QSqlQuery qu(db);
            qu.prepare(query);
            if (qu.exec()){
                if (report){
                    TreeModel *model = qobject_cast<TreeModel *>(report);
                    if (model){
                        model->refresh(qu);
                    }
                }
            } else {
                emit sigError(qu.lastError().text());
            }
        } else {
            emit sigError(db.lastError().text());
        }
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(name);
}

void Executor::setQuery(QString q)
{
    query=q;
}

void Executor::setTitle(QString s)
{
    pprd->setWindowTitle(s);
}

void Executor::showError(QString text)
{
    QMessageBox::critical(NULL,tr("Ошибка"),text,QMessageBox::Ok);
}
