//-----------------------------------------------------------------------------
// File: FileDependencyEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYEDITOR_H
#define FILEDEPENDENCYEDITOR_H

#include "FileDependencyGraphWidget.h"
#include "FileDependencyInfoWidget.h"

#include <QGroupBox>
#include <QToolBar>
#include <QTableWidget>
#include <QProgressBar>

//-----------------------------------------------------------------------------
//! File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------
class FileDependencyEditor : public QWidget
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyEditor(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyEditor();

private:
    // Disable copying.
    FileDependencyEditor(FileDependencyEditor const& rhs);
    FileDependencyEditor& operator=(FileDependencyEditor const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Toolbar.
    QToolBar toolbar_;

    //! The progress bar.
    QProgressBar progressBar_;

    //! The dependency graph widget.
    FileDependencyGraphWidget graphWidget_;

    //! The dependency info widget,.
    FileDependencyInfoWidget infoWidget_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYEDITOR_H
