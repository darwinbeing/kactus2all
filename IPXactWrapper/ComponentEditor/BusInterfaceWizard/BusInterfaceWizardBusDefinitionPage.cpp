//-----------------------------------------------------------------------------
// File: BusInterfaceWizardBusDefinitionPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for editing bus and abstraction definitions.
//-----------------------------------------------------------------------------

#include "BusInterfaceWizardBusDefinitionPage.h"

#include "BusInterfaceWizard.h"

#include <models/component.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <IPXactWrapper/BusEditor/absdefgroup.h>
#include <IPXactWrapper/BusEditor/busdefgroup.h>
#include <IPXactWrapper/ComponentEditor/ports/portsdelegate.h>
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusEditorPage::BusInterfaceWizardBusEditorPage(QSharedPointer<Component> component,
    QSharedPointer<BusInterface> busIf,
    LibraryInterface* lh, BusInterfaceWizard* parent)
    : QWizardPage(parent),
    component_(component),
    busIf_(busIf),
    parent_(parent),
    handler_(lh),
    editor_(this, handler_, QSharedPointer<BusDefinition>(0), QSharedPointer<AbstractionDefinition>(0), true),
    portNamesType_(NAME),
    portMappings_()
{       
    setTitle(tr("Bus Definition"));
    setSubTitle(tr("Verify the logical signals in the bus definition."));
    setFinalPage(false);

    connect(&editor_, SIGNAL(portRenamed(const QString&, const QString&)), 
        this, SLOT(portRenamed(const QString&, const QString&)), Qt::UniqueConnection);
    connect(&editor_, SIGNAL(portRemoved(const QString&, const General::InterfaceMode)), 
        this, SLOT(portRemoved(const QString&, const General::InterfaceMode)), Qt::UniqueConnection);
    
    setupLayout();    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::~BusInterfaceWizardBusDefinitionPage()
//-----------------------------------------------------------------------------
BusInterfaceWizardBusEditorPage::~BusInterfaceWizardBusEditorPage()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::nextId()
//-----------------------------------------------------------------------------
int BusInterfaceWizardBusEditorPage::nextId() const
{
    return BusInterfaceWizard::PAGE_PORTMAP;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::initializePage()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::initializePage()
{       
    bool newAbsDef = (busIf_->getAbstractionType() == parent_->getCreatedAbsDef());
    // Create logical signals from selected ports.
    if (newAbsDef)
    {
        QSharedPointer<LibraryComponent> libComp = handler_->getModel(busIf_->getAbstractionType());
        QSharedPointer<AbstractionDefinition> absDef = libComp.dynamicCast<AbstractionDefinition>();
        if (absDef)
        {
            createLogicalPorts(absDef);
        }
        
        editor_.setAbsDef(absDef);   
    }
    else
    {        
        editor_.setAbsDef(handler_->getModel(busIf_->getAbstractionType()).dynamicCast<AbstractionDefinition>());
    }

    editor_.setBusDef(handler_->getModel(busIf_->getBusType()).dynamicCast<BusDefinition>());
    editor_.setProtection(!newAbsDef);   
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::validatePage()
//-----------------------------------------------------------------------------
bool BusInterfaceWizardBusEditorPage::validatePage()
{    
    QStringList errors;
    bool valid = editor_.validate(errors);

    if (!valid)
    {
        QMessageBox warningDialog(QMessageBox::Warning,
            tr("Warning"),
            tr("Bus definition has the following error(s):\n") + errors.join("\n"),
            QMessageBox::Ok,
            this);        
        warningDialog.exec();
        return false;
    }

    bool newAbsDef = busIf_->getAbstractionType() == parent_->getCreatedAbsDef();
   
    // Create port maps.
    if (newAbsDef)
    {
        editor_.save();

        QSharedPointer<AbstractionDefinition> absDef = handler_->getModel(busIf_->getAbstractionType()).dynamicCast<AbstractionDefinition>();
        if (absDef && !absDef->getPorts().isEmpty())
        {
            QList<QSharedPointer<PortAbstraction> >  ports = absDef->getPorts();
            QList<QSharedPointer<General::PortMap> > portMaps;

            foreach(QString port, parent_->getPorts())
            {
                QString absPortName = portMappings_.value(port);                

                // Skip logical signals that are not defined.
                bool found = false;                
                foreach(QSharedPointer<PortAbstraction> absDefPort, ports)
                {
                    if (QString::compare(absDefPort->getLogicalName(), absPortName) == 0)
                    {
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    continue;
                }

                QSharedPointer<Port> physPort = component_->getPort(port);
                int portWidth = absDef->getPortSize(absPortName, busIf_->getInterfaceMode());
                
                if (portWidth < 1)
                {
                    portWidth = component_->getPortWidth(port);
                }
                if (portWidth < 1)
                {
                    portWidth = 1;
                }

                QSharedPointer<General::PortMap> portMap(new General::PortMap());
                portMap->logicalPort_ = absPortName;
                portMap->logicalVector_= QSharedPointer<Vector>(new Vector(portWidth-1, 0));
                portMap->physicalPort_ = port;
                portMap->physicalVector_= QSharedPointer<Vector>(new Vector(portWidth-1, 0));
                portMaps.append(portMap);
            }

            busIf_->setPortMaps(portMaps);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::setSignalNaming()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::setSignalNaming(SignalNaming type)
{
    portNamesType_ = type;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::setupLayout()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(&editor_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusDefinitionPage::createLogicalPorts()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::createLogicalPorts(QSharedPointer<AbstractionDefinition> absDef)
{
    QList<QSharedPointer<PortAbstraction> >  ports;
    QList<QSharedPointer<General::PortMap> > portMaps;

    portMappings_.clear();

    foreach(QString port, parent_->getPorts())
    {
        QSharedPointer<Port> physPort = component_->getPort(port);
        General::Direction portDirection = physPort->getDirection();
        int portWidth = component_->getPortWidth(port);

        QSharedPointer<PortAbstraction> absPort(new PortAbstraction());

        if (portNamesType_ == NAME)
        {
            absPort->setLogicalName(physPort->getName().toUpper());
        } 
        else //if (portNamesType_ == DESCRIPTION)
        {
            absPort->setLogicalName(physPort->getDescription().toUpper());
        }

        if (busIf_->getInterfaceMode() == General::MASTER || 
            busIf_->getInterfaceMode() == General::MIRROREDMASTER)
        {
            absPort->setMasterDirection(portDirection);
            absPort->setSlaveDirection(General::convert2Mirrored(portDirection));            

            if (portWidth > 0)
            {    
                absPort->setMasterWidth(portWidth);
                absPort->setSlaveWidth(portWidth);
            }
        } 
        else if (busIf_->getInterfaceMode() == General::SLAVE ||
            busIf_->getInterfaceMode() == General::MIRROREDSLAVE)
        {
            absPort->setSlaveDirection(portDirection);
            absPort->setMasterDirection(General::convert2Mirrored(portDirection));

            if (portWidth > 0)
            {
                absPort->setMasterWidth(portWidth);
                absPort->setSlaveWidth(portWidth);
            }
        }
        else if (busIf_->getInterfaceMode() == General::SYSTEM ||
            busIf_->getInterfaceMode() == General::MIRROREDSYSTEM)
        {
            absPort->setSystemDirection(portDirection);
            if (portWidth > 0)
            {
                absPort->setSystemWidth(portWidth);
            }
        }
        else
        {
            continue;
        }

        ports.append(absPort);
        portMappings_.insert(port, absPort->getLogicalName());
    }

    absDef->setPorts(ports);    
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::portRenamed()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::portRenamed(const QString& oldName, const QString& newName)
{
    for (QMap<QString, QString>::iterator i = portMappings_.begin(); i != portMappings_.end(); i++)
    {
        if (QString::compare(i.value(), oldName) == 0)
        {
            portMappings_.insert(i.key(), newName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceWizardBusEditorPage::portRemoved()
//-----------------------------------------------------------------------------
void BusInterfaceWizardBusEditorPage::portRemoved(const QString& portName, const General::InterfaceMode mode)
{
    if (mode == busIf_->getInterfaceMode() || mode == General::INTERFACE_MODE_COUNT)
    {
        QStringList removeable;
        for (QMap<QString, QString>::iterator i = portMappings_.begin(); i != portMappings_.end(); i++)
        {
            if (QString::compare(i.value(), portName) == 0 && !removeable.contains(i.key()))
            {
                removeable.append(i.key());
            }
        }

        foreach(QString port, removeable)
        {
            portMappings_.remove(port);
        }
    }       
}