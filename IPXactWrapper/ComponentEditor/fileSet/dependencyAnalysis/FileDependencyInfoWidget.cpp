//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.01.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#include "FileDependencyInfoWidget.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::FileDependencyInfoWidget(QWidget* parent)
    : QGroupBox(tr("Dependency Information"), parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&descEdit_);

    descEdit_.setMaximumHeight(100);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::~FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::~FileDependencyInfoWidget()
{

}
