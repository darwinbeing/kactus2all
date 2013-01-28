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
#include <QCryptographicHash>
#include <QRegExp>

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
    // Try to open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        // File could not be opened, show error
        return 0;
    }

    // Read the file data
    QString fileData;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        fileData.append(line);
    }   
    
        // TODO: Strip comments and whitespace here
        // 1. if no running comment active, look for the first // or /*
        // 2. in case /* is the first one, look if */ is on the same line
        // 3. if running comment is active, look for */ and go to 1.

    // Calculate the hash
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(fileData.toAscii());

    QString result = hash.result().toHex();
    return 0;
}


//-----------------------------------------------------------------------------
// Function: CppSourceAnalyzer::getFileDependencies()
//-----------------------------------------------------------------------------
void CppSourceAnalyzer::getFileDependencies(IPluginUtility* utility, QString const& filename,
                                            QList<FileDependencyDesc>& dependencies)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        // File could not be opened, show error
        return;
    }

    // Read the file data
    QStringList fileData;
    while (!file.atEnd())
    {
        QString line = file.readLine();
        fileData.append(line.simplified());
    }

    // TODO: Strip comments..

    // Find #includes
    for (int i=0; i < fileData.length(); i++)
    {
        const QString* currentLine = &fileData.at(i);
        // include found (can they be somewhere else than start of line?
        int includePosition = currentLine->indexOf("#include");
        if (includePosition == 0)
        {
            int includeStart = 0;
            int includeEnd = 0;
            // includes with < >
            if (currentLine->indexOf("<") != -1)
            {
                includeStart = currentLine->indexOf("<") + 1;
                includeEnd = currentLine->indexOf(">");
            }
            // includes with " "
            else
            {
                includeStart = currentLine->indexOf("\"") + 1;
                includeEnd = currentLine->indexOf("\"", includeStart+1);
            }
            // Include found, TODO: store it
            QString includeName = currentLine->mid(includeStart, includeEnd-includeStart);
        }
    }
}



Q_EXPORT_PLUGIN2(CppSourceAnalyzer, CppSourceAnalyzer)