//-----------------------------------------------------------------------------
// File: FileDependencyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 12.02.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyModel.h"

#include "FileDependencyItem.h"

#include <models/fileset.h>
#include <models/component.h>

#include <PluginSystem/PluginManager.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QIcon>

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::FileDependencyModel(PluginManager& pluginMgr, QSharedPointer<Component> component,
                                         QString const& basePath)
    : pluginMgr_(pluginMgr),
      component_(component),
      basePath_(basePath),
      root_(new FileDependencyItem()),
      timer_(0),
      curFolderIndex_(0),
      curFileIndex_(0),
      progressValue_(0)
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::~FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::~FileDependencyModel()
{
    delete root_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::headerData()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::headerData(int section, Qt::Orientation orientation,
                                         int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        switch (section)
        {
        case FILE_DEPENDENCY_COLUMN_STATUS:
            {
                return tr("Status");
            }

        case FILE_DEPENDENCY_COLUMN_PATH:
            {
                return tr("Path");
            }

        case  FILE_DEPENDENCY_COLUMN_FILESETS:
            {
                return tr("Filesets");
            }

        case FILE_DEPENDENCY_COLUMN_CREATE:
            {
                return tr("#");
            }

        case FILE_DEPENDENCY_COLUMN_DEPENDENCIES:
            {
                return tr("Dependencies");
            }

        case FILE_DEPENDENCY_COLUMN_REFERENCES:
            {
                return tr("References");
            }
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::columnCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::columnCount(const QModelIndex & parent /*= QModelIndex() */) const
{
    return FILE_DEPENDENCY_COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::rowCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    if (parent.isValid())
    {
        return static_cast<FileDependencyItem*>(parent.internalPointer())->getChildCount();
    }
    else
    {
        return root_->getChildCount();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::index()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::index(int row, int column, const QModelIndex &parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    FileDependencyItem* child = parentItem->getChild(row);
    
    if (child == 0)
    {
        return QModelIndex();
    }

    return createIndex(row, column, child);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::parent()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    FileDependencyItem* childItem = static_cast<FileDependencyItem*>(child.internalPointer());
    FileDependencyItem* parent = childItem->getParent();

    if (parent == 0)
    {
        return QModelIndex();
    }

    int index = parent->getIndex();

    if (index < 0)
    {
        return QModelIndex();
    }

    return createIndex(index, 0, parent);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::data()
//-----------------------------------------------------------------------------
QVariant FileDependencyModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    FileDependencyItem* item = static_cast<FileDependencyItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case FILE_DEPENDENCY_COLUMN_PATH:
            {
                return item->getSimplePath();
            }

        case FILE_DEPENDENCY_COLUMN_FILESETS:
            {
                QList<FileSet*> fileSets = item->getFileSets();

                if (fileSets.empty())
                {
                    return tr("[none]");
                }
                else
                {
                    QString str = fileSets[0]->getName();

                    for (int i = 1; i < fileSets.count(); ++i)
                    {
                        str += "; " + fileSets[i]->getName();
                    }

                    return str;
                }
            }
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
        {
            switch (item->getStatus())
            {
            case FILE_DEPENDENCY_STATUS_UNKNOWN:
                {
                    return QIcon(":icons/graphics/traffic-light_gray.png");
                }

            case FILE_DEPENDENCY_STATUS_OK:
                {
                    return QIcon(":icons/graphics/traffic-light_green.png");
                }

            case FILE_DEPENDENCY_STATUS_CHANGED:
                {
                    return QIcon(":icons/graphics/traffic-light_yellow.png");
                }

            case FILE_DEPENDENCY_STATUS_CHANGED2:
                {
                    return QIcon(":icons/graphics/traffic-light_red.png");
                }
            }
        }
    }
    else if (role == Qt::SizeHintRole)
    {
        if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
        {
            return QSize(16, 16);
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            return QColor(230, 230, 230);
        }
        else
        {
            return QColor(Qt::white);
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::hasChildren()
//-----------------------------------------------------------------------------
bool FileDependencyModel::hasChildren(const QModelIndex& parent /*= QModelIndex()*/) const
{
    FileDependencyItem* parentItem = root_;

    if (parent.isValid())
    {
        parentItem = static_cast<FileDependencyItem*>(parent.internalPointer());
    }

    return (parentItem->getChildCount() > 0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags FileDependencyModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    switch (index.column())
    {
    case FILE_DEPENDENCY_COLUMN_STATUS:
    case FILE_DEPENDENCY_COLUMN_CREATE:
    case FILE_DEPENDENCY_COLUMN_DEPENDENCIES:
        return Qt::NoItemFlags;

    default:
        return Qt::ItemIsEnabled;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::startAnalysis()
//-----------------------------------------------------------------------------
void FileDependencyModel::startAnalysis()
{
    resolvePlugins();

    // Begin analysis for each plugin.
    foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
    {
        plugin->beginAnalysis(component_.data());
    }

    // Reset state variables.
    curFolderIndex_ = 0;
    curFileIndex_ = 0;
    progressValue_ = 0;
    emit analysisProgressChanged(progressValue_ + 1);

    // Start the analysis timer.
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(performAnalysisStep()));
    timer_->start();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addFolder()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::addFolder(QString const& path)
{
    return root_->addFolder(0, path);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::beginReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::beginReset()
{
    beginResetModel();
    delete root_;
    root_ = new FileDependencyItem();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::endReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::endReset()
{
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::performAnalysisStep()
//-----------------------------------------------------------------------------
void FileDependencyModel::performAnalysisStep()
{
    // Safe-check whether the analysis has already ended.
    if (curFolderIndex_ == root_->getChildCount())
    {
        return;
    }

    // Run the dependency analysis for the current file.
    FileDependencyItem* fileItem = root_->getChild(curFolderIndex_)->getChild(curFileIndex_);
    analyze(fileItem);


    ++curFileIndex_;
    ++progressValue_;

    // Check if all files in the current folder have been analyzed.
    while (curFileIndex_ == root_->getChild(curFolderIndex_)->getChildCount())
    {
        // Update the status of the folder and continue to the next folder.
        FileDependencyItem* folderItem = root_->getChild(curFolderIndex_);
        folderItem->updateStatus();
        emit dataChanged(getItemIndex(folderItem, 0), getItemIndex(folderItem, FILE_DEPENDENCY_COLUMN_DEPENDENCIES));

        ++curFolderIndex_;
        curFileIndex_ = 0;

        if (curFolderIndex_ == root_->getChildCount())
        {
            break;
        }
    }

    // Stop the timer when there are no more folders.
    if (curFolderIndex_ == root_->getChildCount())
    {
        timer_->stop();
        delete timer_;

        // Reset the progress.
        emit analysisProgressChanged(0);

        // End analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->endAnalysis(component_.data());
        }
    }
    else
    {
        // Otherwise notify progress of the next file.
        emit analysisProgressChanged(progressValue_ + 1);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::getTotalFileCount()
//-----------------------------------------------------------------------------
int FileDependencyModel::getTotalFileCount() const
{
    int count = 0;

    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        count += root_->getChild(i)->getChildCount();
    }

    return count;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::updateData()
//-----------------------------------------------------------------------------
QModelIndex FileDependencyModel::getItemIndex(FileDependencyItem* item, int column) const
{
    if (item == 0)
    {
        return QModelIndex();
    }

    FileDependencyItem* parent = item->getParent();

    if (parent == 0)
    {
        return QModelIndex();
    }

    return createIndex(item->getIndex(), column, item);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::resolvePlugins()
//-----------------------------------------------------------------------------
void FileDependencyModel::resolvePlugins()
{
    analyzerPluginMap_.clear();
    usedPlugins_.clear();

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

            usedPlugins_.append(analyzer);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::analyze()
//-----------------------------------------------------------------------------
void FileDependencyModel::analyze(FileDependencyItem* fileItem)
{
    Q_ASSERT(fileItem != 0);

    // Retrieve the corresponding plugin based on the file type.
    ISourceAnalyzerPlugin* plugin;
    
    foreach (QString const& fileType, fileItem->getFileTypes())
    {
        plugin = analyzerPluginMap_.value(fileType);

        if (plugin != 0)
        {
            break;
        }
    }

    if (plugin != 0)
    {
        // Check the file for modifications by calculating its hash and comparing to the saved value.
        QString absPath = General::getAbsolutePath(basePath_, fileItem->getPath());
        QString hash = plugin->calculateHash(absPath);
        QString lastHash = fileItem->getLastHash();

        if (!lastHash.isEmpty() && hash != lastHash)
        {
            fileItem->setStatus(FILE_DEPENDENCY_STATUS_CHANGED);
        }
        else
        {
            fileItem->setStatus(FILE_DEPENDENCY_STATUS_OK);
        }

        fileItem->setLastHash(hash);
    }
    else
    {
        fileItem->setStatus(FILE_DEPENDENCY_STATUS_OK);
    }

    //emit dataChanged(getItemIndex(fileItem, 0), getItemIndex(fileItem, FILE_DEPENDENCY_COLUMN_DEPENDENCIES));
}
