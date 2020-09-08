#include "import1c.h"

Import1C::Import1C()
{
    QSqlQuery query;
    query.prepare("select id, nam from econom_cost_type order by nam");
    if (query.exec()){
        while(query.next()){
            costTypes.insert(query.value(1).toString(),query.value(0).toInt());
        }
    } else {
        s_err=query.lastError().text();
    }
}

int Import1C::importFromFile(QString filename)
{
    int id=-1;
    int id_subdiv=-1;
    int id_nom_gr=-1;
    int id_unload_it=-1;
    int id_type=-1;
    int id_cost=-1;
    int id_unload_it_cost;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("UTF-8"));
        in.readLine();
        DialogNewUnload d(in.readLine());
        if (d.exec()==QDialog::Accepted){
            id=addUnload(d.name(),d.begDate(),d.endDate());
            if (id!=-1){
                QProgressDialog *progress = new QProgressDialog();
                progress->setWindowTitle(QString::fromUtf8("Загрузка данных"));
                progress->setAutoClose(false);
                progress->setCancelButton(NULL);
                progress->setMinimumDuration(0);
                progress->setMaximum(0);
                progress->setValue(-1);

                while(!in.atEnd()){
                    QString str=in.readLine();
                    if (str.contains(QRegExp("^Подразделение\t(.*)$"))){
                        qDebug()<<"подр: "<<afterTab(str);
                        id_subdiv=getIdSubdiv(afterTab(str));
                    } else if (str.contains(QRegExp("^Номенклатурная группа\t(.*)$"))){
                        qDebug()<<"номгр: "<<afterTab(str);
                        id_nom_gr=getIdNomGroup(afterTab(str));
                    } else if (str.contains(QRegExp("^(Услуга|Продукция)\t(.*)$"))){
                        QString prod, kvo, costPrice;
                        str=in.readLine();
                        prod=beforeTab(str);
                        in.readLine();//пустая строка
                        str=in.readLine();
                        if (str.contains(QRegExp("^Количество выпуска\t(.*)$"))){
                            kvo=afterTab(str);
                        }
                        str=in.readLine();
                        if (str.contains(QRegExp("^Себестоимость единицы\t(.*)$"))){
                            costPrice=afterTab(str);
                        }

                        int id_prod=getIdProd(prod);
                        double dkvo=toDouble(kvo);
                        double dcostPrice=toDouble(costPrice);
                        id_unload_it=addUnloadItem(id,id_subdiv,id_nom_gr,id_prod,dkvo,dcostPrice);

                        qDebug()<<"прод: "<<id_prod<<" "<<prod<<" kvo= "<<dkvo<<" costPrice= "<<dcostPrice;
                        progress->setLabelText(QString::fromUtf8("Обработка: ")+prod);

                        in.readLine();//пустая строка
                        in.readLine();//шапка таблицы
                        in.readLine();//пустая строка
                        in.readLine();//пустая строка
                    } else {
                        QStringList list=str.split("\t");
                        if (list.size()==4){
                            if (costTypes.contains(list.at(0))){
                                qDebug()<<list<<" id="<<costTypes.value(list.at(0));
                                id_type=costTypes.value(list.at(0));
                            } else if (QString(list.at(1)).isEmpty() && QString(list.at(2)).isEmpty()){
                                if (list.at(0)!=QString("Итого")){
                                    qDebug()<<"ст. звтрат: "<<list;
                                    id_cost=getIdCost(list.at(0));
                                    double val=toDouble(list.at(3));
                                    id_unload_it_cost=addUnloadItemCost(id_unload_it,id_cost,id_type,val);
                                }
                            } else {
                                qDebug()<<"--эл: "<<list;
                                int id_cost_item=getIdCostItem(list.at(0));
                                double kvo=toDouble(list.at(1));
                                double price=toDouble(list.at(2));
                                double val=toDouble(list.at(3));
                                addUnloadItemCostData(id_unload_it_cost,id_cost_item,kvo,price,val);
                            }
                        }
                    }
                    QApplication::processEvents();
                    if (!s_err.isEmpty()){
                        break;
                    }
                }
                delete progress;
            }
        }
        file.close();
    } else {
        s_err=QObject::tr("Ошибка открытия файла");
    }
    return(id);
}

QString Import1C::lastError() const
{
    return s_err;
}

QString Import1C::afterTab(QString s)
{
    return s.mid(s.indexOf("\t")+1);
}

QString Import1C::beforeTab(QString s)
{
    return s.mid(0,s.indexOf("\t"));
}

int Import1C::addUnload(QString name, QDate dBeg, QDate dEnd)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("insert into econom_unload (nam, d_beg, d_end) values (:nam, :d_beg, :d_end) returning id");
    query.bindValue(":nam",name);
    query.bindValue(":d_beg",dBeg);
    query.bindValue(":d_end",dEnd);
    if (query.exec()){
        while (query.next()){
            id=query.value(0).toInt();
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::getIdSubdiv(QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("select id from econom_subdiv where nam = :nam ");
    query.bindValue(":nam",nam);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                id=query.value(0).toInt();
            }
        } else {
            id=addNam("econom_subdiv",nam);
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::getIdNomGroup(QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("select id from econom_nom_group where nam = :nam ");
    query.bindValue(":nam",nam);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                id=query.value(0).toInt();
            }
        } else {
            id=addNam("econom_nom_group",nam);
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}


int Import1C::getIdProd(QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("select id from econom_production where nam = :nam ");
    query.bindValue(":nam",nam);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                id=query.value(0).toInt();
            }
        } else {
            id=addNam("econom_production", nam);
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::getIdCost(QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("select id from econom_cost where nam = :nam ");
    query.bindValue(":nam",nam);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                id=query.value(0).toInt();
            }
        } else {
            id=addNam("econom_cost", nam);
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::getIdCostItem(QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("select id from econom_cost_item where nam = :nam ");
    query.bindValue(":nam",nam);
    if (query.exec()){
        if (query.size()){
            while (query.next()){
                id=query.value(0).toInt();
            }
        } else {
            id=addNam("econom_cost_item", nam);
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}


int Import1C::addNam(QString tablename, QString nam)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("insert into "+tablename+" (nam) values (:nam) returning id");
    query.bindValue(":nam",nam);
    if (query.exec()){
        while (query.next()){
            id=query.value(0).toInt();
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

double Import1C::toDouble(QString n)
{
    n=n.replace(",",".");
    return n.toDouble();
}

int Import1C::addUnloadItem(int id_unload, int id_subdiv, int id_nom_group, int id_prod, double kvo, double cost_price)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("insert into econom_unload_items (id_unload, id_subdiv, id_nom_group, id_prod, kvo, cost_price) "
                  "values (:id_unload, :id_subdiv, :id_nom_group, :id_prod, :kvo, :cost_price) returning id");
    query.bindValue(":id_unload",id_unload);
    query.bindValue(":id_subdiv",id_subdiv);
    query.bindValue(":id_nom_group",id_nom_group);
    query.bindValue(":id_prod",id_prod);
    query.bindValue(":kvo",kvo);
    query.bindValue(":cost_price",cost_price);
    if (query.exec()){
        while (query.next()){
            id=query.value(0).toInt();
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::addUnloadItemCost(int id_unload_it, int id_cost, int id_type, double val)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("insert into econom_unload_items_cost (id_unload, id_cost, id_type, val) "
                  "values (:id_unload, :id_cost, :id_type, :val) returning id");
    query.bindValue(":id_unload",id_unload_it);
    query.bindValue(":id_cost",id_cost);
    query.bindValue(":id_type",id_type);
    query.bindValue(":val",val);
    if (query.exec()){
        while (query.next()){
            id=query.value(0).toInt();
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}

int Import1C::addUnloadItemCostData(int id_unload_item_cost, int id_cost_item, double kvo, double price, double val)
{
    int id=-1;
    QSqlQuery query;
    query.prepare("insert into econom_unload_items_cost_data (id_item, id_cost_item, kvo, price, val) "
                  "values (:id_item, :id_cost_item, :kvo, :price, :val) returning id");
    query.bindValue(":id_item",id_unload_item_cost);
    query.bindValue(":id_cost_item",id_cost_item);
    query.bindValue(":kvo",kvo);
    query.bindValue(":price",price);
    query.bindValue(":val",val);
    if (query.exec()){
        while (query.next()){
            id=query.value(0).toInt();
        }
    } else {
        s_err=query.lastError().text();
    }
    return id;
}
