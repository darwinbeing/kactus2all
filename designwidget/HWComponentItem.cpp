/* 
 *
 * 		filename: diagramcomponent.cpp
 */

#include "HWComponentItem.h"
#include "BusPortItem.h"
#include "AdHocPortItem.h"
#include "HWConnection.h"
#include "HWDesignDiagram.h"
#include "HWMoveCommands.h"

#include "columnview/HWColumn.h"

#include <LibraryManager/libraryinterface.h>

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

HWComponentItem::HWComponentItem(LibraryInterface* lh_, 
								   QSharedPointer<Component> component,
                                   const QString &instanceName,
								   const QString &displayName,
                                   const QString &description,
                                   const QMap<QString, QString> &configurableElementValues,
                                   QMap<QString, bool> const& portAdHocVisibilities,
                                   QGraphicsItem *parent)
    : ComponentItem(QRectF(-GridSize * 8, 0, GridSize * 8 * 2, 40),
                    lh_,
                    component,
                    instanceName,
                    displayName,
                    description,
                    configurableElementValues, parent),
      AdHocEnabled(),
      hierIcon_(0),
      oldColumn_(0),
      leftPorts_(),
      rightPorts_(),
      connUpdateDisabled_(false)
{
    setFlag(ItemIsMovable);
    setAdHocData(component, portAdHocVisibilities);
    
	QList<QSharedPointer<BusInterface> > busInterfaces = component->getBusInterfaces();

    int portSpacing = 3*GridSize;
    int portCountLeft = busInterfaces.size() / 2.0 + .5;
    setRect(-GridSize*8, 0, GridSize * 8 * 2, 
            6 * GridSize + portSpacing * std::max(portCountLeft - 1, 0));

    bool right = false;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;

    foreach (QSharedPointer<BusInterface> busif, busInterfaces)
    {
        BusPortItem *port = new BusPortItem(busif, getLibraryInterface(), true, this);

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
    foreach (QSharedPointer<Port> adhocPort, componentModel()->getPorts())
    {
        if (!isPortAdHocVisible(adhocPort->getName()))
        {
            continue;
        }

        AdHocPortItem* port = new AdHocPortItem(adhocPort.data(), getLibraryInterface(), this);

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
// Function: ~HWComponentItem()
//-----------------------------------------------------------------------------
HWComponentItem::~HWComponentItem()
{
    // Remove all interconnections.
    foreach (QGraphicsItem *item, childItems()) {
        if (item->type() != BusPortItem::Type)
            continue;

        BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(item);
        foreach (GraphicsConnection *interconn, diagramPort->getConnections()) {
            delete interconn;
        }
    }

    // Remove this item from the column where it resides.
    HWColumn* column = dynamic_cast<HWColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

//-----------------------------------------------------------------------------
// Function: getBusPort()
//-----------------------------------------------------------------------------
BusPortItem *HWComponentItem::getBusPort(const QString &name)
{
    foreach (QGraphicsItem *item, QGraphicsRectItem::children()) {
        if (item->type() == BusPortItem::Type) {
            BusPortItem *busPort = qgraphicsitem_cast<BusPortItem *>(item);
            if (busPort->name() == name)
                return busPort;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard movement if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
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

        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onMoveItem(this);
    }

    connUpdateDisabled_ = false;

    // Update the port connections manually.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != BusPortItem::Type)
            continue;

        BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(item);

        foreach (GraphicsConnection *interconn, diagramPort->getConnections())
        {
            interconn->updatePosition();
        }

        foreach (GraphicsConnection *interconn, diagramPort->getOffPageConnector()->getConnections())
        {
            interconn->updatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldColumn_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for the interconnections.
        foreach (QGraphicsItem *item, childItems())
        {
            if (item->type() != BusPortItem::Type)
                continue;

            BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(item);
            foreach (GraphicsConnection *conn, diagramPort->getConnections())
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());

    // Begin the position update for the interconnections.
    foreach (QGraphicsItem *item, childItems())
    {
        if (item->type() != BusPortItem::Type)
            continue;

        BusPortItem *diagramPort = qgraphicsitem_cast<BusPortItem *>(item);
        foreach (GraphicsConnection *conn, diagramPort->getConnections())
        {
            conn->beginUpdatePosition();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onAddPort()
//-----------------------------------------------------------------------------
void HWComponentItem::onAddPort(HWConnectionEndpoint* port, bool right)
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
void HWComponentItem::onMovePort(HWConnectionEndpoint* port)
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
BusPortItem* HWComponentItem::addPort(QPointF const& pos)
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
    busIf->setInterfaceMode(General::INTERFACE_MODE_COUNT);
    componentModel()->addBusInterface(busIf);

    // Create the visualization for the bus interface.
    BusPortItem* port = new BusPortItem(busIf, getLibraryInterface(), false, this);

    port->setPos(mapFromScene(pos));
    onAddPort(port, mapFromScene(pos).x() >= 0);

    // Update the component size.
    updateSize();
    return port;
}

//-----------------------------------------------------------------------------
// Function: addPort()
//-----------------------------------------------------------------------------
void HWComponentItem::addPort(HWConnectionEndpoint* port)
{
    port->setParentItem(this);

    if (port->type() == BusPortItem::Type)
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
void HWComponentItem::updateSize()
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

    HWColumn* column = dynamic_cast<HWColumn*>(parentItem());

    if (column != 0)
    {
        column->updateItemPositions();
    }
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void HWComponentItem::updateComponent()
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
        else if (componentModel()->isBus())
        {
            setBrush(QBrush(QColor(0xce,0xdf,0xff))); 
        }
        else
        {
            setBrush(QBrush(QColor(0xa5,0xc3,0xef)));
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

//-----------------------------------------------------------------------------
// Function: removePort()
//-----------------------------------------------------------------------------
void HWComponentItem::removePort(HWConnectionEndpoint* port)
{
    leftPorts_.removeAll(port);
    rightPorts_.removeAll(port);
    updateSize();
    
    if (port->type() == BusPortItem::Type)
    {
        componentModel()->removeBusInterface(port->getBusInterface().data());
    }
}

//-----------------------------------------------------------------------------
// Function: isConnectionUpdateDisabled()
//-----------------------------------------------------------------------------
bool HWComponentItem::isConnectionUpdateDisabled() const
{
    return connUpdateDisabled_;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setPortAdHocVisible()
//-----------------------------------------------------------------------------
void HWComponentItem::onAdHocVisibilityChanged(QString const& portName, bool visible)
{
    // Create/destroy the ad-hoc port graphics item.
    if (visible)
    {
        QSharedPointer<Port> adhocPort = componentModel()->getPort(portName);
        Q_ASSERT(adhocPort != 0);

        AdHocPortItem* port = new AdHocPortItem(adhocPort.data(), getLibraryInterface(), this);

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
        HWConnectionEndpoint* found = getAdHocPort(portName);
        Q_ASSERT(found != 0);

        // Remove the port and delete it.
        removePort(found);
        delete found;
        found = 0;
    }

    emit adHocVisibilitiesChanged();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getAdHocPort()
//-----------------------------------------------------------------------------
AdHocPortItem* HWComponentItem::getAdHocPort(QString const& portName)
{
    foreach (HWConnectionEndpoint* endpoint, leftPorts_)
    {
        if (dynamic_cast<AdHocPortItem*>(endpoint) != 0 && endpoint->name() == portName)
        {
            return static_cast<AdHocPortItem*>(endpoint);
        }
    }
    
    foreach (HWConnectionEndpoint* endpoint, rightPorts_)
    {
        if (dynamic_cast<AdHocPortItem*>(endpoint) != 0 && endpoint->name() == portName)
        {
            return static_cast<AdHocPortItem*>(endpoint);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::attach()
//-----------------------------------------------------------------------------
void HWComponentItem::attach(AdHocEditor* editor)
{
    connect(this, SIGNAL(adHocVisibilitiesChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(destroyed(ComponentItem*)), editor, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::detach()
//-----------------------------------------------------------------------------
void HWComponentItem::detach(AdHocEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::isProtected()
//-----------------------------------------------------------------------------
bool HWComponentItem::isProtected() const
{
    return static_cast<HWDesignDiagram*>(scene())->isProtected();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getGenericEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& HWComponentItem::getEditProvider()
{
    return static_cast<HWDesignDiagram*>(scene())->getEditProvider();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getDiagramAdHocPort()
//-----------------------------------------------------------------------------
HWConnectionEndpoint* HWComponentItem::getDiagramAdHocPort(QString const& portName)
{
    return getAdHocPort(portName);
}
