/* 
 *
 * 		filename: diagramcomponent.cpp
 */

#include "diagramcomponent.h"
#include "diagramport.h"
#include "DiagramAdHocPort.h"
#include "diagraminterconnection.h"
#include "blockdiagram.h"
#include "DiagramMoveCommands.h"

#include "columnview/DiagramColumn.h"

#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>
#include <common/layouts/VCollisionLayout.h>

#include <AdHocEditor/AdHocEditor.h>

#include <models/component.h>
#include <models/businterface.h>
#include <models/model.h>
#include <models/modelparameter.h>
#include <models/view.h>

#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QTextDocument>

#include <QDebug>

DiagramComponent::DiagramComponent(LibraryInterface* lh_, 
								   QSharedPointer<Component> component,
                                   const QString &instanceName,
								   const QString &displayName,
                                   const QString &description,
                                   const QMap<QString, QString> &configurableElementValues,
                                   QMap<QString, bool> const& portAdHocVisibilities,
                                   QGraphicsItem *parent)
    : ComponentItem(QRectF(-GridSize * 8, 0, GridSize * 8 * 2, 40),
                    component,
                    instanceName,
                    displayName,
                    description,
                    configurableElementValues, parent),
      AdHocEnabled(),
      lh_(lh_),
      hierIcon_(0),
      oldColumn_(0),
      leftPorts_(),
      rightPorts_(),
      connUpdateDisabled_(false)
{
    setFlag(ItemIsMovable);
    setAdHocData(component, portAdHocVisibilities);
    
	QList<QSharedPointer<BusInterface> > busInterfaces
            = component->getBusInterfaces().values();

    int portSpacing = 3*GridSize;
    int portCountLeft = busInterfaces.size() / 2.0 + .5;
    setRect(-GridSize*8, 0, GridSize * 8 * 2, 
            6 * GridSize + portSpacing * std::max(portCountLeft - 1, 0));

    bool right = false;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;

    foreach (QSharedPointer<BusInterface> busif, busInterfaces)
    {
        DiagramPort *port = new DiagramPort(busif, lh_, this);

        // If this is a platform component, place master bus interfaces to the right
        // and slave bus interfaces to the left.
        if (componentModel()->getComponentImplementation() == KactusAttribute::KTS_SW &&
            componentModel()->getComponentSWType() == KactusAttribute::KTS_SW_PLATFORM)
        {
            if (busif->getInterfaceMode() == General::MASTER)
            {
                right = true;
            }
            else
            {
                right = false;
            }
        }

        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right) {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        } else {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }

    // Parse port ad-hoc visibilities.
    foreach (QSharedPointer<Port> adhocPort, componentModel()->getPorts().values())
    {
        if (!isPortAdHocVisible(adhocPort->getName()))
        {
            continue;
        }

        DiagramAdHocPort* port = new DiagramAdHocPort(adhocPort.data(), lh_, this);
        connect(port, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

        if (right)
        {
            port->setPos(QPointF(rect().width(), rightY) + rect().topLeft());
            rightY += portSpacing;
        }
        else
        {
            port->setPos(QPointF(0, leftY) + rect().topLeft());
            leftY += portSpacing;
        }

        onAddPort(port, right);
        right = !right;
    }

    updateSize();
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: ~DiagramComponent()
//-----------------------------------------------------------------------------
DiagramComponent::~DiagramComponent()
{
    // Remove all interconnections.
    foreach (QGraphicsItem *item, childItems()) {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
        foreach (DiagramInterconnection *interconn, diagramPort->getInterconnections()) {
            delete interconn;
        }
    }

    // Remove this item from the column where it resides.
    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }

	emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: getBusPort()
//-----------------------------------------------------------------------------
DiagramPort *DiagramComponent::getBusPort(const QString &name)
{
    foreach (QGraphicsItem *item, QGraphicsRectItem::children()) {
        if (item->type() == DiagramPort::Type) {
            DiagramPort *busPort = qgraphicsitem_cast<DiagramPort *>(item);
            if (busPort->name() == name)
                return busPort;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    if (static_cast<BlockDiagram*>(scene())->isProtected())
    {
        return;
    }

    // Disable connection updates so that Qt does not update the connections
    // before the possible column change has been done.
    connUpdateDisabled_ = true;

    ComponentItem::mouseMoveEvent(event);
    
    if (oldColumn_ != 0)
    {
        setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this, oldColumn_);
    }

    connUpdateDisabled_ = false;

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);

        foreach (DiagramInterconnection *interconn, diagramPort->getInterconnections())
        {
            interconn->updatePosition();
        }

        foreach (DiagramInterconnection *interconn, diagramPort->getOffPageConnector()->getInterconnections())
        {
            interconn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the interconnections.
        foreach (QGraphicsItem *item, childItems())
        {
            if (item->type() != DiagramPort::Type)
                continue;

            DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
            foreach (DiagramInterconnection *conn, diagramPort->getInterconnections())
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            column->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramComponent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<DiagramColumn*>(parentItem());

    // Begin the position update for the interconnections.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != DiagramPort::Type)
            continue;

        DiagramPort *diagramPort = qgraphicsitem_cast<DiagramPort *>(item);
        foreach (DiagramInterconnection *conn, diagramPort->getInterconnections())
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void DiagramComponent::onAddPort(DiagramConnectionEndPoint* port, bool right)
{
    if (right)
    {
        rightPorts_.append(port);
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        leftPorts_.append(port);
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
        VCollisionLayout::setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT, SPACING);
    }
}

//-----------------------------------------------------------------------------
// Function: onMovePort()
//-----------------------------------------------------------------------------
void DiagramComponent::onMovePort(DiagramConnectionEndPoint* port)
{
    // Remove the port from the stacks (this simplifies code).
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);

    // Restrict the position so that the port cannot be placed too high.
    port->setPos(snapPointToGrid(port->x(), std::max(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));
    
    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0.0)
    {
        VCollisionLayout::updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }
    else
    {
        VCollisionLayout::updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT, SPACING);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
DiagramPort* DiagramComponent::addPort(QPointF const& pos)
{
    // Determine a unique name for the bus interface.
    QString name = "bus";
    unsigned int count = 0;

    while (componentModel()->getBusInterface(name) != 0)
    {
        ++count;
        name = "bus_" + QString::number(count);
    }

    // Create an empty bus interface and add it to the component model.
    QSharedPointer<BusInterface> busIf(new BusInterface());
    busIf->setName(name);
    busIf->setInterfaceMode(General::MODE_UNDEFINED);
    componentModel()->addBusInterface(busIf);

    // Create the visualization for the bus interface.
    DiagramPort* port = new DiagramPort(busIf, lh_, this);
    port->setPos(mapFromScene(pos));
    onAddPort(port, mapFromScene(pos).x() >= 0);

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
void DiagramComponent::addPort(DiagramConnectionEndPoint* port)
{
    port->setParentItem(this);

    if (port->type() == DiagramPort::Type)
    {
        // Add the bus interface to the component.
        componentModel()->addBusInterface(port->getBusInterface());
    }

    // Make preparations.
    onAddPort(port, port->x() >= 0);

    // Update the component size.
    updateSize();
}

//-----------------------------------------------------------------------------
// Function: updateSize()
//-----------------------------------------------------------------------------
void DiagramComponent::updateSize()
{
    // Update the component's size based on the port that is positioned at
    // the lowest level of them all.
    qreal maxY = 4 * GridSize;

    if (!leftPorts_.empty())
    {
        maxY = leftPorts_.back()->y();
    }

    if (!rightPorts_.empty())
    {
        maxY = std::max(maxY, rightPorts_.back()->y());
    }

    setRect(-GridSize * 8, 0, GridSize * 8 * 2, maxY + 2 * GridSize);

    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void DiagramComponent::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (vlnv->isValid())
    {
        if (componentModel()->isBus())
            setBrush(QBrush(QColor(0xce,0xdf,0xff))); 
        else
            setBrush(QBrush(QColor(0xa5,0xc3,0xef)));
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

//-----------------------------------------------------------------------------
// Function: removePort()
//-----------------------------------------------------------------------------
void DiagramComponent::removePort(DiagramConnectionEndPoint* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    updateSize();
    
    if (port->type() == DiagramPort::Type)
    {
        componentModel()->removeBusInterface(port->getBusInterface().data());
    }
}

//-----------------------------------------------------------------------------
// Function: isConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
bool DiagramComponent::isConnectionUpdateDisabled() const
{
    return connUpdateDisabled_;
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::setPortAdHocVisible()
//-----------------------------------------------------------------------------
void DiagramComponent::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    emit contentChanged();

    // Create/destroy the ad-hoc port graphics item.
    if (visible)
    {
        Port* adhocPort = componentModel()->getPort(portName);
        Q_ASSERT(adhocPort != 0);

        DiagramAdHocPort* port = new DiagramAdHocPort(adhocPort, lh_, this);

        // Place the port at the bottom of the side that contains fewer ports.
        if (leftPorts_.size() < rightPorts_.size())
        {
            if (!leftPorts_.empty())
            {
                port->setPos(QPointF(0, leftPorts_.last()->pos().y() + GridSize * 3) + rect().topLeft());
            }
            else
            {
                port->setPos(QPointF(0, GridSize * 4) + rect().topLeft());
            }

            onAddPort(port, false);
        }
        else
        {
            if (!rightPorts_.empty())
            {
                port->setPos(QPointF(rect().width(), rightPorts_.last()->pos().y() + GridSize * 3) + rect().topLeft());
            }
            else
            {
                port->setPos(QPointF(rect().width(), GridSize * 4) + rect().topLeft());
            }

            onAddPort(port, true);
        }

        // Update the component's size after addition.
        updateSize();
    }
    else
    {
        // Search for the ad-hoc port from both sides.
        DiagramConnectionEndPoint* found = getAdHocPort(portName);
        Q_ASSERT(found != 0);

        // Remove the port and delete it.
        removePort(found);
        delete found;
        found = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::getAdHocPort()
//-----------------------------------------------------------------------------
DiagramAdHocPort* DiagramComponent::getAdHocPort(QString const& portName)
{
    foreach (DiagramConnectionEndPoint* endPoint, leftPorts_)
    {
        if (dynamic_cast<DiagramAdHocPort*>(endPoint) != 0 && endPoint->name() == portName)
        {
            return static_cast<DiagramAdHocPort*>(endPoint);
        }
    }
    
    foreach (DiagramConnectionEndPoint* endPoint, rightPorts_)
    {
        if (dynamic_cast<DiagramAdHocPort*>(endPoint) != 0 && endPoint->name() == portName)
        {
            return static_cast<DiagramAdHocPort*>(endPoint);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::attach()
//-----------------------------------------------------------------------------
void DiagramComponent::attach(AdHocEditor* editor)
{
    connect(this, SIGNAL(contentChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(destroyed(ComponentItem*)), editor, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::detach()
//-----------------------------------------------------------------------------
void DiagramComponent::detach(AdHocEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::isProtected()
//-----------------------------------------------------------------------------
bool DiagramComponent::isProtected() const
{
    return static_cast<BlockDiagram*>(scene())->isProtected();
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::getGenericEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DiagramComponent::getEditProvider()
{
    return static_cast<BlockDiagram*>(scene())->getEditProvider();
}

//-----------------------------------------------------------------------------
// Function: DiagramComponent::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
DiagramConnectionEndPoint* DiagramComponent::getDiagramAdHocPort(QString const& portName)
{
    return getAdHocPort(portName);
}
