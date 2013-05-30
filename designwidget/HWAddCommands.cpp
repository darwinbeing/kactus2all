//-----------------------------------------------------------------------------
// File: HWAddCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 4.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#include "HWAddCommands.h"

#include "HWConnection.h"
#include "BusPortItem.h"
#include "HWComponentItem.h"
#include "BusInterfaceItem.h"

#include <common/graphicsItems/GraphicsColumn.h>

#include <models/businterface.h>

//-----------------------------------------------------------------------------
// Function: PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::PortAddCommand(HWComponentItem* component, QPointF const& pos,
                               QUndoCommand* parent) : QUndoCommand(parent),
                                                       component_(component), pos_(pos),
                                                       port_(0), scene_(component->scene()),
                                                       del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~PortAddCommand()
//-----------------------------------------------------------------------------
PortAddCommand::~PortAddCommand()
{
    if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PortAddCommand::undo()
{
    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);
    del_ = true;

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortAddCommand::redo()
{
    // Add a port to the component.
    if (port_ == 0)
    {
        port_ = component_->addPort(pos_);
    }
    else
    {
        component_->addPort(port_);
    }

    del_ = false;

    // Child commands need not be executed because the other ports change their position
    // in a deterministic way.
    //QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::ConnectionAddCommand(QGraphicsScene* scene, HWConnection* conn,
                                           QUndoCommand* parent) : QUndoCommand(parent),
                                                                   conn_(conn), mode1_(General::MASTER),
                                                                   mode2_(General::MASTER),
                                                                   portMaps_(), scene_(scene),
                                                                   del_(false)
                                                                   
{
    QSharedPointer<BusInterface> busIf1 = conn_->endpoint1()->getBusInterface();
    QSharedPointer<BusInterface> busIf2 = conn_->endpoint2()->getBusInterface();

    if (busIf1 != 0 && busIf1->getBusType().isValid())
    {
        mode1_ = busIf1->getInterfaceMode();

        if (conn_->endpoint1()->isHierarchical())
        {
            portMaps_ = busIf1->getPortMaps();
        }
    }

    if (busIf2 != 0 && busIf2->getBusType().isValid())
    {
        mode2_ = busIf2->getInterfaceMode();

        if (conn_->endpoint2()->isHierarchical())
        {
            portMaps_ = busIf2->getPortMaps();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionAddCommand()
//-----------------------------------------------------------------------------
ConnectionAddCommand::~ConnectionAddCommand()
{
    if (del_)
    {
        delete conn_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::undo()
{
    // Disconnect the ends.
    conn_->disconnectEnds();
    conn_->setSelected(false);

    // Remove the interconnection from the scene.
    scene_->removeItem(conn_);

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionAddCommand::redo()
{
    // Add the back to the scene.
    if (!scene_->items().contains(conn_))
    {
        scene_->addItem(conn_);
    }

    // Connect the ends and set the interface modes and port map for the hierarchical end point.
    if (conn_->connectEnds())
    {
        QSharedPointer<BusInterface> busIf1 = conn_->endpoint1()->getBusInterface();
        QSharedPointer<BusInterface> busIf2 = conn_->endpoint2()->getBusInterface();

        if (busIf1 != 0 && busIf1->getBusType().isValid())
        {
            busIf1->setInterfaceMode(mode1_);

            if (conn_->endpoint1()->isHierarchical())
            {
                busIf1->setPortMaps(portMaps_);
            }

            conn_->endpoint1()->updateInterface();
        }

        if (busIf2 != 0 && busIf2->getBusType().isValid())
        {
            busIf2->setInterfaceMode(mode2_);

            if (conn_->endpoint2()->isHierarchical())
            {
                busIf2->setPortMaps(portMaps_);
            }

            conn_->endpoint2()->updateInterface();
        }
    }

    scene_->clearSelection();
    conn_->setVisible(true);
    conn_->setSelected(true);
    del_ = false;
}

//-----------------------------------------------------------------------------
// Function: PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::PortPasteCommand(HWComponentItem* destComponent, QSharedPointer<Component> srcComponent, 
	                               QPointF const& pos, BusPortItem* port, 
								   QUndoCommand* parent) : QUndoCommand(parent),
                                                           component_(destComponent), pos_(pos),
                                                           port_(port), scene_(destComponent->scene())
{
	// Create child commands for adding physical ports to target component. 
	// Physical ports must have a unique name within the component.
	foreach (QString const& portName, port_->getBusInterface()->getPhysicalPortNames())
	{	
		QString uniquePortName = portName;
		unsigned int count = 0;
		while ( component_->componentModel()->getPort(uniquePortName) != 0 )
		{
			count++;
			uniquePortName = portName + "_" + QString::number(count);
		}

		// Create copies of the physical ports in the source component and rename them.
		QSharedPointer<Port> physPortCopy = QSharedPointer<Port>(new Port(*srcComponent->getPort(portName)));
		physPortCopy->setName(uniquePortName);	

		// If port name changed, it is also changed in bus interface.
		if( uniquePortName != portName )
		{
			foreach (QSharedPointer<General::PortMap> portMap, port_->getBusInterface()->getPortMaps())
			{
				if( portMap->physicalPort_ == portName )
				{
					portMap->physicalPort_ = uniquePortName;
				}
			}
		}

		QUndoCommand* childCmd = new PastePhysicalPortCommand(component_, physPortCopy, this);
	}
}

//-----------------------------------------------------------------------------
// Function: ~PortPasteCommand()
//-----------------------------------------------------------------------------
PortPasteCommand::~PortPasteCommand()
{
	if (del_)
    {
        delete port_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PortPasteCommand::undo()
{
    // Execute child commands.
    QUndoCommand::undo();

    Q_ASSERT(port_ != 0);

    // Remove the port from the component and from the scene
    component_->removePort(port_);
    scene_->removeItem(port_);

    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PortPasteCommand::redo()
{
	QUndoCommand::redo();

	Q_ASSERT(port_ != 0);
	
    // Copy a port to the component.
    component_->addPort(port_);
	port_->updateInterface();
  
    del_ = false;
}


	//-----------------------------------------------------------------------------
// Function: PastePhysicalPortCommand()
//-----------------------------------------------------------------------------
PastePhysicalPortCommand::PastePhysicalPortCommand(HWComponentItem* component, QSharedPointer<Port> port,
                               QUndoCommand* parent) : QUndoCommand(parent),
							   component_(component->componentModel()), port_(port)                                                           
{
}

//-----------------------------------------------------------------------------
// Function: ~PastePhysicalPortCommand()
//-----------------------------------------------------------------------------
PastePhysicalPortCommand::~PastePhysicalPortCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void PastePhysicalPortCommand::undo()
{
    Q_ASSERT(component_ != 0);
    component_->removePort(port_->getName());
	
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void PastePhysicalPortCommand::redo()
{
	Q_ASSERT(component_ != 0);
    component_->addPort(port_);

	// Execute child commands.
    QUndoCommand::redo();
}