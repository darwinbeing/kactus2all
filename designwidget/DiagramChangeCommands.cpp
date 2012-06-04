//-----------------------------------------------------------------------------
// File: DiagramChangeCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.8.2011
//
// Description:
// Undo change commands for the block diagram.
//-----------------------------------------------------------------------------

#include "DiagramChangeCommands.h"

#include "DiagramDeleteCommands.h"

#include <models/component.h>
#include <common/graphicsItems/ComponentItem.h>

#include <ConfigurationEditor/activeviewmodel.h>

#include "diagraminterconnection.h"
#include "diagramport.h"
#include "DiagramAdHocPort.h"
#include "DiagramAdHocInterface.h"
#include "diagramcomponent.h"
#include "diagraminterface.h"
#include "columnview/DiagramColumn.h"

#include <models/businterface.h>

//-----------------------------------------------------------------------------
// Function: ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::ComponentChangeNameCommand(ComponentItem* component,
                                               QString const& newName,
                                               QUndoCommand* parent) : QUndoCommand(parent),
                                                                       component_(component),
                                                                       oldName_(component->name()),
                                                                       newName_(newName)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentChangeNameCommand()
//-----------------------------------------------------------------------------
ComponentChangeNameCommand::~ComponentChangeNameCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::undo()
{
    component_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentChangeNameCommand::redo()
{
    component_->setName(newName_);
}

ComponentChangeDisplayNameCommand::ComponentChangeDisplayNameCommand(ComponentItem* component,
													   QString const& newDisplayName,
													   QUndoCommand* parent):
QUndoCommand(parent),
component_(component),
oldDisplayName_(component->displayName()),
newDisplayName_(newDisplayName) {
}

ComponentChangeDisplayNameCommand::~ComponentChangeDisplayNameCommand() {
}

void ComponentChangeDisplayNameCommand::undo() {
	component_->setDisplayName(oldDisplayName_);
}

void ComponentChangeDisplayNameCommand::redo() {
	component_->setDisplayName(newDisplayName_);
}

ComponentChangeDescriptionNameCommand::ComponentChangeDescriptionNameCommand(ComponentItem* component,
																			 QString const& newDescription, 
																			 QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldDescription_(component->description()),
newDescription_(newDescription) {
}

ComponentChangeDescriptionNameCommand::~ComponentChangeDescriptionNameCommand() {
}

void ComponentChangeDescriptionNameCommand::undo() {
	component_->setDescription(oldDescription_);
}

void ComponentChangeDescriptionNameCommand::redo() {
	component_->setDescription(newDescription_);
}

ComponentActiveViewChangeCommand::ComponentActiveViewChangeCommand( 
	const QString& instanceName, 
	QString const& oldActiveView, 
	QString const& newActiveView,
	ActiveViewModel* activeViewModel,
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
instanceName_(instanceName),
newViewName_(newActiveView),
oldViewName_(oldActiveView),
activeViewModel_(activeViewModel) {
}

ComponentActiveViewChangeCommand::~ComponentActiveViewChangeCommand() {

}

void ComponentActiveViewChangeCommand::undo() {
	activeViewModel_->setActiveView(instanceName_, oldViewName_);
}

void ComponentActiveViewChangeCommand::redo() {
	activeViewModel_->setActiveView(instanceName_, newViewName_);
}

//-----------------------------------------------------------------------------
// Function: ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::ComponentPacketizeCommand(ComponentItem* component,
                                                     VLNV const& vlnv,
                                                     QUndoCommand* parent) : QUndoCommand(parent),
                                                     component_(component),
                                                     vlnv_(vlnv)
{
}

//-----------------------------------------------------------------------------
// Function: ~ComponentPacketizeCommand()
//-----------------------------------------------------------------------------
ComponentPacketizeCommand::~ComponentPacketizeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::undo()
{
    // Set an empty VLNV.
    component_->componentModel()->setVlnv(VLNV());
    component_->updateComponent();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ComponentPacketizeCommand::redo()
{
    component_->componentModel()->setVlnv(vlnv_);
    component_->updateComponent();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::EndpointChangeCommand(DiagramConnectionEndpoint* endpoint, 
											 QString const& newName,
                                             General::InterfaceMode newMode,
											 QString const& newDescription,
											 QUndoCommand* parent):
QUndoCommand(parent), 
endpoint_(endpoint),
oldName_(endpoint->name()), 
oldMode_(endpoint->getBusInterface()->getInterfaceMode()),
oldDescription_(endpoint->description()),
newName_(newName),
newMode_(newMode),
newDescription_(newDescription) {
}

//-----------------------------------------------------------------------------
// Function: ~EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointChangeCommand::~EndpointChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::undo() {
    endpoint_->getBusInterface()->setInterfaceMode(oldMode_);
    endpoint_->setDescription(oldDescription_);
    endpoint_->setName(oldName_);
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointChangeCommand::redo() {
    endpoint_->getBusInterface()->setInterfaceMode(newMode_);
    endpoint_->setDescription(newDescription_);
    endpoint_->setName(newName_);
    //endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::EndpointNameChangeCommand(ConnectionEndpoint* endpoint, 
                                                     QString const& newName,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
       endpoint_(endpoint),
       oldName_(endpoint->name()), 
       newName_(newName)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointChangeCommand()
//-----------------------------------------------------------------------------
EndpointNameChangeCommand::~EndpointNameChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::undo()
{
    endpoint_->setName(oldName_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointNameChangeCommand::redo()
{
    endpoint_->setName(newName_);
}

//-----------------------------------------------------------------------------
// Function: EndpointDescChangeCommand()
//-----------------------------------------------------------------------------
EndpointDescChangeCommand::EndpointDescChangeCommand(ConnectionEndpoint* endpoint, 
                                                     QString const& newDescription,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent), 
      endpoint_(endpoint),
      oldDescription_(endpoint->description()), 
      newDescription_(newDescription)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointDescChangeCommand()
//-----------------------------------------------------------------------------
EndpointDescChangeCommand::~EndpointDescChangeCommand() {
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::undo()
{
    endpoint_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointDescChangeCommand::redo()
{
    endpoint_->setDescription(newDescription_);
}

//-----------------------------------------------------------------------------
// Function: EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::EndPointTypesCommand(DiagramConnectionEndpoint* endpoint,
                                           VLNV const& oldBusType, VLNV const& oldAbsType,
                                           General::InterfaceMode oldMode,
                                           QString const& oldName, QUndoCommand* parent)
    : QUndoCommand(parent), endpoint_(endpoint), oldBusType_(oldBusType), oldAbsType_(oldAbsType),
      oldMode_(oldMode), oldName_(oldName), newBusType_(), newAbsType_(),
      newMode_(General::MASTER), newName_(""), connModes_()
{
    if (endpoint_->getBusInterface() != 0)
    {
        newBusType_ = endpoint_->getBusInterface()->getBusType();
        newAbsType_ = endpoint_->getBusInterface()->getAbstractionType();
        newMode_ = endpoint_->getBusInterface()->getInterfaceMode();
        newName_ = endpoint_->getBusInterface()->getName();
    }

    // Save the interface modes for each connection.
    foreach (DiagramInterconnection* conn, endpoint_->getInterconnections())
    {
        DiagramConnectionEndpoint* endpoint = conn->endpoint1();

        if (conn->endpoint1() == endpoint_)
        {
            endpoint = conn->endpoint2();
        }
        
        QSharedPointer<BusInterface> busIf = endpoint->getBusInterface();

        if (busIf != 0 && busIf->getBusType().isValid())
        {
            connModes_.insert(endpoint, busIf->getInterfaceMode());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~EndPointTypesCommand()
//-----------------------------------------------------------------------------
EndPointTypesCommand::~EndPointTypesCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::undo()
{
    endpoint_->setTypes(oldBusType_, oldAbsType_, oldMode_);

    if (endpoint_->getBusInterface() != 0 && oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(oldName_);
    }

    endpoint_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointTypesCommand::redo()
{
    endpoint_->setTypes(newBusType_, newAbsType_, newMode_);

    if (oldName_ != newName_)
    {
        endpoint_->getBusInterface()->setName(newName_);
    }

    endpoint_->updateInterface();

    // Set interface modes for the other end points.
    QMap<DiagramConnectionEndpoint*, General::InterfaceMode>::iterator cur = connModes_.begin();

    while (cur != connModes_.end())
    {
        cur.key()->getBusInterface()->setInterfaceMode(cur.value());
        cur.key()->updateInterface();
        ++cur;
    }
}

//-----------------------------------------------------------------------------
// Function: EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::EndPointPortMapCommand(DiagramConnectionEndpoint* endpoint,
                                               QList< QSharedPointer<General::PortMap> > newPortMaps,
                                               QUndoCommand* parent)
    : QUndoCommand(parent), endpoint_(endpoint),
      oldPortMaps_(endpoint->getBusInterface()->getPortMaps()),
      newPortMaps_(newPortMaps)
{
}

//-----------------------------------------------------------------------------
// Function: ~EndPointPortMapCommand()
//-----------------------------------------------------------------------------
EndPointPortMapCommand::~EndPointPortMapCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::undo()
{
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->setPortMaps(oldPortMaps_);
        endpoint_->updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndPointPortMapCommand::redo()
{
    if (endpoint_->isHierarchical())
    {
        endpoint_->getBusInterface()->setPortMaps(newPortMaps_);
        endpoint_->updateInterface();
    }
}

ComponentConfElementChangeCommand::ComponentConfElementChangeCommand( 
	ComponentItem* component, 
	const QMap<QString, QString>& newConfElements, 
	QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
component_(component),
oldConfElements_(component->getConfigurableElements()),
newConfElements_(newConfElements) {
}

ComponentConfElementChangeCommand::~ComponentConfElementChangeCommand() {
}

void ComponentConfElementChangeCommand::undo() {
	component_->setConfigurableElements(oldConfElements_);
}

void ComponentConfElementChangeCommand::redo() {
	component_->setConfigurableElements(newConfElements_);
}

// ConnectionChangeCommand methods
ConnectionChangeCommand::ConnectionChangeCommand(DiagramInterconnection* connection, 
												 const QString& newName, 
												 const QString& newDescription,
												 QUndoCommand* parent /*= 0*/ ):
QUndoCommand(parent),
connection_(connection),
newName_(newName),
newDescription_(newDescription),
oldName_(connection->name()),
oldDescription_(connection->description()) {
}

ConnectionChangeCommand::~ConnectionChangeCommand() {
}

void ConnectionChangeCommand::undo() {
	connection_->setName(oldName_);
	connection_->setDescription(oldDescription_);
}

void ConnectionChangeCommand::redo() {
	connection_->setName(newName_);
	connection_->setDescription(newDescription_);
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::AdHocVisibilityChangeCommand(AdHocEnabled* dataSource, QString const& portName,
                                                           bool newVisibility, QUndoCommand* parent)
    : QUndoCommand(parent),
      dataSource_(dataSource),
      portName_(portName),
      pos_(),
      newVisibility_(newVisibility)
{
    if (!newVisibility_)
    {
        // Create child commands for removing interconnections.
        DiagramConnectionEndpoint* port = dataSource->getDiagramAdHocPort(portName);
        Q_ASSERT(port != 0);

        pos_ = port->scenePos();

        foreach (DiagramInterconnection* conn, port->getInterconnections())
        {
            QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
        }

        foreach (DiagramInterconnection* conn, port->getOffPageConnector()->getInterconnections())
        {
            QUndoCommand* cmd = new ConnectionDeleteCommand(conn, this);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
//-----------------------------------------------------------------------------
AdHocVisibilityChangeCommand::~AdHocVisibilityChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::undo()
{
    dataSource_->setPortAdHocVisible(portName_, !newVisibility_);

    if (!newVisibility_)
    {
        DiagramConnectionEndpoint* port = dataSource_->getDiagramAdHocPort(portName_);
        port->setPos(port->parentItem()->mapFromScene(pos_));

        // 
        DiagramAdHocInterface* adHocIf = dynamic_cast<DiagramAdHocInterface*>(port);

        if (adHocIf != 0)
        {
            GraphicsColumn* column = static_cast<GraphicsColumn*>(adHocIf->parentItem());
            column->onMoveItem(adHocIf);
        }

        DiagramAdHocPort* adHocPort = dynamic_cast<DiagramAdHocPort*>(port);

        if (adHocPort != 0)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(adHocPort->parentItem());
            comp->onMovePort(adHocPort);
        }
    }

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: AdHocVisibilityChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocVisibilityChangeCommand::redo()
{
    // Execute child commands.
    QUndoCommand::redo();

    dataSource_->setPortAdHocVisible(portName_, newVisibility_);
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::AdHocBoundsChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::AdHocBoundsChangeCommand(DiagramInterconnection* connection,
                                                   bool right, int endpointIndex,
                                                   int oldValue, int newValue, QUndoCommand* parent)
    : QUndoCommand(parent),
      connection_(connection),
      right_(right),
      endpointIndex_(endpointIndex),
      oldValue_(oldValue),
      newValue_(newValue)
{
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::~AdHocBoundChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::~AdHocBoundsChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::undo()
{
    if (right_)
    {
        connection_->setAdHocRightBound(endpointIndex_, oldValue_);
    }
    else
    {
        connection_->setAdHocLeftBound(endpointIndex_, oldValue_);
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::redo()
{
    if (right_)
    {
        connection_->setAdHocRightBound(endpointIndex_, newValue_);
    }
    else
    {
        connection_->setAdHocLeftBound(endpointIndex_, newValue_);
    }
}
