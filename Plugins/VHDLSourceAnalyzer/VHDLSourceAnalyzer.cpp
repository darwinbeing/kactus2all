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
    : fileTypes_()
{
    fileTypes_.append("vhdlSource");
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
// Function: VHDLSourceAnalyzer::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VHDLSourceAnalyzer::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList const& VHDLSourceAnalyzer::getSupportedFileTypes() const
{
    return fileTypes_;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::calculateHash()
//-----------------------------------------------------------------------------
QString VHDLSourceAnalyzer::calculateHash(QString const& filename)
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
void VHDLSourceAnalyzer::getFileDependencies(Component const* component,
                                             QString const& filename,
                                             QList<FileDependencyDesc>& dependencies)
{
    // Try to open the file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        // File could not be opened, end
        return;
    }

    QString line;
    // Read the data line by line
    while (!file.atEnd())
    {
        line = file.readLine();

        // Check if there's a component specification
        if (line.contains("component", Qt::CaseInsensitive))
        {
            int index = line.indexOf("component", Qt::CaseInsensitive);
            // This is a component end, ignore
            if (line.contains("end", Qt::CaseInsensitive) && 
                line.indexOf("end", Qt::CaseInsensitive) < line.indexOf("component", Qt::CaseInsensitive))
            {
                continue;
            }
            // Commented, ignore
            else if (line.contains("--") && line.indexOf("--") < line.indexOf("component", Qt::CaseInsensitive))
            {
                continue;
            }
            // Get the component name
            else
            {
                line = line.right(line.length()-index-9);
                QStringList words = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                // Now the component name should be the first word of the list
                QString componentName = words.at(0).toLower();
                if (cachedEntities_.contains(componentName))
                {
                    // Add all existing entities to the 
                    for (int j=0; j<cachedEntities_[componentName].count(); j++)
                    {
                        FileDependencyDesc dependency;
                        dependency.description = "Component instantiation.";
                        dependency.filename = cachedEntities_[componentName].at(j);
                        dependencies.append(dependency);
                    }
                }
            }
        }
    }
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


//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::removeComments(QString& source)
//-----------------------------------------------------------------------------
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
                finalData.append(line.append("\n"));
            }
        }
        // No comment on line, just add the line
        else
        {
            finalData.append(line.append("\n"));
        }
    }
    return finalData;
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::beginAnalysis()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::beginAnalysis(Component const* component)
{
    scanEntities(component);
}

//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::endAnalysis()
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::endAnalysis(Component const* component)
{
    cachedEntities_.clear();
}


//-----------------------------------------------------------------------------
// Function: VHDLSourceAnalyzer::scanEntities(Component const* component)
//-----------------------------------------------------------------------------
void VHDLSourceAnalyzer::scanEntities(Component const* component)
{
    // Get all the files from component
    QStringList files = component->getFiles();
    
    // Go through all the files
    for (int i=0; i<files.size(); i++)
    {
        // Try to open the file
        QFile file(files.at(i));
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text) )
        {
            // File could not be opened, skip
            continue;
        }

        QString line;
        // Read the data line by line
        while (!file.atEnd())
        {
            line = file.readLine();

            // Check if there's an entity specification
            if (line.contains("entity", Qt::CaseInsensitive))
            {
                int index = line.indexOf("entity", Qt::CaseInsensitive);
                // This is an entity end, ignore
                if (line.contains("end", Qt::CaseInsensitive) && 
                    line.indexOf("end", Qt::CaseInsensitive) < line.indexOf("entity", Qt::CaseInsensitive))
                {
                    continue;
                }
                // Commented, ignore
                else if (line.contains("--") && line.indexOf("--") < line.indexOf("entity", Qt::CaseInsensitive))
                {
                    continue;
                }
                // Get the entity name
                else
                {
                    line = line.right(line.length()-index-6);
                    QStringList words = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                    // Now the entity name should be the first word of the list
                    QString entityName = words.at(0).toLower();
                    cachedEntities_[entityName].append(files.at(i).toLower());
                }
            }
        }
    }
}

Q_EXPORT_PLUGIN2(VHDLSourceAnalyzer, VHDLSourceAnalyzer)