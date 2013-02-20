//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.01.2013
//
// Description:
// Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------

#include "FileDependencyGraphWidget.h"

#include "FileDependencyModel.h"
#include "FileDependencyDelegate.h"

#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::FileDependencyGraphWidget(QWidget* parent)
    : QTreeView(parent)
{
    setModel(new FileDependencyModel());
    setItemDelegate(new FileDependencyDelegate(this));
    resizeColumnToContents(FILE_DEPENDENCY_COLUMN_TREE);
    resizeColumnToContents(FILE_DEPENDENCY_COLUMN_STATUS);
    resizeColumnToContents(FILE_DEPENDENCY_COLUMN_CREATE);

    setColumnWidth(FILE_DEPENDENCY_COLUMN_PATH, 250);

    header()->setResizeMode(FILE_DEPENDENCY_COLUMN_TREE, QHeaderView::Fixed);
    header()->setResizeMode(FILE_DEPENDENCY_COLUMN_STATUS, QHeaderView::Fixed);
    header()->setResizeMode(FILE_DEPENDENCY_COLUMN_CREATE, QHeaderView::Fixed);

    expandAll();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::~FileDependencyGraphWidget()
{
}
