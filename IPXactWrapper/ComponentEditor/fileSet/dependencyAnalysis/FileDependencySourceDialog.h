//-----------------------------------------------------------------------------
// File: FileDependencySourceDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Tommi Korhonen, Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYSOURCEDIALOG_H
#define FILEDEPENDENCYSOURCEDIALOG_H

#include <QDialog>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------
class FileDependencySourceDialog : public QDialog
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] sourceDirs  The current list of source directories.
     *      @param [in] parent      The parent widget.
     */
    FileDependencySourceDialog(QStringList const& sourceDirs, QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~FileDependencySourceDialog();

    /*!
     *  Retrieves the new list of source directories.
     *
     *      @return The list of source directories.
     *
     *      @remarks Valid only when the user presses OK.
     */
    QStringList const& getSourceDirectories() const;

private slots:
    /*!
     *  Adds a new source to the list. The source directory is prompted with a folder selection dialog.
     */
    void addSource();

    /*!
     *  Removes the currently selected source from the list.
     */
    void removeSource();

private:
    // Disable copying.
    FileDependencySourceDialog(FileDependencySourceDialog const& rhs);
    FileDependencySourceDialog& operator=(FileDependencySourceDialog const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYSOURCEDIALOG_H
