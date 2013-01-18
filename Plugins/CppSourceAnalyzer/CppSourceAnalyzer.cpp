//-----------------------------------------------------------------------------
// File: CppSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#include "CppSourceAnalyzer.h"

#include <QtPlugin>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>

#include <PluginSystem/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::CppSourceAnalyzer()
//-----------------------------------------------------------------------------
CppSourceAnalyzer::CppSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::~CppSourceAnalyzer()
//-----------------------------------------------------------------------------
CppSourceAnalyzer::~CppSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::getName()
//----------------------------------------------------------------------------
QString const& CppSourceAnalyzer::getName() const
{
    static QString name("C/C++ Source Analyzer");
    return name;
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::getVersion()
//-----------------------------------------------------------------------------
QString const& CppSourceAnalyzer::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString const& CppSourceAnalyzer::getDescription() const
{
    static QString desc("Analyzes file dependencies from C/C++ files.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::checkFileTypeSupport()
//-----------------------------------------------------------------------------
bool CppSourceAnalyzer::checkFileTypeSupport(QString const& fileType)
{
    return (fileType == "cppSource" || fileType == "cSource");
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
quint64 CppSourceAnalyzer::calculateHash(IPluginUtility* utility, QString const& filename)
{
    // TODO: Hash calculation.
    return 0;
}

//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
void CppSourceAnalyzer::getFileDependencies(IPluginUtility* utility, QString const& filename,
                                            QList<FileDependencyDesc>& dependencies)
{
    // TODO: File dependency retrieval.
}



Q_EXPORT_PLUGIN2(CppSourceAnalyzer, CppSourceAnalyzer)