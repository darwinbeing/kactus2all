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
#include <QSharedPointer>

class File;
class FileSet;
class Component;

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

    void updateStatus();

    /*!
     *  Adds a file item.
     *
     *      @param [in] type       The item type.
     *      @param [in] component  The component being edited.
     *      @param [in] path       The file path.
     *      @param [in] fileRefs   The file references.
     */
    FileDependencyItem* addFile(Component* component, QString const& path,
                                QList<File*> const& fileRefs);

    /*!
     *  Adds a folder item.
     *
     *      @param [in] type       The item type.
     *      @param [in] component  The component being edited.
     *      @param [in] path       The folder path.
     */
    FileDependencyItem* addFolder(Component* component, QString const& path);

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
     *  Returns the status of the item.
     */
    FileDependencyStatus getStatus() const;

    /*!
     *  Returns the item type.
     */
    ItemType getType() const;

    /*!
     *  Returns the file sets where the file is currently contained.
     */
    QList<FileSet*> getFileSets() const;

    /*!
     *  Returns a shortened version of the path.
     */
    QString getSimplePath();

private:
    // Disable copying.
    FileDependencyItem(FileDependencyItem const& rhs);
    FileDependencyItem& operator=(FileDependencyItem const& rhs);

    /*!
     *  Constructor which creates a file item.
     *
     *      @param [in] parent     The parent item.
     *      @param [in] component  The component being edited.
     *      @param [in] path       The path of the file/folder.
     *      @param [in] fileRefs   The file references.
     */
    FileDependencyItem(FileDependencyItem* parent, Component* component, QString const& path,
                       QList<File*> const& fileRefs);

    /*!
     *  Constructor which creates a folder item.
     *
     *      @param [in] parent     The parent item.
     *      @param [in] component  The component being edited.
     *      @param [in] path       The path of the file/folder.
     */
    FileDependencyItem(FileDependencyItem* parent, Component* component, QString const& path);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent item.
    FileDependencyItem* parent_;
    
    //! The current status.
    FileDependencyStatus status_;

    //! The item type.
    ItemType type_;

    //! The component being edited.
    Component* component_;

    //! The file path of the file/folder.
    QString path_;

    //! VLNV references.
    QList<VLNV> references_;

    //! File pointers.
    QList<File*> fileRefs_;

    //! The child items.
    QList<FileDependencyItem*> children_;

};

#endif // FILEDEPENDENCYITEM_H
