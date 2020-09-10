#include "treemodel.h"

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
{
    parentItem = parent;
    itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
    return childItems.value(row);
}

int TreeItem::childCount() const
{
    return childItems.count();
}

int TreeItem::columnCount() const
{
    return itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return itemData.value(column);
}

TreeItem *TreeItem::parent()
{
    return parentItem;
}

int TreeItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << QString("/");
    rootItem = new TreeItem(rootData);
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

void TreeModel::refresh(QSqlQuery &query)
{
    beginResetModel();
    QList<QVariant> rootData;
    rootData << QString("Затраты")<<QString("К-во вып.")<<QString("Себестоим.ед.")<<QString("К-во")<<QString("Цена")<<QString("Сумма");
    if (rootItem){
        delete rootItem;
    }

    rootItem = new TreeItem(rootData);

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << rootItem;
    indentations << 0;
    QChar sep=QChar('&');

    if (query.exec()){
        while (query.next()) {
            int position = 0;
            QString lineData;
            QStringList l = query.value(0).toString().split(sep);
            if (l.last().isEmpty()){
                position= (l.size()>1)? l.size()-2 : 0;
                lineData=(l.size()>1)? l.at(l.size()-2) : QString();
            } else {
                position=l.size()-1;
                lineData=l.at(l.size()-1);
            }
            if (!lineData.isEmpty()) {
                QList<QVariant> columnData;
                columnData.push_back(lineData);
                for (int i=1;i<query.record().count();i++){
                    columnData.push_back(query.value(i));
                }
                if (position > indentations.last()) {
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
                parents.last()->appendChild(new TreeItem(columnData, parents.last()));
            }
        }
    }
    endResetModel();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    if (role==Qt::TextAlignmentRole && index.column()>0){
        return int(Qt::AlignRight | Qt::AlignVCenter);
    }

    if (role != Qt::DisplayRole)
        return QVariant();

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

