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
    // Try to open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        // File could not be opened, show error
        return 0;
    }

    QString source = getSourceData(file);

    // Calculate the hash
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(source.toAscii());

    QString result = hash.result().toHex();
    return result;
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

QString VHDLSourceAnalyzer::getSourceData(QFile& file)
{
    // Read the file data
    QString fileData;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        line = line.simplified();
        if (line == "")
        {
            // Skip empty lines
            continue;
        }
        fileData.append(line.append("\n"));
    }
    
    // Remove comments from the source
    QString finalData = removeComments(fileData);

    return finalData;
}


QString VHDLSourceAnalyzer::removeComments(QString& source)
{
    QTextStream sourceStream(&source);
    QString finalData;
    QString line;
    // Read the data line by line
    while (!sourceStream.atEnd())
    {
        line = sourceStream.readLine();

        // Find start of comment on the line
        if (line.count("--") > 0)
        {
            int index = line.indexOf("--");
            line = line.left(index).simplified();

            // Only add the line if it's not empty after removing comments 
            if (line != "")
            {
                finalData.append(line);
            }
        }
        // No comment on line, just add the line
        else
        {
            finalData.append(line);
        }
    }
    return finalData;
}

Q_EXPORT_PLUGIN2(VHDLSourceAnalyzer, VHDLSourceAnalyzer)