//-----------------------------------------------------------------------------
// File: FileDependencyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 12.02.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyModel.h"

#include "FileDependencyItem.h"

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::FileDependencyModel()
    : root_(new FileDependencyItem())
{
    FileDependencyItem* folder = root_->addChild(FileDependencyItem::ITEM_TYPE_FOLDER, "some/code");
    folder->addChild(FileDependencyItem::ITEM_TYPE_FILE, "main.c");
    folder->addChild(FileDependencyItem::ITEM_TYPE_FILE, "utils.c");
    folder->addChild(FileDependencyItem::ITEM_TYPE_FILE, "utils.h");
    root_->addChild(FileDependencyItem::ITEM_TYPE_FOLDER, "some/documentation");
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::~FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::~FileDependencyModel()
{
    delete root_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::headerData(int section, Qt::Orientation orientation,
                                         int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case FILE_DEPENDENCY_COLUMN_STATUS:
            {
                return tr("Status");
            }

        case FILE_DEPENDENCY_COLUMN_PATH:
            {
                return tr("Path");
            }

        case  FILE_DEPENDENCY_COLUMN_FILESETS:
            {
                return tr("Filesets");
            }

        case FILE_DEPENDENCY_COLUMN_CREATE:
            {
                return tr("#");
            }

        case FILE_DEPENDENCY_COLUMN_DEPENDENCIES:
            {
                return tr("Dependencies");
            }

        case FILE_DEPENDENCY_COLUMN_REFERENCES:
            {
                return tr("References");
            }
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::columnCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::columnCount(const QModelIndex & parent /*= QModelIndex() */) const
{
    return FILE_DEPENDENCY_COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::rowCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return static_cast<FileDependencyItem*>(parent.internalPointer())->getChildCount();
    }
    else
    {
        return root_->getChildCount();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::index()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    FileDependencyItem* child = parentItem->getChild(row);
    
    if (child == 0)
    {
        return QModelIndex();
    }

    return createIndex(row, column, child);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::parent()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    FileDependencyItem* childItem = static_cast<FileDependencyItem*>(child.internalPointer());
    FileDependencyItem* parent = childItem->getParent();

    if (parent == 0)
    {
        return QModelIndex();
    }

    int index = parent->getIndex();

    if (index < 0)
    {
        return QModelIndex();
    }

    return createIndex(index, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::data()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case FILE_DEPENDENCY_COLUMN_PATH:
            {
                return item->getSimplePath();
            }

        case FILE_DEPENDENCY_COLUMN_FILESETS:
            {
                return tr("none");
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
        {
            switch (item->getStatus())
            {
            case FILE_DEPENDENCY_STATUS_UNKNOWN:
                {
                    return QIcon(":icons/graphics/traffic-light_gray.png");
                }

            case FILE_DEPENDENCY_STATUS_OK:
                {
                    return QIcon(":icons/graphics/traffic-light_green.png");
                }

            case FILE_DEPENDENCY_STATUS_CHANGED:
                {
                    return QIcon(":icons/graphics/traffic-light_yellow.png");
                }

            case FILE_DEPENDENCY_STATUS_CHANGED2:
                {
                    return QIcon(":icons/graphics/traffic-light_red.png");
                }
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
        {
            return QSize(16, 16);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            return QColor(230, 230, 230);
        }
        else
        {
            return QColor(Qt::white);
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::hasChildren()
//-----------------------------------------------------------------------------
bool FileDependencyModel::hasChildren(const QModelIndex& parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    return (parentItem->getChildCount() > 0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileDependencyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    switch (index.column())
    {
    case FILE_DEPENDENCY_COLUMN_STATUS:
    case FILE_DEPENDENCY_COLUMN_CREATE:
    case FILE_DEPENDENCY_COLUMN_DEPENDENCIES:
        return Qt::NoItemFlags;

    default:
        return Qt::ItemIsEnabled;
    }
}