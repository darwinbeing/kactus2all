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

#include <library/LibraryManager/libraryinterface.h>

#include <common/GenericEditProvider.h>
#include <common/KactusColors.h>
#include <common/layouts/VCollisionLayout.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>

#include <designEditors/common/diagramgrid.h>
#include <designEditors/HWDesign/AdHocEditor/AdHocEditor.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/view.h>
#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <QGraphicsDropShadowEffect>
#include <QFont>
#include <QTextDocument>

#include <QDebug>

HWComponentItem::HWComponentItem(LibraryInterface* lh_, 
								 QSharedPointer<Component> component,
                                 const QString &instanceName,
								 const QString &displayName,
                                 const QString &description,
                                 const QString& uuid,
                                 const QMap<QString, QString> &configurableElementValues,
                                 QMap<QString, bool> const& portAdHocVisibilities,
                                 QGraphicsItem *parent)
    : ComponentItem(QRectF(-COMPONENTWIDTH/ 2, 0, COMPONENTWIDTH, 40),
                    lh_,
                    component,
                    instanceName,
                    displayName,
                    description,
						  uuid,
                    configurableElementValues, parent),
      AdHocEnabled(),
      hierIcon_(0),
      oldColumn_(0),
      portLayout_(new VCollisionLayout<HWConnectionEndpoint>(SPACING)),
      leftPorts_(),
      rightPorts_(),
      connUpdateDisabled_(false),
      vendorExtensions_()
{
    setFlag(ItemIsMovable);
    setAdHocData(component, portAdHocVisibilities);
    
	QList<QSharedPointer<BusInterface> > busInterfaces = component->getBusInterfaces();

    int portSpacing = 3*GridSize;
    int portCountLeft = busInterfaces.size() / 2.0 + .5;
    setRect(-COMPONENTWIDTH/ 2, 0, COMPONENTWIDTH, 
            6 * GridSize + portSpacing * qMax(portCountLeft - 1, 0));

    bool right = false;
    int leftY = 4 * GridSize;
    int rightY = 4 * GridSize;
	
    foreach (QSharedPointer<BusInterface> busif, busInterfaces)
    {
        BusPortItem *port = new BusPortItem(busif, getLibraryInterface(), true, this);

        // Check if the default position has been specified.
        if (!busif->getDefaultPos().isNull())
        {
            port->setPos(busif->getDefaultPos());
            onAddPort(port, port->pos().x() >= 0);
        }
        else
        {
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
    }

    // Parse port ad-hoc visibilities.
    foreach (QSharedPointer<Port> adhocPort, componentModel()->getPorts())
    {
        if (!isPortAdHocVisible(adhocPort->getName()))
        {
            continue;
        }

        AdHocPortItem* port = new AdHocPortItem(adhocPort.data(), getLibraryInterface(), this);

        // Check if the default position has been specified.
        if (!adhocPort->getDefaultPos().isNull())
        {
            port->setPos(adhocPort->getDefaultPos());
            onAddPort(port, port->pos().x() >= 0);
        }
        else
        {
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
    foreach (QGraphicsItem* item, QGraphicsRectItem::childItems()) {
        if (item->type() == BusPortItem::Type)
        {
            BusPortItem* busPort = qgraphicsitem_cast<BusPortItem*>(item);

            if (busPort->name() == name)
            {
                return busPort;
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: getBusPort()
//-----------------------------------------------------------------------------
BusPortItem const* HWComponentItem::getBusPort(const QString &name) const
{
    foreach (QGraphicsItem const* item, QGraphicsRectItem::childItems())
    {
        if (item->type() == BusPortItem::Type)
        {
            BusPortItem const* busPort = qgraphicsitem_cast<BusPortItem const*>(item);

            if (busPort->name() == name)
            {
                return busPort;
            }
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
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0 || diagram->isProtected())
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
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    ComponentItem::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        HWColumn* column = dynamic_cast<HWColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        QSharedPointer<QUndoCommand> cmd;

        if (scenePos() != oldPos_)
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_, oldColumn_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // End the position update for all connections.
        foreach (QGraphicsItem *item, scene()->items())
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

            if (conn != 0)
            {
                conn->endUpdatePosition(cmd.data());
            }
        }

        // Add the undo command to the edit stack only if it has at least some real changes.
        if (cmd->childCount() > 0 || scenePos() != oldPos_)
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd);
        }

        oldColumn_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void HWComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DesignDiagram* diagram = dynamic_cast<DesignDiagram*>(scene());

    if (diagram == 0)
    {
        return;
    }

    ComponentItem::mousePressEvent(event);
    setZValue(1001.0);

    oldPos_ = scenePos();
    oldColumn_ = dynamic_cast<HWColumn*>(parentItem());

    // Begin the position update for all connections.
    foreach (QGraphicsItem *item, scene()->items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0)
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
    connect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

    if (right)
    {
        rightPorts_.append(port);
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
        portLayout_->setItemPos(rightPorts_, port, rect().right(), MIN_Y_PLACEMENT);
		checkPortLabelSize(port, leftPorts_);
    }
    else
    {
        leftPorts_.append(port);
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);		
		portLayout_->setItemPos(leftPorts_, port, rect().left(), MIN_Y_PLACEMENT);
		checkPortLabelSize(port, rightPorts_);
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
    port->setPos(snapPointToGrid(port->x(), qMax(MIN_Y_PLACEMENT - port->boundingRect().top(), port->y())));
    
    // Check on which side the port is to determine the stack to which it should be placed.
    if (port->x() < 0.0)
    {
        portLayout_->updateItemMove(leftPorts_, port, MIN_Y_PLACEMENT);
		checkPortLabelSize(port, rightPorts_);
    }
    else
    {
        portLayout_->updateItemMove(rightPorts_, port, MIN_Y_PLACEMENT);
		checkPortLabelSize(port, leftPorts_);
    }

    updateSize();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::checkPortLabelSize()
//-----------------------------------------------------------------------------
void HWComponentItem::checkPortLabelSize( HWConnectionEndpoint* port, QList<HWConnectionEndpoint*> otherSide )
{
	for ( int i = 0; i < otherSide.size(); ++i)
	{ 
		if (port->y() == otherSide.at(i)->y())
		{
			qreal portLabelWidth = port->getNameLength();
			qreal otherLabelWidth = otherSide.at(i)->getNameLength();

			// Check if both of the labels exceed the mid section of the component.
		    if (portLabelWidth + SPACING * 2 > (ComponentItem::COMPONENTWIDTH / 2 ) &&
				otherLabelWidth + SPACING * 2 > (ComponentItem::COMPONENTWIDTH) / 2)
		    {
				port->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
				otherSide.at(i)->shortenNameLabel( ComponentItem::COMPONENTWIDTH / 2 );
		    }
				
			// Check if the other port is wider than the other.
		    else if (portLabelWidth > otherLabelWidth )
		    {
				port->shortenNameLabel( ComponentItem::COMPONENTWIDTH - otherLabelWidth - SPACING * 2 );
		    }

		    else
		    {
				otherSide.at(i)->shortenNameLabel( ComponentItem::COMPONENTWIDTH - portLabelWidth - SPACING * 2 );
		    }				
			
			return;
		} 
	}

	// If the port gets here, there is no ports with the same y() value, and so the port name is restored.
	port->shortenNameLabel( ComponentItem::COMPONENTWIDTH );
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
// Function: HWComponentItem::getHeight()
//-----------------------------------------------------------------------------
qreal HWComponentItem::getHeight()
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
		maxY = qMax(maxY, rightPorts_.back()->y());
	}

	return (maxY + BOTTOM_MARGIN);
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getWidth()
//-----------------------------------------------------------------------------
qreal HWComponentItem::getWidth()
{
	return COMPONENTWIDTH;
}

//-----------------------------------------------------------------------------
// Function: updateComponent()
//-----------------------------------------------------------------------------
void HWComponentItem::updateComponent()
{
    ComponentItem::updateComponent();

    VLNV* vlnv = componentModel()->getVlnv();

    // Check whether the component is packaged (valid vlnv) or not.
    if (isDraft())
    {
        setBrush(QBrush(KactusColors::DRAFT_COMPONENT));
    }
    else if (getLibraryInterface()->contains(*vlnv))
    {
        if (componentModel()->isBus())
        {
            setBrush(QBrush(KactusColors::HW_BUS_COMPONENT));
        }
        else
        {
            setBrush(QBrush(KactusColors::HW_COMPONENT));
        }
    }
    else
    {
        setBrush(QBrush(KactusColors::MISSING_COMPONENT));
    }

    // Create a hierarchy icon if the component is a hierarchical one.
    if (componentModel()->getModel()->hasHierView())
    {
        if (hierIcon_ == 0)
        {
            hierIcon_ = new QGraphicsPixmapItem(QPixmap(":icons/common/graphics/hierarchy.png"), this);
            hierIcon_->setToolTip(tr("Hierarchical"));
            hierIcon_->setPos(COMPONENTWIDTH/2 - hierIcon_->pixmap().width() - SPACING, SPACING);
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
    disconnect(port, SIGNAL(moved(ConnectionEndpoint*)), this, SIGNAL(endpointMoved(ConnectionEndpoint*)));

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
// Function: HWComponentItem::getAdHocPort()
//-----------------------------------------------------------------------------
AdHocPortItem const* HWComponentItem::getAdHocPort(QString const& portName) const
{
    foreach (HWConnectionEndpoint const* endpoint, leftPorts_)
    {
        if (dynamic_cast<AdHocPortItem const*>(endpoint) != 0 && endpoint->name() == portName)
        {
            return static_cast<AdHocPortItem const*>(endpoint);
        }
    }

    foreach (HWConnectionEndpoint const* endpoint, rightPorts_)
    {
        if (dynamic_cast<AdHocPortItem const*>(endpoint) != 0 && endpoint->name() == portName)
        {
            return static_cast<AdHocPortItem const*>(endpoint);
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

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setBusInterfacePositions()
//-----------------------------------------------------------------------------
void HWComponentItem::setBusInterfacePositions(QMap<QString, QPointF> const& positions, bool createMissing)
{
    QMapIterator<QString, QPointF> itrPortPos(positions);

    while (itrPortPos.hasNext())
    {
        itrPortPos.next();
        BusPortItem* port = getBusPort(itrPortPos.key());

        if (port == 0)
        {
            if (!createMissing || componentModel()->getVlnv()->isValid())
            {
                continue;
            }

            port = addPort(itrPortPos.value());
            port->setName(itrPortPos.key());
        }

        port->setPos(itrPortPos.value());
        onMovePort(port);
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setAdHocPortPositions()
//-----------------------------------------------------------------------------
void HWComponentItem::setAdHocPortPositions(QMap<QString, QPointF> const& positions)
{
    QMapIterator<QString, QPointF> itrPortPos(positions);

    while (itrPortPos.hasNext())
    {
        itrPortPos.next();
        AdHocPortItem* port = getAdHocPort(itrPortPos.key());

        if (port != 0)
        {
            port->setPos(itrPortPos.value());
            onMovePort(port);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setVendorExtensions()
//-----------------------------------------------------------------------------
void HWComponentItem::setVendorExtensions(QList<QSharedPointer<VendorExtension> > const& vendorExtensions)
{
    vendorExtensions_ = vendorExtensions;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getBusInterfacePositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> HWComponentItem::getBusInterfacePositions() const
{
    QMap<QString, QPointF> positions;
    QListIterator<QSharedPointer<BusInterface> > itrBusIf(componentModel()->getBusInterfaces());

    while (itrBusIf.hasNext())
    {
        QSharedPointer<BusInterface> busif = itrBusIf.next();
        positions[busif->getName()] = getBusPort(busif->getName())->pos();
    }

    return positions;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> HWComponentItem::getAdHocPortPositions() const
{
    QMap<QString, QPointF> positions;
    QMapIterator<QString, bool> itrAdHoc(getPortAdHocVisibilities());

    while (itrAdHoc.hasNext())
    {
        itrAdHoc.next();

        if (itrAdHoc.value())
        {
            positions[itrAdHoc.key()] = getAdHocPort(itrAdHoc.key())->pos();
        }
    }

    return positions;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::getVendorExtensions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > HWComponentItem::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setPacketized()
//-----------------------------------------------------------------------------
void HWComponentItem::setPacketized()
{
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
    {
        if (extension->type() == "kactus2:draft")
        {
            vendorExtensions_.removeAll(extension);
        }
    }
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::setDraft()
//-----------------------------------------------------------------------------
void HWComponentItem::setDraft()
{
    if (!isDraft())
    {
        vendorExtensions_.append(QSharedPointer<Kactus2Placeholder>(new Kactus2Placeholder("kactus2:draft")));
    }
    updateComponent();
}

//-----------------------------------------------------------------------------
// Function: HWComponentItem::isDraft()
//-----------------------------------------------------------------------------
bool HWComponentItem::isDraft()
{
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
    {
        if (extension->type() == "kactus2:draft")
        {
            return true;
        }
    }
    return false;
}
