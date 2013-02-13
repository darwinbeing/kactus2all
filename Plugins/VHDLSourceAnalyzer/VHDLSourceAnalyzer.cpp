//-----------------------------------------------------------------------------
// File: VHDLSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Honkonen, Joni-Matti M‰‰tt‰
// Date: 18.1.2013
//
// Description:
// C/C++ source analyzer plugin.
//-----------------------------------------------------------------------------

#include "VHDLSourceAnalyzer.h"

#include <QtPlugin>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QTextStream>

#include <PluginSystem/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::VHDLSourceAnalyzer()
//-----------------------------------------------------------------------------
VHDLSourceAnalyzer::VHDLSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::~VHDLSourceAnalyzer()
//-----------------------------------------------------------------------------
VHDLSourceAnalyzer::~VHDLSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getName()
//----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getName() const
{
    static QString name("VHDL Source Analyzer");
    return name;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getVersion()
//-----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getDescription()
//-----------------------------------------------------------------------------
QString const& VHDLSourceAnalyzer::getDescription() const
{
    static QString desc("Analyzes file dependencies from VHDL files.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::checkFileTypeSupport()
//-----------------------------------------------------------------------------
bool VHDLSourceAnalyzer::checkFileTypeSupport(QString const& fileType)
{
    return (fileType == "vhdlSource");
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::calculateHash(IPluginUtility* utility, QString const& filename)
{
    // TODO:
    return QString();
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::getFileDependencies(IPluginUtility* utility,
                                             QSharedPointer<Component const> component,
                                             QString const& filename,
                                             QList<FileDependencyDesc>& dependencies)
{
    // TODO:
}

Q_EXPORT_PLUGIN2(VHDLSourceAnalyzer, VHDLSourceAnalyzer)