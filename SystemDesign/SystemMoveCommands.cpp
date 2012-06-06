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

#include "SystemColumn.h"
#include "SWComponentItem.h"

#include <common/graphicsItems/GraphicsConnection.h>

//-----------------------------------------------------------------------------
// Function: SWConnectionMoveCommand()
//-----------------------------------------------------------------------------
SWConnectionMoveCommand::SWConnectionMoveCommand(GraphicsConnection* conn,
                                                 QList<QPointF> const& oldRoute,
                                                 QUndoCommand* parent) : QUndoCommand(parent),
                                                 conn_(conn),
                                                 oldRoute_(oldRoute),
                                                 newRoute_(conn->route())
{
}

//-----------------------------------------------------------------------------
// Function: ~SWConnectionMoveCommand()
//-----------------------------------------------------------------------------
SWConnectionMoveCommand::~SWConnectionMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void SWConnectionMoveCommand::undo()
{
    conn_->setRoute(oldRoute_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void SWConnectionMoveCommand::redo()
{
    conn_->setRoute(newRoute_);
}

//-----------------------------------------------------------------------------
// Function: SystemItemMoveCommand()
//-----------------------------------------------------------------------------
SystemItemMoveCommand::SystemItemMoveCommand(QGraphicsItem* item, QPointF const& oldPos,
                                             IGraphicsItemStack* oldStack, QUndoCommand* parent)
    : QUndoCommand(parent),
      item_(item),
      oldPos_(oldPos),
      oldStack_(oldStack),
      newPos_(item->scenePos()),
      newStack_(dynamic_cast<IGraphicsItemStack*>(item->parentItem()))
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