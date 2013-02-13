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

#include <models/FileDependency.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::FileDependencyInfoWidget(QWidget* parent)
    : QGroupBox(tr("Dependency Information"), parent),
      descEdit_(this),
      directionCombo_(this),
      lockedCheck_(tr("Locked"), this),
      dependency_()
{
    directionCombo_.setFixedWidth(150);
    descEdit_.setMaximumHeight(100);
    //descEdit_.setMaximumWidth(600);
    
    directionCombo_.addItem(QIcon(":/icons/graphics/dependency_oneway.png"), "Up");
    directionCombo_.addItem(QIcon(":/icons/graphics/dependency_oneway.png"), "Down");
    directionCombo_.addItem(QIcon(":/icons/graphics/dependency_twoway.png"), "Bidirectional");

    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel(tr("Description:"), this));
    leftLayout->addWidget(&descEdit_, 1);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel(tr("Direction:"), this));
    rightLayout->addWidget(&directionCombo_);
    rightLayout->addWidget(&lockedCheck_);
    rightLayout->addStretch(1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addLayout(leftLayout, 1);
    layout->addLayout(rightLayout);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::~FileDependencyInfoWidget()
//-----------------------------------------------------------------------------
FileDependencyInfoWidget::~FileDependencyInfoWidget()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::setEditedDependency()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::setEditedDependency(QSharedPointer<FileDependency> dependency)
{
    dependency_ = dependency;
    // TODO: Update contents of all widgets.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::getEditedDependency()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependency> FileDependencyInfoWidget::getEditedDependency() const
{
    return dependency_;
}
