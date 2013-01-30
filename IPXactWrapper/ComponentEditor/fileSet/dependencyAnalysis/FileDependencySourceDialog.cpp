//-----------------------------------------------------------------------------
// File: FileDependencySourceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Tommi Korhonen
// Date: 22.01.2013
//
// Description:
// Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------

#include "FileDependencySourceDialog.h"

//-----------------------------------------------------------------------------
//! Dialog for choosing the file dependency source directories.
//-----------------------------------------------------------------------------


FileDependencySourceDialog::FileDependencySourceDialog(QStringList const& sourceDirs, QWidget* parent)
{
    mainGroupBox_ = new QGroupBox(tr("Directories"), this);
    buttonAdd_ = new QPushButton(tr("Add"), this);
    buttonRemove_ = new QPushButton(tr("Remove"), this);
    directoryListView_ = new QListView(this);

    horizontalGroupBoxLayout_ = new QHBoxLayout;
    verizontalGroupBoxLayout_ = new QVBoxLayout;
    verizontalMainLayout_ = new QVBoxLayout;

    verizontalGroupBoxLayout_->addWidget(buttonAdd_);
    verizontalGroupBoxLayout_->addWidget(buttonRemove_);
    horizontalGroupBoxLayout_->addWidget(directoryListView_);
    horizontalGroupBoxLayout_->addLayout(verizontalGroupBoxLayout_);
    mainGroupBox_->setLayout(horizontalGroupBoxLayout_);

    buttonBox_ = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    verizontalMainLayout_->addWidget(mainGroupBox_);
    verizontalMainLayout_->addWidget(buttonBox_);

    this->setLayout(verizontalMainLayout_);

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addSource()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeSource()));
    connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));

    directoryListModel_ = new QStringListModel(sourceDirs);
    directoryListView_->setModel(directoryListModel_);
}

FileDependencySourceDialog::~FileDependencySourceDialog()
{
}

QStringList const& FileDependencySourceDialog::getSourceDirectories() const
{
    return directoryListModel_->stringList();
}

void FileDependencySourceDialog::addSource()
{
    QStringList sourceDirectories = directoryListModel_->stringList();
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), QDir::currentPath(), QFileDialog::ReadOnly );

    // Checking if the selected directory has been previously added.
    for( int i = 0; i < sourceDirectories.count(); ++i )
    {
        int subDirectory = 0;
        for( int j = 0; j < newDirectory.count("/"); ++j )
        {
            subDirectory = newDirectory.indexOf("/", subDirectory+1);
            if( newDirectory.left(subDirectory) == sourceDirectories.at(i) )
            {
                return;
            }
            else if( newDirectory == sourceDirectories.at(i))
            {
                return;
            }
        }
    }
    // TODO:
    /* toisinpäin tarkastus
     *
     */
    sourceDirectories.push_back(newDirectory);
    directoryListModel_->setStringList(sourceDirectories);
}

void FileDependencySourceDialog::removeSource()
{
    directoryListModel_->removeRow(directoryListView_->selectionModel()->currentIndex().row());
}
