//-----------------------------------------------------------------------------
// File: GraphicsColumnUndoCommands.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 1.6.2012
//
// Description:
// Undo commands for graphics columns.
//-----------------------------------------------------------------------------

#include "GraphicsColumnUndoCommands.h"

#include "GraphicsColumn.h"
#include "GraphicsColumnLayout.h"

//-----------------------------------------------------------------------------
// Function: GraphicsColumnMoveCommand()
//-----------------------------------------------------------------------------
GraphicsColumnMoveCommand::GraphicsColumnMoveCommand(GraphicsColumnLayout* layout,
                                                     GraphicsColumn* column,
                                                     QPointF const& oldPos,
                                                     QUndoCommand* parent)
    : QUndoCommand(parent),
      layout_(layout),
      column_(column),
      oldPos_(oldPos),
      newPos_(column->pos())
{
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumnMoveCommand()
//-----------------------------------------------------------------------------
GraphicsColumnMoveCommand::~GraphicsColumnMoveCommand()
{
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void GraphicsColumnMoveCommand::undo()
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
void GraphicsColumnMoveCommand::redo()
{
    column_->setPos(newPos_);
    layout_->onMoveColumn(column_);
    layout_->onReleaseColumn(column_);

    // Execute child commands.
    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: GraphicsColumnAddCommand()
//-----------------------------------------------------------------------------
GraphicsColumnAddCommand::GraphicsColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
                                                   QUndoCommand* parent)
    : QUndoCommand(parent),
      layout_(layout),
      column_(column),
      del_(false)
{
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsColumnAddCommand()
//-----------------------------------------------------------------------------
GraphicsColumnAddCommand::~GraphicsColumnAddCommand()
{
    if (del_)
    {
        delete column_;
    }
}

//-----------------------------------------------------------------------------
// Function: undo()
//-----------------------------------------------------------------------------
void GraphicsColumnAddCommand::undo()
{
    Q_ASSERT(column_ != 0);

    // Remove the column from the layout.
    layout_->removeColumn(column_);
    del_ = true;
}

//-----------------------------------------------------------------------------
// Function: redo()
//-----------------------------------------------------------------------------
void GraphicsColumnAddCommand::redo()
{
    // Add the column to the layout.
    layout_->addColumn(column_, true);
    del_ = false;
}