//-----------------------------------------------------------------------------
// File: SWCompItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 30.4.2012
//
// Description:
// Graphics item for visualizing SW components.
//-----------------------------------------------------------------------------

#include "SWCompItem.h"

#include "SWPortItem.h"
#include "SWConnection.h"

#include "../EndpointDesign/SystemMoveCommands.h"

#include "IComponentStack.h"
#include "SystemDesignDiagram.h"

#include <models/component.h>
#include <models/model.h>

#include <LibraryManager/libraryinterface.h>

#include <common/GenericEditProvider.h>

#include <QBrush>
#include <QUndoCommand>

//-----------------------------------------------------------------------------
// Function: SWCompItem::SWCompItem()
//-----------------------------------------------------------------------------
SWCompItem::SWCompItem(LibraryInterface* libInterface,
                       QSharedPointer<Component> component,
                       QString const& instanceName,
                       QString const& displayName,
                       QString const& description,
                       QMap<QString, QString> const& configurableElementValues,
                       unsigned int id)
    : SWComponentItem(QRectF(-WIDTH / 2, 0, WIDTH, MIN_HEIGHT), libInterface, component, instanceName,
                      displayName, description, configurableElementValues, 0),
      id_(id),
      oldStack_(0),
      oldPos_(),
      hierIcon_(0)
{
    setFlag(ItemIsMovable);
    
    updateComponent();
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: SWCompItem::~SWCompItem()
//-----------------------------------------------------------------------------
SWCompItem::~SWCompItem()
{
    // Remove this item from the stack where it resides.
    IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());

    if (stack != 0)
    {
        stack->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setZValue(1001.0);
    ComponentItem::mousePressEvent(event);

    oldPos_ = scenePos();
    oldStack_ = dynamic_cast<IComponentStack*>(parentItem());
    oldStackPos_ = parentItem()->scenePos();

    // Begin the position update for the connections.
    foreach (QGraphicsItem* item, childItems())
    {
        if (item->type() == SWPortItem::Type)
        {
            SWPortItem* port = qgraphicsitem_cast<SWPortItem*>(item);

            foreach (SWConnection* conn, port->getConnections())
            {
                conn->beginUpdatePosition();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<SystemDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    setConnectionUpdateDisabled(true);

    ComponentItem::mouseMoveEvent(event);

    if (oldStack_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldStackPos_ + pos()));

        IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onMoveItem(this);
    }

    setConnectionUpdateDisabled(false);

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != SWPortItem::Type)
        {
            continue;
        }

        SWPortItem* port = static_cast<SWPortItem*>(item);

        foreach (SWConnection* conn, port->getConnections())
        {
            conn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void SWCompItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldStack_ != 0)
    {
        IComponentStack* stack = dynamic_cast<IComponentStack*>(parentItem());
        Q_ASSERT(stack != 0);
        stack->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new SystemItemMoveCommand(this, oldPos_, oldStack_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the connections and clear the list.
        foreach (QGraphicsItem* item, childItems())
        {
            if (item->type() == SWPortItem::Type)
            {
                SWPortItem* port = qgraphicsitem_cast<SWPortItem*>(item);

                foreach (SWConnection* conn, port->getConnections())
                {
                    conn->endUpdatePosition(cmd.data());
                }
            }
        }
        
        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }

        oldStack_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: SWCompItem::updateComponent()
//-----------------------------------------------------------------------------
void SWCompItem::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        if (!getLibraryInterface()->contains(*vlnv))
        {
            setBrush(QBrush(QColor(0xe8, 0xc5, 0xc5)));
        }
        else
        {
            setBrush(QBrush(QColor(0xce,0xdf,0xff)));
        }
    }
    else
    {
        setBrush(QBrush(QColor(217, 217, 217)));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (componentModel()->getModel()->hasHierView())
    {
        if (hierIcon_ == 0)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/graphics/hierarchy.png"), this);
            hierIcon_->setToolTip(tr("Hierarchical"));
            hierIcon_->setPos(58, 6);
        }
    }
    else if (hierIcon_ != 0)
    {
        delete hierIcon_;
    }
}

