//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M��tt�
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#ifndef VHDLSOURCEANALYZER_H
#define VHDLSOURCEANALYZER_H

#include <PluginSystem/IPlugin.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>
#include <QFileInfo>

#include <QObject>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class VHDLSourceAnalyzer : public QObject, public ISourceAnalyzerPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ISourceAnalyzerPlugin)

public:
    VHDLSourceAnalyzer();
    ~VHDLSourceAnalyzer();

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const;

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the list of file types this plugin can run analysis for.
     */
    virtual QStringList const& getSupportedFileTypes() const;

    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(QString const& filename);

    /*!
     *  Begins the analysis for the given component.
     *
     *      @param [in] component The component.
     *
     *      @remarks Any preparations needed for the file dependency analysis should be made here.
     */
    virtual void beginAnalysis(Component const* component);

    /*!
     *  Ends the analysis for the given component.
     *
     *      @param [in] component The component.
     *
     *      @remarks Any cleanups needed should be made here.
     */
    virtual void endAnalysis(Component const* component);

    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  component     The component to which the dependency scan is being run.
     *      @param [in]  filename      The name of the file to which the analysis is run.
     *      @param [out] dependencies  The list of found dependencies.
     */
    virtual void getFileDependencies(Component const* component,
                                     QString const& filename,
                                     QList<FileDependencyDesc>& dependencies);

    
private:
    // Disable copying.
    VHDLSourceAnalyzer(VHDLSourceAnalyzer const& rhs);
    VHDLSourceAnalyzer& operator=(VHDLSourceAnalyzer const& rhs);

    /*!
     *
     * Reads source file data from given file. Used by calulateHash and getFileDependencies.
     *
     *      @param [in] file    The file that is read.
     *
     *      @return The meaningful source data of the file, with comments and empty lines removed.
     */
    QString getSourceData(QFile& file);

    /*!
     *
     * Removes comments from a string that consists of a C/C++ source code
     *
     *      @param [inout] data    The input source code with comments.
     *
     *      @return The meaningful source data of the file, with comments removed.
     */
    QString removeComments(QString& source);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The supported file types.
    QStringList fileTypes_;
};

#endif // VHDLSOURCEANALYZER_H
