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
#include "FileDependencyItem.h"

#include <LibraryManager/libraryinterface.h>

#include <models/component.h>
#include <models/fileset.h>
#include <models/file.h>

#include <PluginSystem/PluginManager.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfoList>
#include <QSettings>
#include <QHeaderView>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QSharedPointer<Component> component,
                                           LibraryInterface* libInterface,
                                           PluginManager& pluginMgr, QWidget* parent)
    : QWidget(parent),
      toolbar_(this),
      progressBar_(this),
      graphWidget_(this),
      infoWidget_(this),
      component_(component),
      libInterface_(libInterface),
      fileTypeLookup_(),
      model_(pluginMgr, component, QFileInfo(libInterface_->getPath(*component_->getVlnv())).path() + "/"),
      xmlPath_()
{
    // Initialize the widgets.
    progressBar_.setStyleSheet("QProgressBar:horizontal { margin: 0px; border: none; background: #cccccc; } "
                               "QProgressBar::chunk:horizontal { background: #009eff;}");
    progressBar_.setFixedHeight(2);
    progressBar_.setTextVisible(false);
    progressBar_.setContentsMargins(0, 0, 0, 0);
    progressBar_.setValue(0);

    graphWidget_.setContentsMargins(0, 0, 0, 0);
    graphWidget_.getView().setModel(&model_);

    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_TREE);
    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_STATUS);
    graphWidget_.getView().resizeColumnToContents(FILE_DEPENDENCY_COLUMN_CREATE);
    graphWidget_.getView().setColumnWidth(FILE_DEPENDENCY_COLUMN_PATH, 250);
    graphWidget_.getView().header()->setResizeMode(FILE_DEPENDENCY_COLUMN_TREE, QHeaderView::Fixed);
    graphWidget_.getView().header()->setResizeMode(FILE_DEPENDENCY_COLUMN_STATUS, QHeaderView::Fixed);
    graphWidget_.getView().header()->setResizeMode(FILE_DEPENDENCY_COLUMN_CREATE, QHeaderView::Fixed);

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
    toolbar_.addAction(QIcon(":/icons/graphics/refresh_16x16.png"), "Rescan",
                       this, SLOT(scan()));

    // Create the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&toolbar_);
    layout->addWidget(&progressBar_);
    layout->addWidget(&graphWidget_, 1);
    layout->addWidget(&infoWidget_);
    layout->setContentsMargins(0, 0, 0, 0);
    //layout->setSpacing(1);

    // Resolve plugins and save the component's xml path.
    xmlPath_ = QFileInfo(libInterface_->getPath(*component_->getVlnv())).path();

    connect(&model_, SIGNAL(analysisProgressChanged(int)),
            this, SLOT(updateProgressBar(int)), Qt::UniqueConnection);
    connect(&graphWidget_.getView(), SIGNAL(selectionChanged(FileDependency*)),
            &infoWidget_, SLOT(setEditedDependency(FileDependency*)), Qt::UniqueConnection);

    //scan();
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
    // Show the source directories dialog.
    FileDependencySourceDialog dialog(xmlPath_, component_->getSourceDirectories(), this);

    if (dialog.exec() == QDialog::Accepted)
    {
        component_->setSourceDirectories(dialog.getSourceDirectories());
        scan();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scan()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scan()
{
    // Preparations. Resolve file types for each extension.
    resolveExtensionFileTypes();

    // Phase 1. Scan all files and folders in the source paths recursively.
    model_.beginReset();
    
    foreach (QString const& sourcePath, component_->getSourceDirectories())
    {
        scanFiles(sourcePath);
    }

    model_.endReset();
    graphWidget_.getView().expandAll();

    // Phase 2. Run the dependency analysis.
    progressBar_.setMaximum(model_.getTotalFileCount());
    model_.startAnalysis();

    emit fileSetsUpdated();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::resolveExtensionFileSets()
//-----------------------------------------------------------------------------
void FileDependencyEditor::resolveExtensionFileTypes()
{
    fileTypeLookup_.clear();

    // Retrieve the file types information from the settings.
    QSettings settings;
    settings.beginGroup("FileTypes");

    QStringList fileTypes = settings.childKeys();

    foreach (QString const& fileType, fileTypes)
    {
        // Enumerate all extensions for the currently investigated file type.
        QStringList extensions = settings.value(fileType).toString().split(';');

        foreach (QString const& ext, extensions)
        {
            // Add to the lookup map only if the extension is not already in use.
            if (!fileTypeLookup_.contains(ext))
            {
                fileTypeLookup_.insert(ext, fileType);
            }
        }
    }

    settings.endGroup();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::scanFiles()
//-----------------------------------------------------------------------------
void FileDependencyEditor::scanFiles(QString const& path)
{
    QString relativePath = General::getRelativePath(xmlPath_, path);
    FileDependencyItem* folderItem = model_.addFolder(relativePath);

    QFileInfoList list = QDir(path).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QFileInfo const& info, list)
    {
        // Check if the entry is a directory.
        if (info.isDir())
        {
            scanFiles(info.filePath());
        }
        else
        {
            // Otherwise the entry is a file.
            // Check which file type corresponds to the extension.
            QString fileType = fileTypeLookup_.value(info.completeSuffix(), "unknown");

            // Check if the file is already packaged into the metadata.
            QString relativePath = General::getRelativePath(xmlPath_, info.absoluteFilePath());

            QList<File*> fileRefs;
            component_->getFiles(relativePath, fileRefs);

            if (fileRefs.empty())
            {
                // Check if the file set does not exist in the component.
                FileSet* fileSet = component_->getFileSet(fileType + "s");

                if (fileSet == 0)
                {
                    fileSet = new FileSet(fileType + "s", "");
                    component_->addFileSet(fileSet);
                }

                File* file = new File(relativePath, fileSet);
                file->addFileType(fileType);
                fileSet->addFile(file);

                fileRefs.append(file);
            }

            folderItem->addFile(component_.data(), relativePath, fileRefs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::updateProgressBar()
//-----------------------------------------------------------------------------
void FileDependencyEditor::updateProgressBar(int value)
{
    progressBar_.setValue(value);
}
