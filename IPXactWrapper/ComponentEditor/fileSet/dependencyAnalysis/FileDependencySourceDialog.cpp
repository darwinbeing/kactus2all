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
#include <models/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::FileDependencySourceDialog(QString const& basePath,
                                                       QStringList const& sourceDirs,
                                                       QWidget* parent)
    : QDialog(parent)
{
    basePath_ = basePath;
    setWindowTitle(tr("Import Sources"));

    mainGroupBox_ = new QGroupBox(tr("Directories"), this);
    buttonAdd_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    buttonRemove_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    buttonRemove_->setEnabled(false);

    directoryListView_ = new QListView(this);

    horizontalGroupBoxLayout_ = new QHBoxLayout;
    verizontalMainLayout_ = new QVBoxLayout;

    QDialogButtonBox* addRemoveButtonBox = new QDialogButtonBox(Qt::Vertical);
    addRemoveButtonBox->addButton(buttonAdd_, QDialogButtonBox::ActionRole);
    addRemoveButtonBox->addButton(buttonRemove_, QDialogButtonBox::ActionRole);

    horizontalGroupBoxLayout_->addWidget(directoryListView_);
    horizontalGroupBoxLayout_->addWidget(addRemoveButtonBox);
    mainGroupBox_->setLayout(horizontalGroupBoxLayout_);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    verizontalMainLayout_->addWidget(mainGroupBox_);
    verizontalMainLayout_->addWidget(buttonBox);

    this->setLayout(verizontalMainLayout_);

    connect(buttonAdd_, SIGNAL(clicked()), this, SLOT(addSource()));
    connect(buttonRemove_, SIGNAL(clicked()), this, SLOT(removeSource()));
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()), Qt::UniqueConnection);

    if( sourceDirs.count() > 0 )
    {
        buttonRemove_->setEnabled(true);
    }
    

    for(int i = 0; i < sourceDirs.count(); ++i )
    {
        absolutePaths_.push_back(sourceDirs.at(i));
    }
    toRelative();

    directoryListModel_ = new QStringListModel(relativePaths_);
    directoryListView_->setModel(directoryListModel_);
    resize(600, sizeHint().height());

}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::~FileDependencySourceDialog()
//-----------------------------------------------------------------------------
FileDependencySourceDialog::~FileDependencySourceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::getSourceDirectories()
//-----------------------------------------------------------------------------
QStringList FileDependencySourceDialog::getSourceDirectories() const
{
    return absolutePaths_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::addSource()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::addSource()
{
    QString newDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"), basePath_);
    newDirectory = QFileInfo(newDirectory).filePath();

    if( newDirectory.size() < 1 )
    {
        return;
    }
    if( !checkIfSelectedDirectoryHasBeenPreviouslyAdded(newDirectory) )
    {
        // Now removing possibly unnecessary directories
        removeUnnecessaryDirectories(newDirectory);
        // Adding the new directory to the list.
        absolutePaths_.push_back(newDirectory);
        toRelative();
        directoryListModel_->setStringList(relativePaths_);
        buttonRemove_->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------------------
// Function: FileDependencySourceDialog::checkIfSelectedDirectoryHasBeenPreviouslyAdded()
//-----------------------------------------------------------------------------------------
bool FileDependencySourceDialog::checkIfSelectedDirectoryHasBeenPreviouslyAdded(QString newDirectory)
{
    QStringList oldDirectories = absolutePaths_;
    // Checking if the selected directory has been previously added.
    for(int i = 0; i < oldDirectories.count(); ++i)
    {
        int subDirectory = 0;
        for(int j = 0; j < newDirectory.count("/"); ++j)
        {
            subDirectory = newDirectory.indexOf("/", subDirectory+1);
            // Checking whether old directory is a root directory.
            QString oldDirString = oldDirectories.at(i);
            if( oldDirString.right(1) == "\\" || oldDirString.right(1) == "/" )
            {
                oldDirString = oldDirString.left(oldDirString.size() -1 );
            }

            if( newDirectory.left(subDirectory) == oldDirString )
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

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::removeUnnecessaryDirectories()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::removeUnnecessaryDirectories(QString newDirectory)
{
    QStringList oldDirectories = absolutePaths_;
    QStringList tempDirectoryList;
    bool necessaryDirectory = true;

    // Checking if root directory
    if( newDirectory.right(1) == "\\" || newDirectory.right(1) == "/" )
    {
        newDirectory = newDirectory.left(newDirectory.size() -1 );
    }

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
    absolutePaths_.clear();
    absolutePaths_ = tempDirectoryList;
}

//-----------------------------------------------------------------------------
// Function: FileDependencySourceDialog::removeSource()
//-----------------------------------------------------------------------------
void FileDependencySourceDialog::removeSource()
{
    absolutePaths_.removeAt(directoryListView_->selectionModel()->currentIndex().row());
    toRelative();
    directoryListModel_->setStringList(relativePaths_);
    QStringList list = directoryListModel_->stringList();
    if( list.count() < 1 )
    {
        buttonRemove_->setEnabled(false);
    }
}

void FileDependencySourceDialog::toRelative()
{
    relativePaths_.clear();
    for( int i = 0; i < absolutePaths_.count(); ++i )
    {
        if( absolutePaths_.at(i).at(0) == basePath_.at(0) )
        {
            relativePaths_.push_back(General::getRelativePath(basePath_, absolutePaths_.at(i)));
        }
        else
        {
            relativePaths_.push_back(absolutePaths_.at(i));
        }
    }
}