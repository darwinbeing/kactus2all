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

#include <QGroupBox>
#include <QToolBar>
#include <QTableWidget>
#include <QProgressBar>

class Component;
class PluginManager;
class ISourceAnalyzerPlugin;

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
     *      @param [in] component  The component being edited.
     *      @param [in] pluginMgr  The plugin manager for accessing plugins.
     *      @param [in] parent     The parent widget.
     */
    FileDependencyEditor(QSharedPointer<Component> component,
                         PluginManager& pluginMgr, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyEditor();

private slots:
    /*!
     *  Opens up the source selection dialog.
     */
    void openSourceDialog();

private slots:
    /*!
     *  Scans the source directories for new files, adds them to the component
     *  file sets and runs the dependency analysis.
     */
    void scan();

private:
    // Disable copying.
    FileDependencyEditor(FileDependencyEditor const& rhs);
    FileDependencyEditor& operator=(FileDependencyEditor const& rhs);

    /*!
     *  Resolves plugins for each file type.
     */
    void resolvePlugins();

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

    //! The plugin manager.
    PluginManager& pluginMgr_;

    //! The current source directories.
    QStringList sourceDirectories_;

    //! Analyzer plugin map for fast access to correct plugin for each file type.
    QMap<QString, ISourceAnalyzerPlugin*> analyzerPluginMap_;

    //! Extension fileset lookup.
    QMap<QString, QString> fileTypeLookup_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYEDITOR_H
