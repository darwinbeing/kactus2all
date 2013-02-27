//-----------------------------------------------------------------------------
// File: FileDependencyItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
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
     *  Sets the status of the item.
     *
     *      @param [in] status The new status to set.
     */
    void setStatus(FileDependencyStatus status);

    /*!
     *  Updates the status based on the child items.
     *
     *      @remarks Only relevant to folder items.
     */
    void updateStatus();

    /*!
     *  Sets the latest hash to the metadata.
     *
     *      @param [in] hash The hash to set.
     */
    void setLastHash(QString const& hash);

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
     *  Returns the all file types the file represents.
     */
    QStringList getFileTypes() const;

    /*!
     *  Returns a shortened version of the path.
     */
    QString getSimplePath() const;

    /*!
     *  Returns the name of the file (i.e. the packaged relative name).
     */
    QString const& getPath() const;

    /*!
     *  Returns the latest calculated hash or an empty string if the file is a new one.
     */
    QString getLastHash() const;

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
