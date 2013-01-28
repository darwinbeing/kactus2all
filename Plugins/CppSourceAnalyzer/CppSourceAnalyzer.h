//-----------------------------------------------------------------------------
// File: CppSourceAnalyzer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M��tt�
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#ifndef CPPSOURCEANALYZER_H
#define CPPSOURCEANALYZER_H

#include <PluginSystem/IPlugin.h>
#include <PluginSystem/ISourceAnalyzerPlugin.h>

#include <QObject>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class CppSourceAnalyzer : public QObject, public ISourceAnalyzerPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ISourceAnalyzerPlugin)

public:
    CppSourceAnalyzer();
    ~CppSourceAnalyzer();

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
    virtual quint64 calculateHash(IPluginUtility* utility, QString const& filename);

    /*!
     *  Retrieves all file dependencies the given file has.
     *
     *      @param [in]  utility       The plugin utility interface.
     *      @param [in]  filename      The name of the file to which the analysis is run.
     *      @param [out] dependencies  The list of found dependencies.
     */
    virtual void getFileDependencies(IPluginUtility* utility,
                                     QString const& filename,
                                     QList<FileDependencyDesc>& dependencies);

    
private:
    // Disable copying.
    CppSourceAnalyzer(CppSourceAnalyzer const& rhs);
    CppSourceAnalyzer& operator=(CppSourceAnalyzer const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    // TODO: Possible member variables.
};

#endif // CPPSOURCEANALYZER_H
