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
    QFileDialog::QFileDialog(parent);
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
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), QDir::currentPath(), QFileDialog::ReadOnly );

    if( newDirectory.size() < 1 )
    {
        return;
    }
    if( !checkIfSelectedDirectoryHasBeenPreviouslyAdded(newDirectory) )
    {
        // Now removing possibly unnecessary directories
        removeUnnecessaryDirectories(newDirectory);
        // Adding the new directory to the list.
        QStringList sourceDirectories = directoryListModel_->stringList();
        sourceDirectories.push_back(newDirectory);
        directoryListModel_->setStringList(sourceDirectories);
    }
}

bool FileDependencySourceDialog::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString newDirectory)
{
    QStringList oldDirectories = directoryListModel_->stringList();
    // Checking if the selected directory has been previously added.
    for(int i = 0; i < oldDirectories.count(); ++i)
    {
        int subDirectory = 0;
        for(int j = 0; j < newDirectory.count("/"); ++j)
        {
            subDirectory = newDirectory.indexOf("/", subDirectory+1);
            if( newDirectory.left(subDirectory) == oldDirectories.at(i) )
            {
                return true;
            }
            else if( newDirectory == oldDirectories.at(i) )
            {
                return true;
            }
        }
    }
    return false;
}

void FileDependencySourceDialog::removeUnnecessaryDirectories(QString newDirectory)
{
    QStringList oldDirectories = directoryListModel_->stringList();
    QStringList tempDirectoryList;
    bool necessaryDirectory = true;
    // Checking if unnecessary directories exist in the list.
    for(int i = 0; i < oldDirectories.count(); ++i)
    {
        int subDirectory = 0;
        necessaryDirectory = true;
        for(int j = 0; j < oldDirectories.at(i).count("/"); ++j)
        {
            subDirectory = oldDirectories.at(i).indexOf("/", subDirectory+1);
            // Checking to see if old directory is contained in the new directory.
            if( newDirectory == oldDirectories.at(i).left(subDirectory))
            {
                // Unnecessary directory found. Marking it unnecessary.
                necessaryDirectory = false;
                break;
            }
        }
        // Checking to see if directory is needed after the new added directory
        if( necessaryDirectory )
        {
            tempDirectoryList.push_back(oldDirectories.at(i));
        }
    }
    // Updating the source directory model.
   directoryListModel_->setStringList(tempDirectoryList);
}

void FileDependencySourceDialog::removeSource()
{
    directoryListModel_->removeRow(directoryListView_->selectionModel()->currentIndex().row());
}
