//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYGRAPHWIDGET_H
#define FILEDEPENDENCYGRAPHWIDGET_H

#include <QTreeView>

//-----------------------------------------------------------------------------
//! Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------
class FileDependencyGraphWidget : public QTreeView
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyGraphWidget(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyGraphWidget();

private:
    // Disable copying.
    FileDependencyGraphWidget(FileDependencyGraphWidget const& rhs);
    FileDependencyGraphWidget& operator=(FileDependencyGraphWidget const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYGRAPHWIDGET_H
