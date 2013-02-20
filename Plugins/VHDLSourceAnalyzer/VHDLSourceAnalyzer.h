//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
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
     *  Checks whether the plugin supports analysis for the given file type.
     *
     *      @param [in] fileType The file type to check.
     *
     *      @return True, if the plugin supports the file type; false if it doesn't.
     */
    virtual bool checkFileTypeSupport(QString const& fileType);

    /*!
     *  Calculates a language-dependent hash for the given file.
     *
     *      @param [in] utility   The plugin utility interface.
     *      @param [in] filename  The name of the file.
     *
     *      @return The hash value for the file.
     *
     *      @remarks Comments and whitespace are ignored and do not affect the hash value.
     */
    virtual QString calculateHash(IPluginUtility* utility, QString const& filename);

    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  utility       The plugin utility interface.
     *      @param [in]  component     The component to which the dependency scan is being run.
     *      @param [in]  filename      The name of the file to which the analysis is run.
     *      @param [out] dependencies  The list of found dependencies.
     */
    virtual void getFileDependencies(IPluginUtility* utility,
                                     QSharedPointer<Component const> component,
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

    // TODO: Possible member variables.
};

#endif // VHDLSOURCEANALYZER_H
