#ifndef IMPORT1C_H
#define IMPORT1C_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QDate>
#include <QProgressDialog>
#include <QApplication>
#include "dialognewunload.h"

class Import1C
{
public:
    Import1C();
    int importFromFile(QString filename);
    QString lastError() const;
private:
    QString afterTab(QString s);
    QString beforeTab(QString s);
    QMap <QString, int> costTypes;
    int addUnload(QString name, QDate dBeg, QDate dEnd);
    int getIdSubdiv(QString nam);
    int getIdNomGroup(QString nam);
    int getIdProd(QString nam);
    int getIdCost(QString nam);
    int getIdCostItem(QString nam);
    int addNam(QString tablename, QString nam);
    double toDouble(QString n);
    int addUnloadItem(int id_unload, int id_subdiv, int id_nom_group, int id_prod, double kvo, double cost_price);
    int addUnloadItemCost(int id_unload_it, int id_cost, int id_type, double val);
    int addUnloadItemCostData(int id_unload_item_cost, int id_cost_item, double kvo, double price, double val);
    QString s_err;
};

#endif // IMPORT1C_H
