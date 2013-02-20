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

#include <models/component.h>

#include <PluginSystem/PluginManager.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QVBoxLayout>
#include <QIcon>
#include <QFileInfoList>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::FileDependencyEditor()
//-----------------------------------------------------------------------------
FileDependencyEditor::FileDependencyEditor(QSharedPointer<Component> component,
                                           PluginManager& pluginMgr, QWidget* parent)
    : QWidget(parent),
      toolbar_(this),
      progressBar_(this),
      graphWidget_(this),
      infoWidget_(this),
      component_(component),
      pluginMgr_(pluginMgr),
      sourceDirectories_(),
      analyzerPluginMap_(),
      fileTypeLookup_()
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
    toolbar_.addAction(QIcon(":/icons/graphics/refresh_16x16.png"), "Rescan",
                       this, SLOT(scan()));

    resolvePlugins();
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
    FileDependencySourceDialog dialog(sourceDirectories_, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        sourceDirectories_ = dialog.getSourceDirectories();
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
    foreach (QString const& sourcePath, sourceDirectories_)
    {
        scanFiles(sourcePath);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyEditor::resolvePlugins()
//-----------------------------------------------------------------------------
void FileDependencyEditor::resolvePlugins()
{
    analyzerPluginMap_.clear();

    foreach (IPlugin* plugin, pluginMgr_.getPlugins())
    {
        ISourceAnalyzerPlugin* analyzer = dynamic_cast<ISourceAnalyzerPlugin*>(plugin);

        if (analyzer != 0)
        {
            foreach (QString const& fileType, analyzer->getSupportedFileTypes())
            {
                if (!analyzerPluginMap_.contains(fileType))
                {
                    analyzerPluginMap_.insert(fileType, analyzer);
                }
            }
        }
    }
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
    QFileInfoList list = QDir(path).entryInfoList();

    foreach (QFileInfo const& info, list)
    {
        // Check if the entry is a directory.
        if (info.isDir())
        {
            //scanFiles(info.filePath());
        }
        else
        {

        }
    }
}
