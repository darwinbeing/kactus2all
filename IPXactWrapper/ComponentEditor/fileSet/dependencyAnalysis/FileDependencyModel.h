//-----------------------------------------------------------------------------
// File: FileDependencyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYMODEL_H
#define FILEDEPENDENCYMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QMap>
#include <QSharedPointer>

class FileDependencyItem;
class FileDependency;
class ISourceAnalyzerPlugin;
class Component;
class PluginManager;

//-----------------------------------------------------------------------------
//! Dependency headers.
//-----------------------------------------------------------------------------
enum FileDependencyColumn
{
    FILE_DEPENDENCY_COLUMN_TREE = 0,
    FILE_DEPENDENCY_COLUMN_STATUS,
    FILE_DEPENDENCY_COLUMN_PATH,
    FILE_DEPENDENCY_COLUMN_FILESETS,
    FILE_DEPENDENCY_COLUMN_REFERENCES,
    FILE_DEPENDENCY_COLUMN_CREATE,
    FILE_DEPENDENCY_COLUMN_DEPENDENCIES,
    FILE_DEPENDENCY_COLUMN_COUNT
};

//-----------------------------------------------------------------------------
//! The data model for the file dependencies.
//-----------------------------------------------------------------------------
class FileDependencyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] pluginMgr  The plugin manager.
     *      @param [in] component  The component to which this model is made.
     *      @param [in] basePath   The component's base path.
     */
    FileDependencyModel(PluginManager& pluginMgr, QSharedPointer<Component> component,
                        QString const& basePath);

    /*!
     *  Destructor.
     */
    ~FileDependencyModel();

    /*!
     *  Applies the models to the component metadata.
     */
    void apply();

    /*!
     *  Adds a new dependency to the model.
     *
     *      @param [in] dependency The file dependency to add.
     */
    void addDependency(QSharedPointer<FileDependency> dependency);

    /*!
     *  Removes a dependency from the model.
     *
     *      @param [in] dependency The file dependency to remove.
     */
    void removeDependency(FileDependency* dependency);

    /*!
     *  Searches for a file item with the given path.
     *
     *      @param [in] path The path.
     *
     *      @return The corresponding item of null if not found in the model.
     */
    FileDependencyItem* findFileItem(QString const& path);

    /*!
     *  Returns the model index of the given file dependency item.
     *
     *      @param [in] item The file dependency item.
     */
    QModelIndex getItemIndex(FileDependencyItem* item, int column) const;

    /*!
     *  Begins reset.
     */
    void beginReset();

    /*!
     *  Ends reset.
     */
    void endReset();

    /*!
     *  Starts the dependency analysis.
     */
    void startAnalysis();

    /*!
     *  Adds a new folder item to the model.
     *
     *      @param [in] path The folder path.
     */
    FileDependencyItem* addFolder(QString const& path);

    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation,
    		                    int role = Qt::DisplayRole) const;

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
    */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the specified object.
     *
     * \param row Row number of the object.
     * \param column Column number of the object.
     * \param parent Model index of the parent of the object.
     *
     * \return QModelIndex that identifies the object.
    */
    virtual QModelIndex index(int row, int column,
    		const QModelIndex &parent = QModelIndex()) const;

    /*! \brief Get the model index of the parent of the object
     *
     * \param child Model index that identifies the child of the object.
     *
     * \return QModelIndex that identifies the parent of the given object.
    */
    virtual QModelIndex parent(const QModelIndex &child) const;

    /*! \brief Get the data associated with given object.
     *
     * \param index Model index that identifies the object that's data is wanted.
     * \param role Specifies the type of data wanted.
     *
     * \return QVariant Containing the requested data.
    */
    virtual QVariant data(const QModelIndex& index,
    		              int role = Qt::DisplayRole) const;

    /*! \brief Does the specified item have child items or not.
     *
     * \param parent Model index identifying the object that's children are asked.
     *
     * \return True if object has child objects.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*! \brief Get the flags that identify possible methods for given object.
     *
     * \param index Model index identifying the object that's flags are requested.
     *
     * \return Qt::ItemFlags that specify how the object can be handled.
    */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Returns the number of files in the model.
     */
    int getTotalStepCount() const;

    /*!
     *  Returns the list of dependencies.
     */
    QList< QSharedPointer<FileDependency> > getDependencies() const;


signals:
    /*!
     *  Emitted when the analysis progress has changed.
     */
    void analysisProgressChanged(int value);

    /*!
     *  Emitted when a file dependency has been added to the model.
     */
    void dependencyAdded(FileDependency* dependency);

    /*!
     *  Emitted when a file dependency has been removed from the model.
     */
    void dependencyRemoved(FileDependency* dependency);

    /*!
     *  Emitted when a file dependency has changed.
     */
    void dependencyChanged(FileDependency* dependency);

private slots:
    /*!
     *  Performs one step of the dependency analysis.
     */
    void performAnalysisStep();

private:
    // Disable copying.
    FileDependencyModel(FileDependencyModel const& rhs);
    FileDependencyModel& operator=(FileDependencyModel const& rhs);

    /*!
     *  Resolves plugins for each file type.
     */
    void resolvePlugins();

    /*!
     *  Analyzes the given file item.
     *
     *      @param [in] fileItem The file item to analyze.
     */
    void analyze(FileDependencyItem* fileItem);

    /*!
     *  Searches for a dependency between the given files.
     *
     *      @param [in] file1  File path of the first file.
     *      @param [in] file2  File path of the second file.
     */
    FileDependency* findDependency(QString const& file1, QString const& file2) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin manager.
    PluginManager& pluginMgr_;

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The component's base path.
    QString basePath_;

    //! The dependency tree root.
    FileDependencyItem* root_;

    //! The externals item (for fast access).
    FileDependencyItem* unknownLocation_;

    //! The timer for running the analysis.
    QTimer* timer_;

    //! The current folder scan index.
    int curFolderIndex_;

    //! The current file scan index.
    int curFileIndex_;

    //! The current analysis progress.
    int progressValue_;

    //! The list of used plugins on the current run.
    QList<ISourceAnalyzerPlugin*> usedPlugins_;

    //! Analyzer plugin map for fast access to correct plugin for each file type.
    QMap<QString, ISourceAnalyzerPlugin*> analyzerPluginMap_;

    //! Dependency list.
    QList< QSharedPointer<FileDependency> > dependencies_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYMODEL_H
