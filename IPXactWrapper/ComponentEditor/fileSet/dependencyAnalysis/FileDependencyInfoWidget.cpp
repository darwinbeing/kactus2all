//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
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

    connect(&directionCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(directionComboChanged(int)));
    connect(&lockedCheck_, SIGNAL(stateChanged(int)), this, SLOT(lockedCheckChanged(int)));
    connect(&descEdit_, SIGNAL(textChanged()), this, SLOT(descEditTextChanged()));
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
    
    // Clearing the widgets.
    descEdit_.clear();
    directionCombo_.setCurrentIndex(0);
    lockedCheck_.setChecked(false);
    // Disabling the widgets.
    descEdit_.setEnabled(false);
    directionCombo_.setEnabled(false);
    lockedCheck_.setEnabled(false);
    
    if(dependency_ != NULL)
    {
        // Enabling the widgets.
        descEdit_.setEnabled(true);
        
        // Enable editing for direction and locking only for manual dependencies.
        if(dependency_->isManual())
        {
            directionCombo_.setEnabled(true);
            lockedCheck_.setEnabled(true);
        }

        // Read locked state.
        lockedCheck_.setChecked(dependency_->isLocked());
        // Set previous text.
        descEdit_.setPlainText(dependency_->getDescription());
        // Set direction combobox.
        if(dependency_->isBidirectional())
        {
            directionCombo_.setCurrentIndex(2);
        }
        else
        {
            directionCombo_.setCurrentIndex(0);
        }
    }
    
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::getEditedDependency()
//-----------------------------------------------------------------------------
QSharedPointer<FileDependency> FileDependencyInfoWidget::getEditedDependency() const
{
    return dependency_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::directionComboChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::directionComboChanged(int index)
{
    if( index == 2 )
    {
        dependency_->setBidirectional(true);
    }
    else
    {
        dependency_->setBidirectional(false);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::lockedCheckChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::lockedCheckChanged(int state)
{
    dependency_->setLocked(lockedCheck_.isChecked());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyInfoWidget::descEditTextChanged()
//-----------------------------------------------------------------------------
void FileDependencyInfoWidget::descEditTextChanged()
{
    dependency_->setDescription(descEdit_.toPlainText());
}