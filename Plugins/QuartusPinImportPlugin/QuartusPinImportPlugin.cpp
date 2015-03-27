//-----------------------------------------------------------------------------
// File: QuartusPinImportPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.11.2013
//
// Description:
// Kactus2 plugin for Quartus II pin import.
//-----------------------------------------------------------------------------


#include "QuartusPinImportPlugin.h"

#include "QuartusPinSyntax.h"

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/ImportPlugin/Highlighter.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportColors.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QRegularExpression>
#include <QStringList>
#include <QtPlugin>

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::QuartusPinImportPlugin()
//-----------------------------------------------------------------------------
QuartusPinImportPlugin::QuartusPinImportPlugin(): highlighter_(0)
{
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::~QuartusPinImportPlugin()
//-----------------------------------------------------------------------------
QuartusPinImportPlugin::~QuartusPinImportPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getName()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getName() const
{
    static QString name(tr("Quartus II Pin Import"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getVersion()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getVersion() const
{
    static QString version(tr("1.1"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getDescription()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getDescription() const
{
    static QString description(tr("Imports ports from Quartus pin file."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getVendor()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getLicence()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& QuartusPinImportPlugin::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* QuartusPinImportPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> QuartusPinImportPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::setHighlighter()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::setHighlighter(Highlighter* highlighter)
{
    highlighter_ = highlighter;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getSupportedFileTypes()
//-----------------------------------------------------------------------------
QStringList QuartusPinImportPlugin::getSupportedFileTypes() const
{
    return QStringList("quartusPinFile");
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::getCompatibilityWarnings()
//-----------------------------------------------------------------------------
QString QuartusPinImportPlugin::getCompatibilityWarnings() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::import()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::import(QString const& input, QSharedPointer<Component> targetComponent)
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(input, QColor("gray"));
    }

    foreach (QSharedPointer<Port> existingPort, targetComponent->getPorts())
    {
        existingPort->setDirection(General::DIRECTION_PHANTOM);
    }

    foreach (QString line, input.split(QRegularExpression("(\\r\\n?|\\n\\r?)")))
    {
        if (QuartusPinSyntax::pinDefinition.match(line).hasMatch())
        {
            createPort(line, targetComponent);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::createPort()
//-----------------------------------------------------------------------------
void QuartusPinImportPlugin::createPort(QString const& line, QSharedPointer<Component> targetComponent)
{
    if (highlighter_)
    {
        highlighter_->applyFontColor(line, Qt::black);
        highlighter_->applyHighlight(line, ImportColors::PORT);
    }

    QRegularExpressionMatch lineMatch = QuartusPinSyntax::pinDefinition.match(line);
    
    QString portName = lineMatch.captured(QuartusPinSyntax::LOCATION);

    QString direction = lineMatch.captured(QuartusPinSyntax::DIRECTION);
    General::Direction portDirection = parseDirection(direction);

    QString description = lineMatch.captured(QuartusPinSyntax::PINUSAGE);

    QSharedPointer<Port> port = targetComponent->getPort(portName);
    if (port.isNull())
    {
        port = QSharedPointer<Port>(new Port(portName, General::DIRECTION_PHANTOM, 0, 0, "", "", "", ""));
        targetComponent->addPort(port);
    }

    port->setDirection(portDirection);
    port->setDescription(description);
    port->setPortSize(1);
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportPlugin::parseDirection()
//-----------------------------------------------------------------------------
General::Direction QuartusPinImportPlugin::parseDirection(QString const& direction)
{
    if (QRegularExpression("input", QRegularExpression::CaseInsensitiveOption).match(direction).hasMatch())
    {
        return General::IN;
    }
    else if (QRegularExpression("output", QRegularExpression::CaseInsensitiveOption).match(direction).hasMatch())
    {
        return General::OUT;
    }
    else if (QRegularExpression("bidir|power|gnd",
        QRegularExpression::CaseInsensitiveOption).match(direction).hasMatch())
    {
        return General::INOUT;
    }
    else
    {
        return General::DIRECTION_PHANTOM;
    }
}
