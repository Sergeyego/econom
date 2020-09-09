#include <QtGui>
#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(int id, QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << tr("Затраты") << tr("Количество")<< tr("Цена")<< tr("Сумма");
    rootItem = new TreeItem(rootData);
    setupModelData(id, rootItem);
}

TreeModel::~TreeModel()
{
    delete rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role==Qt::DecorationRole && index.column()==0){
        return QIcon::fromTheme(QString::fromUtf8("folder"));
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

void TreeModel::setupModelData(int id, TreeItem *parent)
{
    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    QMap <QString, QString> map;

    /*MdbSQL *g_sql;
    g_sql=mdb_sql_init();
    mdb_sql_open(g_sql,fname.toLocal8Bit().data());
    mdb_sql_run_query(g_sql,"select InternalCode, catCode, catName from Category");
    QString err=QString::fromLocal8Bit(g_sql->error_msg);
    if (!err.isEmpty()){
        QMessageBox::critical(NULL,tr("Ошибка"),err,QMessageBox::Ok);
    } else {
        QString cod, cat,nam;
        while (mdb_fetch_row(g_sql->cur_table)){
            cod=QString::fromLocal8Bit((char*)g_sql->bound_values[0]);
            cat=QString::fromLocal8Bit((char*)g_sql->bound_values[1]);
            nam=QString::fromLocal8Bit((char*)g_sql->bound_values[2]);
            map.insert(cod,cat+" "+nam+"\t"+cod);
        }
    }
    //qDebug()<<map;

    for (int j=0;j<g_sql->num_columns;j++) {
        g_free(g_sql->bound_values[j]);
    }
    mdb_sql_reset(g_sql);
    mdb_sql_exit(g_sql);*/

    QMap<QString, QString>::const_iterator i = map.constBegin();
    while (i != map.constEnd()) {
        int position = 0;
        if (i.key().contains(QChar('.'))){
            position=i.key().split(QChar('.')).size();
        }

        QString lineData = i.value();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }
        ++i;
    }
}
