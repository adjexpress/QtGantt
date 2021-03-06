#include "gantttreemodel.h"

#include "extensions/gantt-lib_global_values.h"

#include <QSize>

Q_DECLARE_METATYPE(UtcDateTime)

GanttTreeModel::GanttTreeModel(GanttInfoNode *root,QObject * parent)
    : QAbstractItemModel(parent)
{

    m_root = root;

    if(!m_root)
        m_root = new GanttInfoNode;

    m_root->setExpanded(true);
    setIndex(m_root);
}

GanttTreeModel::~GanttTreeModel()
{
    clear();
}

QVariant GanttTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == Qt::SizeHintRole)
    {
        QSize defSize ;
        defSize.setHeight(DEFAULT_ITEM_WIDTH);
        return defSize ;
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    GanttInfoLeaf *leaf = leafForIndex(index);
    if(leaf)
    {
        switch (index.column())
        {
        case titleField:
            return leaf->title();
        case startField:
            return leaf->start().dateTime();
        case finishField:
            return leaf->finish().dateTime();
        case durationField:
            return leaf->timeSpan().toString();
        default:
            Q_ASSERT(false);
        }
    }

    GanttInfoNode *node = nodeForIndex(index);
    if(node == m_root)
    {
        return QVariant();
    }

    if(node)
    {
        switch (index.column())
        {
        case 0:
            return node->title();
        case 1:
            if(node->hasStart())
                return node->start().dateTime();
            return QVariant();
        case finishField:
        case durationField:
            return QVariant();
        default: Q_ASSERT(false);
        }
    }

    return QVariant();
}

Qt::ItemFlags GanttTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled /*| Qt::ItemIsSelectable*/;
}

QVariant GanttTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);

    if (role == Qt::SizeHintRole)
    {
        QSize defSize ;
        defSize.setHeight(2*DEFAULT_ITEM_WIDTH);
        return defSize ;
    }


    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case titleField:
            return trUtf8("Название");
        case startField:
            return trUtf8("Время расчета/начала");
        case finishField:
            return trUtf8("Время окончания");
        case durationField:
            return trUtf8("Длительность");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QModelIndex GanttTreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    GanttInfoNode *parentNode;

    if (!parent.isValid())
    {
        parentNode = m_root;
    }
    else
        parentNode = nodeForIndex(parent);

    GanttInfoItem *childItem = parentNode->at(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();

}

GanttInfoItem *GanttTreeModel::itemForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return m_root;
    }

    return static_cast<GanttInfoItem*>(index.internalPointer());
}

GanttInfoLeaf *GanttTreeModel::leafForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return NULL;
    }

    return dynamic_cast<GanttInfoLeaf*>(itemForIndex(index));
}

GanttInfoNode *GanttTreeModel::nodeForIndex(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return m_root;
    }

    return dynamic_cast<GanttInfoNode*>(itemForIndex(index));
}

void GanttTreeModel::setIndex(GanttInfoItem *item)
{
    if(!item->index().isValid())
        item->setIndex((item==m_root)?(QModelIndex()):(createIndex(item->row(),0,item)));
}

GanttInfoItem *GanttTreeModel::itemForNameHelper(const QString &title,GanttInfoNode* node) const
{
    if(!node)
        return NULL;

    for(int i = 0; i < node->size(); ++i)
    {
        GanttInfoItem *p_item = node->at(i) , *tmp;
        if(p_item->title() == title)
            return p_item;

        GanttInfoNode *p_node = dynamic_cast<GanttInfoNode*>(p_item);
        if(p_node && (tmp = itemForNameHelper(title,p_node)))
            return tmp;
    }
    return NULL;
}
GanttInfoNode *GanttTreeModel::root() const
{
    return m_root;
}


void deleteFunc(GanttInfoItem* item)
{
    GanttInfoNode *parent = item->parent();

    if(parent)
    {
        item->deleteInfoItem();
    }
}

void GanttTreeModel::clear()
{
    beginRemoveRows(QModelIndex(),0,m_root->size());

    m_root->callForEachItemRecursively(&deleteFunc);
    m_root->clear();

    endRemoveRows();

}




QModelIndex GanttTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    GanttInfoItem *childItem = itemForIndex(index);
    GanttInfoNode *parentNode = childItem->parent();

    if(!parentNode)
    {
        qWarning("GanttTreeModel::parent     !parentNode");
        return QModelIndex();
    }

    if (parentNode == m_root)
    {
        return QModelIndex();
    }

    return createIndex(parentNode->row(), 0, parentNode);
}

int GanttTreeModel::rowCount(const QModelIndex &parent) const
{
    GanttInfoNode *parentNode;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = m_root;
    else if(!(parentNode = nodeForIndex(parent)))
        return 0;

    return parentNode->size();
}

int GanttTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
//    GanttInfoNode *parentNode;


//    if (parent.column() > 0)
//        return 0;

//    if (!parent.isValid())
//        parentNode = m_root;
//    else
//        parentNode = nodeForIndex(parent);

//    return parentNode->columnCount();

    return 4;
}

bool GanttTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() == durationField )
        return false;


    GanttInfoItem *info = itemForIndex(index);
    GanttInfoLeaf *leaf = qobject_cast<GanttInfoLeaf*>(info);

    if (info)
    {
        if (role == Qt::EditRole)
        {
            if (index.column() == titleField)
                info->setTitle(value.toString());
            else if (index.column() == startField && leaf)
            {
                UtcDateTime time = UtcDateTime(value.toDateTime());

                if(time >= leaf->finish())
                    return false;
                leaf->setStart(time);
            }
            else if (index.column() == finishField && leaf)
            {
                UtcDateTime time = UtcDateTime(value.toDateTime());
                if(leaf->start() >= time)
                    return false;

                leaf->setTimeSpan(time - leaf->start());
            }
        }
//        else if (role == Qt::CheckStateRole)
//            info->setDone(value.toBool());
        else
            return false;
        return true;
    }
    return false;
}

void GanttTreeModel::addItems(const QList<GanttInfoItem *> &items)
{
    int cnt = 0;
    foreach(GanttInfoItem *item, items){
        qDebug() << "addItems: "<<item->title();
        if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item))
            for(int i = 0; i < node->size();++i)
                qDebug() << "\t.."<<node->at(i)->title();
    }

    QMap<QString, GanttInfoNode*> tmpNodesForNames;
    foreach(GanttInfoItem *item, items){
        if(GanttInfoNode *nodeForName = qobject_cast<GanttInfoNode*>(itemForName(item->title())))
            tmpNodesForNames.insert(item->title(), nodeForName);
        else
            ++cnt;
    }
    qDebug() << "test cnt " << cnt;
    int first = (m_root->isEmpty()? 0 : m_root->size());
    if(cnt > 0){
        beginInsertRows(QModelIndex(), first, first + cnt - 1);
        for(int i = 0; i < items.size(); ++i){
            if(tmpNodesForNames.contains(items.at(i)->title()))
                continue;

            insertItem(items.at(i));
        }
        endInsertRows();

        for(int i = 0; i < items.size(); ++i){
            if(tmpNodesForNames.contains(items.at(i)->title()))
                continue;
            if(GanttInfoNode *node = qobject_cast<GanttInfoNode*>(items.at(i)))
                addNode(node);
        }

    }
    if(cnt!=items.size())
        for(int i = 0; i < items.size(); ++i){
            if(tmpNodesForNames.contains(items.at(i)->title())){
                GanttInfoNode *insertToNode = tmpNodesForNames.value(items.at(i)->title());
                int first = (insertToNode->isEmpty()?0:insertToNode->size());
                GanttInfoNode *extNode = qobject_cast<GanttInfoNode*>(items.at(i));
                if(!extNode || extNode->isEmpty())
                    continue;
                beginInsertRows(insertToNode->index(), first, first + extNode->size() - 1);
                for(int i = 0; i < extNode->size(); ++i)
                    insertItem(extNode->at(i),insertToNode);
                endInsertRows();
            }
        }





    emit itemsAdded();
}

void GanttTreeModel::addNode(GanttInfoNode *node)
{
    if(node->isEmpty())
        return;
    qDebug() << "addnode "<<node->title() << " "<< node->index();
    beginInsertRows(node->index(), 0, node->size() - 1);
    for(int i = 0; i < node->size(); ++i){
        setIndex(node->at(i));
    }
    endInsertRows();
}

//void GanttTreeModel::addItems(GanttInfoItem *item, bool inner)
//{
//    GanttInfoNode *insertToNode = qobject_cast<GanttInfoNode*>(itemForName(item->title()));
//    if(!insertToNode)
//        insertToNode = m_root;
//    if(insertToNode)
//    {
//        GanttInfoNode *node = qobject_cast<GanttInfoNode*>(item);
//        if(node)
//        {
//            if(!node->isEmpty()){
//                beginInsertRows(insertToNode->index(),insertToNode->size(),insertToNode->size() + node->size() - 1);
//                for(int i=0;i<node->size();++i){
//                    insertToNode->append(node->at(i));
//                    setIndex(node->at(i));
//                }
//                endInsertRows();
//            }
//        }
//        setIndex(insertToNode);
//    }
//    else
//    {
//        beginInsertRows(QModelIndex(),m_root->size(),m_root->size()+1);
//        m_root->append(item);
//        endInsertRows();
//        setIndex(item);
//    }

//    if(!inner){
//        // NON RECURSIVELY
//        for(int i = 0; i<m_root->size() ; ++i)
//        {
//            GanttInfoNode *node = m_root->nodeAt(i);
//            if(node && node->isExpanded())
//            {
//                emit needCollapse(node);
//            }

//        }
//        emit itemsAdded(item);
//    }

//}

void GanttTreeModel::insertItem(GanttInfoItem *item, GanttInfoNode *parent)
{
    if(!parent)
        parent = m_root;

    parent->append(item);
    setIndex(item);
}

GanttInfoItem *GanttTreeModel::itemForName(const QString &title) const
{
    return itemForNameHelper(title,m_root);
}


