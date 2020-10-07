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
#include <QApplication>
#include "dialognewunload.h"
#include "progressreportdialog.h"

struct vids
{
    QString vid;
    int id_type;
    int id_prod;

    bool operator == (const vids& other) const {
        return vid==other.vid && id_type==other.id_type && id_prod==other.id_prod;
    }
};

class Import1C
{
public:
    Import1C();
    int importFromFile(QString filename);
    QString lastError() const;
    QString name() const;
private:
    QString afterTab(QString s);
    QString beforeTab(QString s);
    QMap <QString, int> costTypes;
    QMap <QString, int> costVid;
    QMap <QString, int> subdivs;
    QMap <QString, int> costs;
    QVector <vids> mvids;

    int addUnload(QString name, QDate dBeg, QDate dEnd);
    int getIdSubdiv(QString nam);
    int getIdNomGroup(QString nam, int id_subdiv);
    int getIdProd(QString nam, int id_nom_gr);
    int getIdCostItem(QString nam, int id_nom_group, int id_cost);
    int addIdNomGroup(QString nam, int id_subdiv);
    int addIdProd(QString nam, int id_nom_gr);
    int addIdCostItem(QString nam, int id_nom_group, int id_cost);
    bool insertItogo(int id_unload_it, double val);
    bool costExist(int id_unload_it, int id_cost, int id_type);
    int addNam(QString tablename, QString nam);
    bool refreshMap(QMap <QString, int> *map, QString tablename);
    double toDouble(QString n);
    int addUnloadItem(int id_unload, int id_prod, double kvo, double cost_price);
    int addUnloadItemCost(int id_unload_it, int id_cost, int id_type, double val);
    int addUnloadItemCostData(int id_unload_item_cost, int id_cost_item, double kvo, double price, double val);

    QString s_err;
    QString s_name;
};

#endif // IMPORT1C_H
