//-----------------------------------------------------------------------------
// File: FileDependencyEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#include "FileDependencyEditor.h"

#include "FileDependencySourceDialog.h"

#include <QVBoxLayout>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QWidget* parent)
    : QWidget(parent),
      toolbar_(this),
      progressBar_(this),
      graphWidget_(this),
      infoWidget_(this)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&toolbar_);
    layout->addWidget(&progressBar_);
    layout->addWidget(&graphWidget_, 1);
    layout->addWidget(&infoWidget_);
    layout->setContentsMargins(0, 0, 0, 0);
    //layout->setSpacing(1);
    
    progressBar_.setStyleSheet("QProgressBar:horizontal { margin: 0px; border: none; background: #cccccc; } "
                               "QProgressBar::chunk:horizontal { background: #009eff;}");
    progressBar_.setFixedHeight(2);
    progressBar_.setTextVisible(false);
    progressBar_.setContentsMargins(0, 0, 0, 0);
    progressBar_.setValue(50);

    graphWidget_.setContentsMargins(0, 0, 0, 0);

    toolbar_.setFloatable(false);
    toolbar_.setMovable(false);
    toolbar_.setStyleSheet(QString("QToolBar { border: none; }"));
    toolbar_.addAction(QIcon(":/icons/graphics/traffic-light_green.png"), "Show Green");
    toolbar_.addAction(QIcon(":/icons/graphics/traffic-light_yellow.png"), "Show Yellow");
    toolbar_.addAction(QIcon(":/icons/graphics/traffic-light_red.png"), "Show Red");
    toolbar_.addAction(QIcon(":/icons/graphics/dependency_twoway.png"), "Show Bidirectional");
    toolbar_.addAction(QIcon(":/icons/graphics/dependency_oneway.png"), "Show Unidirectional");
    toolbar_.addAction(QIcon(":/icons/graphics/dependency_manual.png"), "Show Manual");
    toolbar_.addAction(QIcon(":/icons/graphics/dependency_auto.png"), "Show Analyzed");
    toolbar_.addAction(QIcon(":/icons/graphics/diff.png"), "Show Differences");
    toolbar_.addSeparator();
    toolbar_.addAction(QIcon(":/icons/graphics/import_folders.png"), "Import Source Directories",
                       this, SLOT(openSourceDialog()));
    toolbar_.addAction(QIcon(":/icons/graphics/refresh_16x16.png"), "Rescan");
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::~FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::~FileDependencyEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::openSourceDialog()
//-----------------------------------------------------------------------------
void FileDependencyEditor::openSourceDialog()
{
    FileDependencySourceDialog dialog(QStringList(), this);

    if (dialog.exec() == QDialog::Accepted)
    {
    }
}
