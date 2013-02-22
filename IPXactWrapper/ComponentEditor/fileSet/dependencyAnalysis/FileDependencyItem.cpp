//-----------------------------------------------------------------------------
// File: FileDependencyItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.02.2013
//
// Description:
// Base class for dependency items.
//-----------------------------------------------------------------------------

#include "FileDependencyItem.h"

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem()
    : parent_(0),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(ITEM_TYPE_ROOT),
      path_(),
      references_(),
      children_()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem(FileDependencyItem* parent, ItemType type, QString const& path)
    : parent_(parent),
      status_(FILE_DEPENDENCY_STATUS_OK),
      type_(type),
      path_(path),
      references_(),
      children_()
{
    Q_ASSERT(type != ITEM_TYPE_ROOT);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::~FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::~FileDependencyItem()
{
    foreach (FileDependencyItem* item, children_)
    {
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::addChild()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::addChild(ItemType type, QString const& path)
{
    FileDependencyItem* item = new FileDependencyItem(this, type, path);
    children_.append(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getChild()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::getChild(int index)
{
    if (index < 0 || index >= getChildCount())
    {
        return 0;
    }

    return children_[index];
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getChildCount()
//-----------------------------------------------------------------------------
int FileDependencyItem::getChildCount() const
{
    return children_.size();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getParent()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::getParent()
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getIndex()
//-----------------------------------------------------------------------------
int FileDependencyItem::getIndex()
{
    if (parent_ == 0)
    {
        return -1;
    }

    return parent_->children_.indexOf(this);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getStatus()
//-----------------------------------------------------------------------------
FileDependencyStatus FileDependencyItem::getStatus() const
{
    return status_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getType()
//-----------------------------------------------------------------------------
FileDependencyItem::ItemType FileDependencyItem::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getSimplePath()
//-----------------------------------------------------------------------------
QString FileDependencyItem::getSimplePath()
{
    if (type_ == ITEM_TYPE_FILE)
    {
        QFileInfo info(path_);
        return info.fileName();
    }
    else
    {
        return path_;
    }
}
