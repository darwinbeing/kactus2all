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

#include <models/component.h>
#include <models/file.h>
#include <models/fileset.h>

#include <QFileInfo>

#include <windows.h>

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem()
    : parent_(0),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(ITEM_TYPE_ROOT),
      path_(),
      references_(),
      fileRefs_(),
      children_()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem(FileDependencyItem* parent,
                                       Component* component, QString const& path,
                                       ItemType type)
    : parent_(parent),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(type),
      component_(component),
      path_(path),
      references_(),
      fileRefs_(),
      children_()
{
    // Allow only folder/location items to be added.
    Q_ASSERT(type != ITEM_TYPE_FILE);
    Q_ASSERT(type != ITEM_TYPE_ROOT);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::FileDependencyItem()
//-----------------------------------------------------------------------------
FileDependencyItem::FileDependencyItem(FileDependencyItem* parent,
                                       Component* component, QString const& path,
                                       QList<File*> const& fileRefs)
    : parent_(parent),
      status_(FILE_DEPENDENCY_STATUS_UNKNOWN),
      type_(ITEM_TYPE_FILE),
      component_(component),
      path_(path),
      references_(),
      fileRefs_(fileRefs),
      children_()
{
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
QString FileDependencyItem::getDisplayPath() const
{
    switch (type_)
    {
    case ITEM_TYPE_FILE:
        {
            QFileInfo info(path_);
            return info.fileName();
        }

    case ITEM_TYPE_EXTERNAL_LOCATION:
        {
            return tr("External: ") + path_;
        }

    case ITEM_TYPE_UNKNOWN_LOCATION:
        {
            return tr("External");
        }

    default:
        {
        return path_;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getFileSets()
//-----------------------------------------------------------------------------
QList<FileSet*> FileDependencyItem::getFileSets() const
{
    QList<FileSet*> fileSets;

    foreach (File* file, fileRefs_)
    {
        if (!fileSets.contains(file->getParent()))
        {
            fileSets.append(file->getParent());
        }
    }

    return fileSets;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::addFile()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::addFile(Component* component, QString const& path,
                                                QList<File*> const& fileRefs)
{
    FileDependencyItem* item = new FileDependencyItem(this, component, path, fileRefs);
    children_.append(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::addFolder()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyItem::addFolder(Component* component, QString const& path, ItemType type)
{
    FileDependencyItem* item = new FileDependencyItem(this, component, path, type);
    children_.append(item);
    return item;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::updateStatus()
//-----------------------------------------------------------------------------
void FileDependencyItem::updateStatus()
{
    status_ = FILE_DEPENDENCY_STATUS_OK;

    foreach (FileDependencyItem* item, children_)
    {
        switch (item->getStatus())
        {
        case FILE_DEPENDENCY_STATUS_CHANGED2:
            {
                status_ = FILE_DEPENDENCY_STATUS_CHANGED2;
                break;
            }

        case FILE_DEPENDENCY_STATUS_CHANGED:
            {
                if (status_ == FILE_DEPENDENCY_STATUS_OK)
                {
                    status_ = FILE_DEPENDENCY_STATUS_CHANGED;
                }
                break;
            }

        default:
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileDependencyItem::getFileTypes() const
{
    QStringList list;

    foreach (File* file, fileRefs_)
    {
        foreach (QString const& type, file->getFileTypes())
        {
            if (!list.contains(type))
            {
                list.append(type);
            }
        }
    }

    return list;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getName()
//-----------------------------------------------------------------------------
QString const& FileDependencyItem::getPath() const
{
    return path_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::setStatus()
//-----------------------------------------------------------------------------
void FileDependencyItem::setStatus(FileDependencyStatus status)
{
    status_ = status;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::getLastHash()
//-----------------------------------------------------------------------------
QString FileDependencyItem::getLastHash() const
{
    if (fileRefs_.empty())
    {
        return QString();
    }

    return fileRefs_[0]->getLastHash();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::setLastHash()
//-----------------------------------------------------------------------------
void FileDependencyItem::setLastHash(QString const& hash)
{
    foreach (File* file, fileRefs_)
    {
        file->setLastHash(hash);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyItem::isExternal()
//-----------------------------------------------------------------------------
bool FileDependencyItem::isExternal() const
{
    if (type_ == ITEM_TYPE_FILE)
    {
        return (parent_->type_ != ITEM_TYPE_FOLDER);
    }
    else
    {
        return (type_ != ITEM_TYPE_FOLDER);
    }
}
