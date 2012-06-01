//-----------------------------------------------------------------------------
// File: DiagramColumn.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 4.4.2011
//
// Description:
// Diagram column class.
//-----------------------------------------------------------------------------

#include "DiagramColumn.h"

#include <QLinearGradient>
#include <QPen>
#include <QFont>
#include <QGraphicsSceneMouseEvent>

#include "ColumnEditDialog.h"
#include "DiagramColumnLayout.h"

#include "../DiagramMoveCommands.h"
#include "../diagraminterface.h"
#include "../diagramcomponent.h"
#include "../diagraminterconnection.h"
#include "../DiagramAdHocInterface.h"
#include "../diagramport.h"
#include "../blockdiagram.h"

#include <common/GenericEditProvider.h>
#include <common/layouts/VStackedLayout.h>
#include <common/layouts/VCollisionLayout.h>

#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: DiagramColumn()
//-----------------------------------------------------------------------------
DiagramColumn::DiagramColumn(ColumnDesc const& desc, DiagramColumnLayout* layout)
    : QGraphicsRectItem(),
      layout_(layout),
      desc_(),
      nameLabel_(0),
      oldPos_(),
      conns_(),
      mouseNearResizeArea_(false),
      oldCursor_()
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setBrush(QBrush(QColor(210, 210, 210)));
    setPen(QPen(Qt::black, 1));
    setAcceptHoverEvents(true);

    // Update the name label.
    nameLabel_ = new QGraphicsTextItem(this);
    nameLabel_->setAcceptHoverEvents(false);
    QFont font = nameLabel_->font();
    font.setBold(true);
    nameLabel_->setFont(font);

    setColumnDesc(desc);
}

//-----------------------------------------------------------------------------
// Function: ~DiagramColumn()
//-----------------------------------------------------------------------------
DiagramColumn::~DiagramColumn()
{
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void DiagramColumn::setName(QString const& name)
{
    desc_.setName(name);
    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: isItemAllowed()
//-----------------------------------------------------------------------------
bool DiagramColumn::isItemAllowed(QGraphicsItem* item) const
{
    return isItemAllowed(item, desc_.getAllowedItems());
}

//-----------------------------------------------------------------------------
// Function: addItem()
//-----------------------------------------------------------------------------
void DiagramColumn::addItem(QGraphicsItem* item, bool load)
{
    // Remove the item from the previous column.
    if (item->parentItem() != 0)
    {
        static_cast<DiagramColumn*>(item->parentItem())->removeItem(item);
    }

    // Map the position to the column's local coordinate system
    // and constrain the item to the horizontal center of the column.
    QPointF pos = mapFromScene(item->scenePos());
    pos.setX(desc_.getWidth() / 2.0);

    item->setPos(pos);
    item->setFlag(ItemStacksBehindParent);

    // Set the column as the parent of the item.
    item->setParentItem(this);

    if (load)
    {
        // Place the item at the correct index in the items list.
        for (int i = 0; i < items_.size(); ++i)
        {
            if (item->y() < items_.at(i)->y())
            {
                items_.insert(i, item);
                return;
            }
        }

        // If we got here, the item must be placed at the end of the list.
        items_.append(item);
    }
    else
    {
        items_.append(item);

        if (desc_.getContentType() == COLUMN_CONTENT_IO)
        {
            VCollisionLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, IO_SPACING);
            VCollisionLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, IO_SPACING);
        }
        else
        {
            VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
            VStackedLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
        }
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: removeItem()
//-----------------------------------------------------------------------------
void DiagramColumn::removeItem(QGraphicsItem* item)
{
    items_.removeAll(item);
    setParentItem(0);

    if (desc_.getContentType() != COLUMN_CONTENT_IO)
    {
        VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: setOffsetY()
//-----------------------------------------------------------------------------
void DiagramColumn::setOffsetY(qreal y)
{
    // Update the rectangle and the label position.
    setRect(0, y, desc_.getWidth(), HEIGHT);
    nameLabel_->setPos(nameLabel_->x(), 5 + y);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<BlockDiagram*>(scene())->isProtected())
    {
        return;
    }

    // If the mouse is moved near the resize area, change the column's width accordingly.
    if (mouseNearResizeArea_)
    {
        qreal snappedRight = (static_cast<int>(event->pos().x() + 10.0) / 20) * 20;
        setWidth(std::max<int>(0, snappedRight - 1));
    }
    else
    {
        // Otherwise handle the movement of the column.
        QGraphicsRectItem::mouseMoveEvent(event);

        setZValue(1001.0);
        layout_->onMoveColumn(this);
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsRectItem::mouseReleaseEvent(event);

    setZValue(0.0);
    layout_->onReleaseColumn(this);

    // Create an undo command only if the position really changed.
    QSharedPointer<QUndoCommand> cmd;

    if (pos() != oldPos_)
    {
        cmd = QSharedPointer<QUndoCommand>(new ColumnMoveCommand(layout_, this, oldPos_));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // End position update for the interconnections.
    foreach (DiagramInterconnection* conn, conns_)
    {
        conn->endUpdatePosition(cmd.data());
    }

    conns_.clear();

    if (cmd->childCount() > 0 || pos() != oldPos_)
    {
        layout_->getEditProvider().addCommand(cmd);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: onMoveItem()
//-----------------------------------------------------------------------------
void DiagramColumn::onMoveItem(QGraphicsItem* item, DiagramColumn* oldColumn)
{
    // Determine the column under the item's current position.
    DiagramColumn* column = layout_->findColumnAt(item->scenePos());

    // Check if item cannot be placed into the column.
    if (column != 0 && !column->isItemAllowed(item))
    {
        // Place to the old column instead.
        column = oldColumn;
    }

    // Check if the column is different than the current one.
    if (column != 0 && column != this)
    {
        // We have to switch the column and update this column's item positions
        // without the moving item.
        items_.removeAll(item);
        
        QPointF newPos = column->mapFromScene(item->scenePos());
        item->setParentItem(column);
        item->setPos(newPos);

        if (desc_.getContentType() != COLUMN_CONTENT_IO)
        {
            VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
        }

        setZValue(0.0);

        // And call the new column's onMoveItem().
        column->onMoveItem(item, oldColumn);
        return;
    }

    setZValue(1001.0);

    // Restrict the position so that the item cannot be placed too high.
    item->setPos(snapPointToGrid(item->x(), std::max(MIN_Y_PLACEMENT - item->boundingRect().top(), item->y())));

    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        VCollisionLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, IO_SPACING);
    }
    else
    {
        VStackedLayout::updateItemMove(items_, item, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onReleaseItem()
//-----------------------------------------------------------------------------
void DiagramColumn::onReleaseItem(QGraphicsItem* item)
{
    setZValue(0.0);

    if (desc_.getContentType() == COLUMN_CONTENT_IO)
    {
        VCollisionLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, IO_SPACING);
    }
    else
    {
        VStackedLayout::setItemPos(items_, item, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: updateItemPositions()
//-----------------------------------------------------------------------------
void DiagramColumn::updateItemPositions()
{
    // Just update the item positions.
    if (desc_.getContentType() != COLUMN_CONTENT_IO)
    {
        VStackedLayout::updateItemPositions(items_, desc_.getWidth() / 2, MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: getName()
//-----------------------------------------------------------------------------
QString const& DiagramColumn::getName() const
{
    return desc_.getName();
}

//-----------------------------------------------------------------------------
// Function: getAllowedItems()
//-----------------------------------------------------------------------------
unsigned int DiagramColumn::getAllowedItems() const
{
    return desc_.getAllowedItems();
}

//-----------------------------------------------------------------------------
// Function: setColumnDesc()
//-----------------------------------------------------------------------------
void DiagramColumn::setColumnDesc(ColumnDesc const& desc)
{
    desc_ = desc;

    switch (desc.getContentType())
    {
    case COLUMN_CONTENT_IO:
        {
            desc_.setAllowedItems(CIT_INTERFACE);
            break;
        }

    case COLUMN_CONTENT_COMPONENTS:
        {
            desc_.setAllowedItems(CIT_COMPONENT);
            break;
        }

    case COLUMN_CONTENT_BUSES:
        {
            desc_.setAllowedItems(CIT_CHANNEL | CIT_BRIDGE);
            break;
        }

    default:
        {
            break;
        }
    }

    // Update the width based on the content type.
    desc_.setWidth(259);

    if (desc_.getAllowedItems() == CIT_INTERFACE)
    {
        desc_.setWidth(119);
    }

    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    updateNameLabel();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getContentType()
//-----------------------------------------------------------------------------
ColumnContentType DiagramColumn::getContentType() const
{
    return desc_.getContentType();
}

//-----------------------------------------------------------------------------
// Function: isAllowedItemsValid()
//-----------------------------------------------------------------------------
bool DiagramColumn::isAllowedItemsValid(unsigned int allowedItems) const
{
    foreach (QGraphicsItem* item, items_)
    {
        if (!isItemAllowed(item, allowedItems))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: isEmpty()
//-----------------------------------------------------------------------------
bool DiagramColumn::isEmpty() const
{
    return items_.empty();
}

//-----------------------------------------------------------------------------
// Function: isItemAllowed()
//-----------------------------------------------------------------------------
bool DiagramColumn::isItemAllowed(QGraphicsItem* item, unsigned int allowedItems) const
{
    switch (item->type())
    {
    case DiagramInterface::Type:
    case DiagramAdHocInterface::Type:
        {
            return (allowedItems & CIT_INTERFACE);
        }

    case DiagramComponent::Type:
        {
            QSharedPointer<Component> comp = qgraphicsitem_cast<DiagramComponent*>(item)->componentModel();

            // Check if this is a packaged component (and has a strict type).
            if (comp->getVlnv()->isValid())
            {
                return ((comp->isBridge() && (allowedItems & CIT_BRIDGE)) ||
                    (comp->isChannel() && (allowedItems & CIT_CHANNEL)) ||
                    (!comp->isBus() && (allowedItems & CIT_COMPONENT)));
            }
            else
            {
                // Otherwise this is an unpacked component and can be of any type.
                return (allowedItems & (CIT_BRIDGE | CIT_CHANNEL | CIT_COMPONENT));
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant DiagramColumn::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemPositionChange:
        {
            return QPointF(value.toPointF().x(), 0.0);
        }
    }

    return QGraphicsRectItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mousePressEvent(event);

    oldPos_ = pos();

    // Begin position update for the interconnections.
    foreach (QGraphicsItem* item, items_)
    {
        if (item->type() == DiagramComponent::Type)
        {
            DiagramComponent* comp = static_cast<DiagramComponent*>(item);

            foreach (QGraphicsItem* childItem, comp->childItems())
            {
                if (childItem->type() == DiagramPort::Type)
                {
                    beginUpdateConnPositions(static_cast<DiagramConnectionEndPoint*>(childItem));
                }
            }
        }
        else if (item->type() == DiagramInterface::Type)
        {
            beginUpdateConnPositions(static_cast<DiagramConnectionEndPoint*>(item));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DiagramColumn::getEditProvider()
{
    return layout_->getEditProvider();
}

//-----------------------------------------------------------------------------
// Function: beginUpdateConnPositions()
//-----------------------------------------------------------------------------
void DiagramColumn::beginUpdateConnPositions(DiagramConnectionEndPoint* endPoint)
{
    foreach (DiagramInterconnection* conn, endPoint->getInterconnections())
    {
        if (!conns_.contains(conn))
        {
            conn->beginUpdatePosition();
            conns_.insert(conn);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: getLayout()
//-----------------------------------------------------------------------------
DiagramColumnLayout& DiagramColumn::getLayout()
{
    return *layout_;
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::updateNameLabel()
//-----------------------------------------------------------------------------
void DiagramColumn::updateNameLabel()
{
    nameLabel_->setHtml("<center>" + desc_.getName() + "</center>");
    nameLabel_->setTextWidth(std::max<unsigned int>(140, desc_.getWidth()));
    nameLabel_->setPos((desc_.getWidth() - nameLabel_->textWidth()) / 2.0, 5.0);
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::getColumnDesc()
//-----------------------------------------------------------------------------
ColumnDesc const& DiagramColumn::getColumnDesc() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::hoverEnterEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverEnterEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::hoverMoveEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverMoveEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::hoverLeaveEvent()
//-----------------------------------------------------------------------------
void DiagramColumn::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsRectItem::hoverLeaveEvent(event);
    updateCursor(event);
}

//-----------------------------------------------------------------------------
// Function: DiagramColumn::updateCursor()
//-----------------------------------------------------------------------------
void DiagramColumn::updateCursor(QGraphicsSceneHoverEvent* event)
{
    if (!static_cast<DesignDiagram*>(scene())->isProtected() &&
        std::abs(event->pos().x() - boundingRect().right()) <= 10)
    {
        if (!mouseNearResizeArea_ )
        {
            oldCursor_ = cursor();
            setCursor(QCursor(Qt::SplitHCursor));
            mouseNearResizeArea_ = true;
        }
    }
    else
    {
        if (mouseNearResizeArea_)
        {
            // Restore the old cursor.
            setCursor(oldCursor_);
            mouseNearResizeArea_ = false;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SystemColumn::setWidth()
//-----------------------------------------------------------------------------
void DiagramColumn::setWidth(unsigned int width)
{
    desc_.setWidth(width);

    setRect(0, 0, desc_.getWidth(), HEIGHT);
    layout_->updateColumnPositions();

    foreach (QGraphicsItem* item, items_)
    {
        item->setX(desc_.getWidth() / 2);
    }

    updateNameLabel();
}
