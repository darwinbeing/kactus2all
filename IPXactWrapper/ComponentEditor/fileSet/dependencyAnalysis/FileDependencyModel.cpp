//-----------------------------------------------------------------------------
// File: FileDependencyModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.02.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#include "FileDependencyModel.h"

#include "FileDependencyItem.h"

#include <models/fileset.h>
#include <models/component.h>
#include <models/FileDependency.h>

#include <PluginSystem/PluginManager.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QIcon>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::FileDependencyModel()
//-----------------------------------------------------------------------------
FileDependencyModel::FileDependencyModel(PluginManager& pluginMgr, QSharedPointer<Component> component,
                                         QString const& basePath)
    : pluginMgr_(pluginMgr),
      component_(component),
      basePath_(basePath),
      root_(new FileDependencyItem()),
      unknownLocation_(0),
      timer_(0),
      curFolderIndex_(0),
      curFileIndex_(0),
      progressValue_(0),
      dependencies_()
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
QVariant FileDependencyModel::headerData(int section, Qt::Orientation /*orientation*/,
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
                return item->getDisplayPath();
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
        if (item->getType() == FileDependencyItem::ITEM_TYPE_FOLDER ||
            item->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION ||
            item->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
        {
            return QColor(230, 230, 230);
        }
//         else
//         {
//             return QColor(Qt::white);
//         }
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

    unknownLocation_ = 0;
    delete root_;
    root_ = new FileDependencyItem();

    // Add the existing dependencies to the model and mark them as removed.
    // The scan will mark preserved dependencies as normal.
    dependencies_.clear();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::endReset()
//-----------------------------------------------------------------------------
void FileDependencyModel::endReset()
{
    endResetModel();

    foreach (QSharedPointer<FileDependency> dependency, component_->getFileDependencies())
    {
        QSharedPointer<FileDependency> copy(new FileDependency(*dependency));
        copy->setStatus(FileDependency::STATUS_UNCHANGED);

        FileDependencyItem* fileItem1 = findFileItem(copy->getFile1());
        FileDependencyItem* fileItem2 = findFileItem(copy->getFile2());

        // First item should always be valid.
        Q_ASSERT(fileItem1 != 0);

        // Check if the second one is an external (not found).
        if (fileItem2 == 0)
        {
            // Create the externals folder if not yet created.
            if (unknownLocation_ == 0)
            {
                beginInsertRows(getItemIndex(root_, 0), root_->getChildCount(),
                    root_->getChildCount());

                unknownLocation_ = root_->addFolder(component_.data(), tr("External"),
                    FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION);
                endInsertRows();
            }

            beginInsertRows(getItemIndex(unknownLocation_, 0),
                unknownLocation_->getChildCount(), unknownLocation_->getChildCount());
            fileItem2 = unknownLocation_->addFile(component_.data(), copy->getFile2(), QList<File*>());
            endInsertRows();
        }

        copy->setItemPointers(fileItem1, fileItem2);
        dependencies_.append(copy);
    }

    emit dependenciesReset();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::performAnalysisStep()
//-----------------------------------------------------------------------------
void FileDependencyModel::performAnalysisStep()
{
    // Safe-check whether the analysis has already ended.
    if (progressValue_ == getTotalStepCount())
    {
        return;
    }

    // On first step begin analysis for each resolved plugin.
    if (progressValue_ == 0)
    {
        resolvePlugins();

        // Begin analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->beginAnalysis(component_.data(), basePath_);
        }

        ++progressValue_;
    }
    // Otherwise scan one file on each step.
    else
    {
        // Run the dependency analysis for the current file.
        if (curFileIndex_ < root_->getChild(curFolderIndex_)->getChildCount())
        {
            FileDependencyItem* fileItem = root_->getChild(curFolderIndex_)->getChild(curFileIndex_);
            analyze(fileItem);

            ++curFileIndex_;
            ++progressValue_;
        }

        // Check if all files in the current folder have been analyzed.
        while (curFileIndex_ == root_->getChild(curFolderIndex_)->getChildCount())
        {
            // Update the status of the folder and continue to the next folder.
            FileDependencyItem* folderItem = root_->getChild(curFolderIndex_);
            folderItem->updateStatus();
            emit dataChanged(getItemIndex(folderItem, 0), getItemIndex(folderItem, FILE_DEPENDENCY_COLUMN_DEPENDENCIES));

            ++curFolderIndex_;
            curFileIndex_ = 0;

            if (curFolderIndex_ == root_->getChildCount() ||
                root_->getChild(curFolderIndex_)->getType() != FileDependencyItem::ITEM_TYPE_FOLDER)
            {
                break;
            }
        }
    }

    // Stop the timer when there are no more folders.
    if (progressValue_ == getTotalStepCount())
    {
        timer_->stop();
        delete timer_;

        // Reset the progress.
        emit analysisProgressChanged(0);

        // End analysis for each plugin.
        foreach (ISourceAnalyzerPlugin* plugin, usedPlugins_)
        {
            plugin->endAnalysis(component_.data(), basePath_);
        }

        // Set the dependencies as pending.
        component_->setPendingFileDependencies(dependencies_);
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
int FileDependencyModel::getTotalStepCount() const
{
    int count = 1;

    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        if (root_->getChild(i)->getType() == FileDependencyItem::ITEM_TYPE_FOLDER)
        {
            count += root_->getChild(i)->getChildCount();
        }
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
        bool dependenciesChanged = false;

        // If the hash has changed, resolve dependencies.
        if (hash != lastHash)
        {
            // Retrieve the old dependencies.
            QList<FileDependency*> oldDependencies;
            findDependencies(fileItem->getPath(), oldDependencies);

            // Scan the current dependencies.
            QList<FileDependencyDesc> dependencyDescs;
            plugin->getFileDependencies(component_.data(), basePath_, absPath, dependencyDescs);

            // Go through all current dependencies.
            foreach (FileDependencyDesc const& desc, dependencyDescs)
            {
                QString file1 = fileItem->getPath();
                QString file2 = General::getRelativePath(basePath_,
                    QFileInfo(QFileInfo(absPath).path() + "/" + desc.filename).canonicalFilePath());

                // Retrieve the item pointers.
                FileDependencyItem* fileItem1 = findFileItem(file1);
                FileDependencyItem* fileItem2 = findFileItem(file2);

                // First item should always be valid.
                Q_ASSERT(fileItem1 != 0);

                // Check if the second file was an external (not found).
                if (fileItem2 == 0)
                {
                    file2 = "External/" + desc.filename;
                    fileItem2 = findFileItem(file2);
                }

                // Check if the dependency already exists.
                FileDependency* found = findDependency(oldDependencies, file1, file2);

                if (found == 0)
                {
                    // Create the item for external file if not found.
                    if (fileItem2 == 0)
                    {
                        // Create the externals folder if not yet created.
                        if (unknownLocation_ == 0)
                        {
                            beginInsertRows(getItemIndex(root_, 0), root_->getChildCount(),
                                root_->getChildCount());

                            unknownLocation_ = root_->addFolder(component_.data(), tr("External"),
                                FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION);
                            endInsertRows();
                        }

                        beginInsertRows(getItemIndex(unknownLocation_, 0),
                            unknownLocation_->getChildCount(), unknownLocation_->getChildCount());
                        fileItem2 = unknownLocation_->addFile(component_.data(), file2, QList<File*>());
                        endInsertRows();
                    }

                    // Create a new dependency if not found.
                    QSharedPointer<FileDependency> dependency(new FileDependency());
                    dependency->setFile1(file1);
                    dependency->setFile2(file2);
                    dependency->setDescription(desc.description);
                    dependency->setItemPointers(fileItem1, fileItem2);
                    dependency->setStatus(FileDependency::STATUS_ADDED);

                    addDependency(dependency);
                    dependenciesChanged = true;
                }
                else
                {
                    // Remove the dependency from the temporary list.
                    oldDependencies.removeOne(found);

                    // Check if the existing dependency needs updating to a bidirectional one.
                    if (!found->isBidirectional() && found->getFile1() != file1)
                    {
                        found->setBidirectional(true);

                        // Combine the descriptions.
                        found->setDescription(found->getDescription() + "\n" + desc.description);
                        emit dependencyChanged(found);
                    }
                    else
                    {
                        // TODO: Update the description even in this case?
                    }
                }
            }

            // Mark all existing old dependencies as removed.
            foreach (FileDependency* dependency, oldDependencies)
            {
                dependency->setStatus(FileDependency::STATUS_REMOVED);
                dependenciesChanged = true;
                emit dependencyChanged(dependency);
            }
        }

        if (!lastHash.isEmpty() && hash != lastHash)
        {
            if (dependenciesChanged)
            {
                fileItem->setStatus(FILE_DEPENDENCY_STATUS_CHANGED2);
            }
            else
            {
                fileItem->setStatus(FILE_DEPENDENCY_STATUS_CHANGED);
            }
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

    emit dataChanged(getItemIndex(fileItem, FILE_DEPENDENCY_COLUMN_STATUS),
                     getItemIndex(fileItem, FILE_DEPENDENCY_COLUMN_STATUS));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findItem()
//-----------------------------------------------------------------------------
FileDependencyItem* FileDependencyModel::findFileItem(QString const& path)
{
    // Extract the folder part of the path.
    QFileInfo info(path);
    QString folderPath = info.path();

    // Search for a matching folder.
    for (int i = 0; i < root_->getChildCount(); ++i)
    {
        FileDependencyItem* folderItem = root_->getChild(i);

        if (folderItem->getPath() == folderPath)
        {
            for (int j = 0; j < folderItem->getChildCount(); ++j)
            {
                FileDependencyItem* fileItem = folderItem->getChild(j);

                if (fileItem->getPath() == path)
                {
                    return fileItem;
                }
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependency()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyModel::findDependency(QList<FileDependency*> const& dependencies,
                                                    QString const& file1, QString const& file2) const
{
    foreach (FileDependency* dependency, dependencies)
    {
        if ((dependency->getFile1() == file1 && dependency->getFile2() == file2) ||
            (dependency->getFile1() == file2 && dependency->getFile2() == file1))
        {
            return dependency;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::addDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::addDependency(QSharedPointer<FileDependency> dependency)
{
    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if ((dep->getFile1() == dependency->getFile1() && dep->getFile2() == dependency->getFile2()) ||
            (dep->isBidirectional() && dep->getFile2() == dependency->getFile1() &&
             dep->getFile1() == dependency->getFile2()))
        {
            // TODO: Print information for the user to the output.
            return;
        }
    }

    // Update the file item pointers if not yet up to date.
    if (dependency->getFileItem1() == 0 || dependency->getFileItem2() == 0)
    {
        FileDependencyItem* fileItem1 = findFileItem(dependency->getFile1());
        FileDependencyItem* fileItem2 = findFileItem(dependency->getFile2());
        dependency->setItemPointers(fileItem1, fileItem2);
    }

    dependencies_.append(dependency);
    emit dependencyAdded(dependency.data());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::removeDependency()
//-----------------------------------------------------------------------------
void FileDependencyModel::removeDependency(FileDependency* dependency)
{
    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep == dependency)
        {
            emit dependencyRemoved(dependency);
            dependencies_.removeOne(dep);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::getDependencies()
//-----------------------------------------------------------------------------
QList< QSharedPointer<FileDependency> > FileDependencyModel::getDependencies() const
{
    return dependencies_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyModel::findDependencies()
//-----------------------------------------------------------------------------
void FileDependencyModel::findDependencies(QString const& file, QList<FileDependency*>& dependencies) const
{
    dependencies.clear();

    foreach (QSharedPointer<FileDependency> dep, dependencies_)
    {
        if (dep->getFile1() == file || (dep->isBidirectional() && dep->getFile2() == file))
        {
            dependencies.append(dep.data());
        }
    }
}

