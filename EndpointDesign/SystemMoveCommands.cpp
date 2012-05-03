//-----------------------------------------------------------------------------
// File: SystemMoveCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 17.10.2011
//
// Description:
// Undo move commands for system designs.
//-----------------------------------------------------------------------------

#include "SystemMoveCommands.h"

#include "../SystemDesign/SystemColumnLayout.h"
#include "../SystemDesign/SystemColumn.h"
#include "../SystemDesign/SWComponentItem.h"

#include "EndpointConnection.h"
#include "EndpointItem.h"
#include "EndpointStack.h"
#include "MappingComponentItem.h"
#include "ProgramEntityItem.h"

//-----------------------------------------------------------------------------
// Function: SystemColumnMoveCommand()
//-----------------------------------------------------------------------------
SystemColumnMoveCommand::SystemColumnMoveCommand(SystemColumnLayout* layout,
                                                 SystemColumn* column,
                                                 QPointF const& oldPos,
                                                 QUndoCommand* parent) : QUndoCommand(parent),
                                                 layout_(layout), column_(column),
                                                 oldPos_(oldPos), newPos_(column->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~SystemColumnMoveCommand()
//-----------------------------------------------------------------------------
SystemColumnMoveCommand::~SystemColumnMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemColumnMoveCommand::undo()
{
    column_->setPos(oldPos_);
    layout_->onMoveColumn(column_);
    layout_->onReleaseColumn(column_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemColumnMoveCommand::redo()
{
    column_->setPos(newPos_);
    layout_->onMoveColumn(column_);
    layout_->onReleaseColumn(column_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: EndpointConnectionMoveCommand()
//-----------------------------------------------------------------------------
EndpointConnectionMoveCommand::EndpointConnectionMoveCommand(EndpointConnection* conn,
                                                             QList<QPointF> const& oldRoute,
                                                             QUndoCommand* parent) : QUndoCommand(parent),
                                                                                     conn_(conn),
                                                                                     oldRoute_(oldRoute),
                                                                                     newRoute_(conn->getRoute())
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointConnectionMoveCommand()
//-----------------------------------------------------------------------------
EndpointConnectionMoveCommand::~EndpointConnectionMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointConnectionMoveCommand::undo()
{
    conn_->setRoute(oldRoute_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointConnectionMoveCommand::redo()
{
    conn_->setRoute(newRoute_);
}

//-----------------------------------------------------------------------------
// Function: SystemItemMoveCommand()
//-----------------------------------------------------------------------------
SystemItemMoveCommand::SystemItemMoveCommand(ComponentItem* item, QPointF const& oldPos,
                                             IComponentStack* oldStack, QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      oldPos_(oldPos),
      oldStack_(oldStack),
      newPos_(item->scenePos()),
      newStack_(dynamic_cast<IComponentStack*>(item->parentItem()))
{
    Q_ASSERT(oldStack != 0);
}

//-----------------------------------------------------------------------------
// Function: ~SystemItemMoveCommand()
//-----------------------------------------------------------------------------
SystemItemMoveCommand::~SystemItemMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SystemItemMoveCommand::undo()
{
    newStack_->removeItem(item_);

    item_->setPos(oldStack_->mapStackFromScene(oldPos_));
    oldStack_->addItem(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SystemItemMoveCommand::redo()
{
    oldStack_->removeItem(item_);
    
    item_->setPos(newStack_->mapStackFromScene(newPos_));
    newStack_->addItem(item_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ProgramEntityMoveCommand()
//-----------------------------------------------------------------------------
ProgramEntityMoveCommand::ProgramEntityMoveCommand(ProgramEntityItem* item, QPointF const& oldPos,
                                                   QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                                           oldPos_(oldPos),
                                                                           newPos_(item->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~ProgramEntityMoveCommand()
//-----------------------------------------------------------------------------
ProgramEntityMoveCommand::~ProgramEntityMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ProgramEntityMoveCommand::undo()
{
    item_->setPos(oldPos_);

    MappingComponentItem* mappingComp = static_cast<MappingComponentItem*>(item_->parentItem());
    mappingComp->onMoveItem(item_);
    
    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ProgramEntityMoveCommand::redo()
{
    item_->setPos(newPos_);

    MappingComponentItem* mappingComp = static_cast<MappingComponentItem*>(item_->parentItem());
    mappingComp->onMoveItem(item_);    

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: EndpointMoveCommand()
//-----------------------------------------------------------------------------
EndpointMoveCommand::EndpointMoveCommand(EndpointItem* item, QPointF const& oldPos,
                                         QUndoCommand* parent) : QUndoCommand(parent), item_(item),
                                                                 oldPos_(oldPos), newPos_(item->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~EndpointMoveCommand()
//-----------------------------------------------------------------------------
EndpointMoveCommand::~EndpointMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void EndpointMoveCommand::undo()
{
    item_->setPos(oldPos_);

    EndpointStack* stack = static_cast<EndpointStack*>(item_->parentItem());
    stack->onMoveEndpoint(item_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void EndpointMoveCommand::redo()
{
    item_->setPos(newPos_);

    EndpointStack* stack = static_cast<EndpointStack*>(item_->parentItem());
    stack->onMoveEndpoint(item_);    

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: ItemMoveCommand()
//-----------------------------------------------------------------------------
SWPortMoveCommand::SWPortMoveCommand(SWPortItem* port, QPointF const& oldPos,
                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      port_(port),
      oldPos_(oldPos),
      newPos_(port->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~SWPortMoveCommand()
//-----------------------------------------------------------------------------
SWPortMoveCommand::~SWPortMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWPortMoveCommand::undo()
{
    SWComponentItem* comp = static_cast<SWComponentItem*>(port_->parentItem());

    port_->setPos(oldPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWPortMoveCommand::redo()
{
    SWComponentItem* comp = static_cast<SWComponentItem*>(port_->parentItem());

    port_->setPos(newPos_);
    comp->onMovePort(port_);

    // Execute child commands.
    QUndoCommand::redo();
}