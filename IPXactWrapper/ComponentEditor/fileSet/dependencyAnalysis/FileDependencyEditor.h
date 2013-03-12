//-----------------------------------------------------------------------------
// File: FileDependencyEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYEDITOR_H
#define FILEDEPENDENCYEDITOR_H

#include "FileDependencyGraphWidget.h"
#include "FileDependencyInfoWidget.h"
#include "FileDependencyModel.h"

#include <QGroupBox>
#include <QToolBar>
#include <QTableWidget>
#include <QProgressBar>
#include <QStringList>
#include <QAction>

class Component;
class PluginManager;
class ISourceAnalyzerPlugin;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! File dependency editor which encapsulates the whole dependency UI.
//-----------------------------------------------------------------------------
class FileDependencyEditor : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] component     The component being edited.
     *      @param [in] libInterface  The library interface.
     *      @param [in] pluginMgr     The plugin manager for accessing plugins.
     *      @param [in] parent        The parent widget.
     */
    FileDependencyEditor(QSharedPointer<Component> component,
                         LibraryInterface* libInterface,
                         PluginManager& pluginMgr, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyEditor();

signals:
    //! Emitted when the file sets have been updated.
    void fileSetsUpdated();
    
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private slots:
    /*!
     *  Opens up the source selection dialog.
     */
    void openSourceDialog();

    /*!
     *  Scans the source directories for new files, adds them to the component
     *  file sets and runs the dependency analysis.
     */
    void scan();

    /*!
     *  Updates the progress bar with the given value.
     *
     *      @param [in] value The value to set to the progress bar.
     */
    void updateProgressBar(int value);

    /*!
     *  Sends filters to the dependecy graph
     *
     *      @param [in] the filter being toggled.
     */
    void greenFilter();
    void yellowFilter();
    void redFilter();
    void twoWayFilter();
    void oneWayFilter();
    void manualFilter();
    void automaticFilter();
    void diffFilter();

private:
    // Disable copying.
    FileDependencyEditor(FileDependencyEditor const& rhs);
    FileDependencyEditor& operator=(FileDependencyEditor const& rhs);

    /*!
     *  Resolves the correct file type for each extension specified in the settings.
     */
    void resolveExtensionFileTypes();

    /*!
     *  Scans recursively files in the given path and adds them to the component file sets.
     *
     *      @param [in] path The path to scan.
     */
    void scanFiles(QString const& path);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Toolbar.
    QToolBar toolbar_;

    //! The progress bar.
    QProgressBar progressBar_;

    //! The dependency graph widget.
    FileDependencyGraphWidget graphWidget_;

    //! The dependency info widget.
    FileDependencyInfoWidget infoWidget_;

    //! The component being edited.
    QSharedPointer<Component> component_;

    //! The library interface.
    LibraryInterface* libInterface_;

    //! Extension fileset lookup.
    QMap<QString, QString> fileTypeLookup_;

    //! Ignore list for extensions.
    QStringList ignoreExtList_;

    //! The file dependency model.
    FileDependencyModel model_;

    //! The XML base path for the component.
    QString xmlPath_;

    QAction* filterGreen_;
    QAction* filterYellow_;
    QAction* filterRed_;
    QAction* filterTwoWay_;
    QAction* filterOneWay_;
    QAction* filterManual_;
    QAction* filterAutomatic_;
    QAction* filterDiff_;

    FileDependencyGraphView::DependencyFilters filters_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYEDITOR_H
