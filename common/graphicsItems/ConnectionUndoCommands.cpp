//-----------------------------------------------------------------------------
// File: ConnectionUndoCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.6.2012
//
// Description:
// Undo commands for graphical connections.
//-----------------------------------------------------------------------------

#include "ConnectionUndoCommands.h"

#include "GraphicsConnection.h"

//-----------------------------------------------------------------------------
// Function: ConnectionMoveCommand()
//-----------------------------------------------------------------------------
ConnectionMoveCommand::ConnectionMoveCommand(GraphicsConnection* conn,
                                             QList<QPointF> const& oldRoute,
                                             QUndoCommand* parent)
    : QUndoCommand(parent),
      conn_(conn), oldRoute_(oldRoute),
      newRoute_(conn->route())
{
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionMoveCommand()
//-----------------------------------------------------------------------------
ConnectionMoveCommand::~ConnectionMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionMoveCommand::undo()
{
    conn_->setRoute(oldRoute_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionMoveCommand::redo()
{
    conn_->setRoute(newRoute_);
}

//-----------------------------------------------------------------------------
// Function: ConnectionChangeCommand()
//-----------------------------------------------------------------------------
ConnectionChangeCommand::ConnectionChangeCommand(GraphicsConnection* connection, 
                                                 const QString& newName, 
                                                 const QString& newDescription,
                                                 QUndoCommand* parent /*= 0*/ )
    : QUndoCommand(parent),
      connection_(connection),
      newName_(newName),
      newDescription_(newDescription),
      oldName_(connection->name()),
      oldDescription_(connection->description())
{
}

//-----------------------------------------------------------------------------
// Function: ~ConnectionChangeCommand()
//-----------------------------------------------------------------------------
ConnectionChangeCommand::~ConnectionChangeCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void ConnectionChangeCommand::undo()
{
    connection_->setName(oldName_);
    connection_->setDescription(oldDescription_);
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void ConnectionChangeCommand::redo()
{
    connection_->setName(newName_);
    connection_->setDescription(newDescription_);
}