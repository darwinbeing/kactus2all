//-----------------------------------------------------------------------------
// File: FileDependencyItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.02.2013
//
// Description:
// Base class for file dependency items.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYITEM_H
#define FILEDEPENDENCYITEM_H

#include <LibraryManager/vlnv.h>

#include <QObject>
#include <QString>
#include <QList>

//-----------------------------------------------------------------------------
//! Dependency status enumeration.
//-----------------------------------------------------------------------------
enum FileDependencyStatus
{
    FILE_DEPENDENCY_STATUS_UNKNOWN = 0,
    FILE_DEPENDENCY_STATUS_OK,
    FILE_DEPENDENCY_STATUS_CHANGED,
    FILE_DEPENDENCY_STATUS_CHANGED2
};

//-----------------------------------------------------------------------------
//! Base class for file dependency items.
//-----------------------------------------------------------------------------
class FileDependencyItem : public QObject
{
    Q_OBJECT

public:
    enum ItemType
    {
        ITEM_TYPE_ROOT = 0,
        ITEM_TYPE_FOLDER,
        ITEM_TYPE_FILE
    };

    /*!
     *  Default constructor which creates a root item.
     */
    FileDependencyItem();

    /*!
     *  Destructor.
     */
    virtual ~FileDependencyItem();

    /*!
     *  Adds a child item.
     *
     *      @param [in] type  The item type.
     *      @param [in] path  The path of the file/folder.
     */
    FileDependencyItem* addChild(ItemType type, QString const& path);

    /*!
     *  Returns the child at the given index.
     *
     *      @param [in] index The index of the child to retrieve.
     */
    FileDependencyItem* getChild(int index);

    /*!
     *  Returns the number of child items.
     */
    int getChildCount() const;

    /*!
     *  Returns the parent item.
     */
    FileDependencyItem* getParent();

    /*!
     *  Returns the index of this item.
     */
    int getIndex();

    /*!
     *  Returns a shortened version of the path.
     */
    QString getSimplePath();

private:
    // Disable copying.
    FileDependencyItem(FileDependencyItem const& rhs);
    FileDependencyItem& operator=(FileDependencyItem const& rhs);

    /*!
     *  Constructor which creates a file/folder item.
     *
     *      @param [in] parent  The parent item.
     *      @param [in] type    The item type.
     *      @param [in] path    The path of the file/folder.
     */
    FileDependencyItem(FileDependencyItem* parent, ItemType type, QString const& path);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent item.
    FileDependencyItem* parent_;
    
    //! The current status.
    FileDependencyStatus status_;

    //! The item type.
    ItemType type_;

    //! The file path of the file/folder.
    QString path_;

    //! VLNV references.
    QList<VLNV> references_;

    //! The child items.
    QList<FileDependencyItem*> children_;

};

#endif // FILEDEPENDENCYITEM_H
