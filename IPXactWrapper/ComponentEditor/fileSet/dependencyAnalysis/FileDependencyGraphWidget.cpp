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

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::FileDependencyGraphWidget(QWidget* parent)
    : QTreeView(parent)
{
    setModel(new FileDependencyModel());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::~FileDependencyGraphWidget()
{
}
