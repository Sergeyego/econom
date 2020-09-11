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

void TreeItem::setData(int column, QVariant data)
{
    itemData[column]=data;
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
    for (int i=0; i<query.record().count(); i++){
        rootData.push_back(query.record().fieldName(i));
    }

    if (rootItem){
        delete rootItem;
    }

    rootItem = new TreeItem(rootData);

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << rootItem;
    indentations << 0;
    QChar sep=QChar('&');
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
    endResetModel();
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    bool ok=false;
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){
        ok=true;
        rootItem->setData(section, value);
        emit headerDataChanged(orientation,section,section);
    }
    return ok;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
    QVariant orig=item->data(index.column());
    QVariant::Type type=orig.type();
    if (role==Qt::TextAlignmentRole){
        return (type==QMetaType::Int || type==QMetaType::Double || type==QMetaType::Float || type==QMetaType::LongLong ) ?
                int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    } else   if (role==Qt::DisplayRole){
        if (type==QMetaType::Double){
            return (orig.isNull()) ? QString("") : QLocale().toString(orig.toDouble(),'f',2);
        } else if (type==QMetaType::QDate){
            return (orig.isNull()) ? QString("") : orig.toDate().toString("dd.MM.yy");
        } else {
            return orig;
        }
    } else if (role==Qt::EditRole){
        return orig;
    } else if (role==Qt::BackgroundRole){
        TreeItem *it=item;
        int n=0;
        QColor color(Qt::white);
        while(it!=rootItem){
            it=it->parent();
            n++;
        }
        if (n==1){
            color=QColor(180,190,170);
        } else if (n==2){
            color=QColor(200,210,190);
        } else if (n==3){
            color=QColor(214,229,203);
        } else if (n==4){
            color=QColor(227,239,220);
        } else if (n==5){
            color=QColor(240,246,239);
        }
        return color;
    }
    return QVariant();
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

