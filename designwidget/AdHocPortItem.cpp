//-----------------------------------------------------------------------------
// File: AdHocPortItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#include "AdHocPortItem.h"

#include "HWComponentItem.h"
#include "HWConnection.h"
#include "BusInterfaceDialog.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "OffPageConnectorItem.h"

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>

#include <models/businterface.h>
#include <models/component.h>
#include <models/busdefinition.h>

#include <LibraryManager/libraryinterface.h>

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QVector2D>
#include <QGraphicsScene>

//-----------------------------------------------------------------------------
// Function: AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::AdHocPortItem(Port* port, LibraryInterface* lh,
                                   QGraphicsItem *parent) : HWConnectionEndpoint(parent),
                                                            port_(port),
                                                            lh_(lh),
                                                            temp_(false),
                                                            oldPos_(), oldPortPositions_(),
                                                            offPageConnector_(0)
{

    Q_ASSERT_X(port, "AdHocPortItem constructor",
        "Null BusInterface pointer given as parameter");

    setType(ENDPOINT_TYPE_ADHOC);

    int squareSize = GridSize - 4;
    int halfSquareSize = squareSize / 2;
    /*  /\
     *  ||
     */
    // Create the port shape based on the port direction.
    QPolygonF shape;

    switch (port->getDirection())
    {
    case General::IN:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                  << QPointF(-halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize)
                  << QPointF(0, squareSize);
            break;
        }

    case General::OUT:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                  << QPointF(-halfSquareSize, -halfSquareSize)
                  << QPointF(0, -squareSize)
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize);
            break;
        }

    case General::INOUT:
        {
            shape << QPointF(-halfSquareSize, halfSquareSize)
                  << QPointF(-halfSquareSize, -halfSquareSize)
                  << QPointF(0, -squareSize)
                  << QPointF(halfSquareSize, -halfSquareSize)
                  << QPointF(halfSquareSize, halfSquareSize)
                  << QPointF(0, squareSize);
            break;
        }
    }

    setPolygon(shape);

    nameLabel_ = new QGraphicsTextItem("", this);
    QFont font = nameLabel_->font();
    font.setPointSize(8);
    nameLabel_->setFont(font);
    nameLabel_->setFlag(ItemIgnoresTransformations);
    nameLabel_->setFlag(ItemStacksBehindParent);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setXOffset(0);
    shadow->setYOffset(0);
    shadow->setBlurRadius(5);
    nameLabel_->setGraphicsEffect(shadow);

    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemSendsScenePositionChanges);

    // Create the off-page connector.
    offPageConnector_ = new OffPageConnectorItem(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: ~AdHocPortItem()
//-----------------------------------------------------------------------------
AdHocPortItem::~AdHocPortItem()
{
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void AdHocPortItem::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString AdHocPortItem::name() const
{
    return port_->getName();
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void AdHocPortItem::setName(QString const& name)
{
	port_->setName(name);
	updateInterface();
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getBusInterface()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> AdHocPortItem::getBusInterface() const
{
    return QSharedPointer<BusInterface>();
}

//-----------------------------------------------------------------------------
// Function: updateInterface()
//-----------------------------------------------------------------------------
void AdHocPortItem::updateInterface()
{
    setBrush(QBrush(Qt::black));

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">" +
                        port_->getName() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    qreal nameHeight = nameLabel_->boundingRect().height();

    if (pos().x() < 0)
    {
        nameLabel_->setPos(nameHeight/2, GridSize/2);
    }
    else
    {
        nameLabel_->setPos(-nameHeight/2, GridSize/2 + nameWidth);
    }

    offPageConnector_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: isHierarchical()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isHierarchical() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool AdHocPortItem::onConnect(ConnectionEndpoint const*)
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void AdHocPortItem::onDisconnect(ConnectionEndpoint const*)
{
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool AdHocPortItem::canConnect(ConnectionEndpoint const* other) const
{
    // Ad-hoc connection is not possible to other type of endpoint.
    if (!other->isAdHoc())
    {
        return false;
    }

    if (other->isHierarchical())
    {
        return (port_->getDirection() == General::INOUT ||
                other->getPort()->getDirection() == General::INOUT ||
                port_->getDirection() == other->getPort()->getDirection());

    }
    else
    {
        return (port_->getDirection() == General::INOUT ||
                other->getPort()->getDirection() == General::INOUT ||
                port_->getDirection() == General::IN && other->getPort()->getDirection() == General::OUT ||
                port_->getDirection() == General::OUT && other->getPort()->getDirection() == General::IN);
    }
}

//-----------------------------------------------------------------------------
// Function: encompassingComp()
//-----------------------------------------------------------------------------
ComponentItem* AdHocPortItem::encompassingComp() const
{
    return static_cast<ComponentItem*>(parentItem());
}

//-----------------------------------------------------------------------------
// Function: getOwnerComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> AdHocPortItem::getOwnerComponent() const
{
	ComponentItem* comp = encompassingComp();
	Q_ASSERT(comp);
	QSharedPointer<Component> compModel = comp->componentModel();
	Q_ASSERT(compModel);
	return compModel;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant AdHocPortItem::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    switch (change) {

    case ItemPositionChange:
        {
            if (!parentItem())
            {
                return snapPointToGrid(value.toPointF());
            }

            QPointF pos = value.toPointF();
            QRectF parentRect = qgraphicsitem_cast<HWComponentItem *>(parentItem())->rect();

            if (pos.x() < 0)
            {
                pos.setX(parentRect.left());
            }
            else
            {
                pos.setX(parentRect.right());
            }

            return snapPointToGrid(pos);
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

            qreal nameWidth = nameLabel_->boundingRect().width();
            qreal nameHeight = nameLabel_->boundingRect().height();

            QRectF parentRect = qgraphicsitem_cast<HWComponentItem*>(parentItem())->rect();

            // Check if the port is directed to the left.
            if (pos().x() < 0)
            {
                setDirection(QVector2D(-1.0f, 0.0f));
                nameLabel_->setPos(nameHeight/2, GridSize/2);
            }
            // Otherwise the port is directed to the right.
            else
            {
                setDirection(QVector2D(1.0f, 0.0f));
                nameLabel_->setPos(-nameHeight/2, GridSize/2 + nameWidth);
            }

            break;
        }

    case ItemScenePositionHasChanged:
        // Check if the updates are not disabled.
        if (!static_cast<HWComponentItem*>(parentItem())->isConnectionUpdateDisabled())
        {
            // Update the connections.
            foreach (GraphicsConnection *interconnection, getConnections())
            {
                interconnection->updatePosition();
            }
        }

        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: isDirectionFixed()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isDirectionFixed() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<HWDesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    HWConnectionEndpoint::mouseMoveEvent(event);
    static_cast<HWComponentItem*>(parentItem())->onMovePort(this);
}

//-----------------------------------------------------------------------------
// Function: setTypes()
//-----------------------------------------------------------------------------
void AdHocPortItem::setTypes(VLNV const&, VLNV const&, General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mousePressEvent(event);    
    oldPos_ = pos();

    // Save old port positions for all ports in the parent component.
    foreach (QGraphicsItem* item, parentItem()->childItems())
    {
        if (dynamic_cast<ConnectionEndpoint*>(item) != 0 && item != this)
        {
            ConnectionEndpoint* port = static_cast<ConnectionEndpoint*>(item);
            oldPortPositions_.insert(port, port->pos());
        }
    }

    // Begin the position update for the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void AdHocPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    HWConnectionEndpoint::mouseReleaseEvent(event);

    QSharedPointer<QUndoCommand> cmd;

    // Check if the port position was really changed.
    if (oldPos_ != pos())
    {
        cmd = QSharedPointer<QUndoCommand>(new PortMoveCommand(this, oldPos_));
    }
    else
    {
        cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
    }

    // Determine if the other ports changed their position and create undo commands for them.
    QMap<ConnectionEndpoint*, QPointF>::iterator cur = oldPortPositions_.begin();

    while (cur != oldPortPositions_.end())
    {
        if (cur.key()->pos() != cur.value())
        {
            QUndoCommand* childCmd =
                new PortMoveCommand(static_cast<HWConnectionEndpoint*>(cur.key()), cur.value(), cmd.data());
        }

        ++cur;
    }

    oldPortPositions_.clear();
    
    // End the position update of the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->endUpdatePosition(cmd.data());
    }

    // Add the undo command to the edit stack only if it has changes.
    if (cmd->childCount() > 0 || oldPos_ != pos())
    {
        static_cast<HWDesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
    }
}

//-----------------------------------------------------------------------------
// Function: setInterfaceMode()
//-----------------------------------------------------------------------------
void AdHocPortItem::setInterfaceMode(General::InterfaceMode)
{
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString AdHocPortItem::description() const
{
	Q_ASSERT(port_);
    return port_->getDescription();
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void AdHocPortItem::setDescription(QString const& description)
{
	Q_ASSERT(port_);
	port_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* AdHocPortItem::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::isBus()
//-----------------------------------------------------------------------------
bool AdHocPortItem::isBus() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocPortItem::getPort()
//-----------------------------------------------------------------------------
Port* AdHocPortItem::getPort() const
{
    return port_;
}
