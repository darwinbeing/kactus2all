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
    setItemDelegate(new FileDependencyDelegate(this));
    setStyleSheet(
        "QTreeView::item"
        "{"
        "   border: 1px solid black;"
        "}"
    );
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::~FileDependencyGraphWidget()
{
}
