//-----------------------------------------------------------------------------
// File: diagraminterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 17.3.2011
//
// Description:
// Diagram interface graphics item.
//-----------------------------------------------------------------------------

#include "diagraminterface.h"

#include "diagramcomponent.h"
#include "diagraminterconnection.h"
#include "DiagramOffPageConnector.h"
#include "DiagramAdHocInterface.h"
#include "PortmapDialog.h"
#include "BusInterfaceDialog.h"
#include "DiagramMoveCommands.h"
#include "blockdiagram.h"

#include "columnview/DiagramColumn.h"

#include <models/businterface.h>
#include <models/component.h>
#include <models/port.h>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/GenericEditProvider.h>
#include <common/diagramgrid.h>

#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QColor>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QGraphicsScene>
#include <QPushButton>

//-----------------------------------------------------------------------------
// Function: DiagramInterface()
//-----------------------------------------------------------------------------
DiagramInterface::DiagramInterface(LibraryInterface* lh, QSharedPointer<Component> component,
                                   QSharedPointer<BusInterface> busIf,
                                   QGraphicsItem *parent)
    : DiagramConnectionEndpoint(parent, QVector2D(1.0f, 0.0f)),
      lh_(lh), nameLabel_(0), busInterface_(), component_(component), oldColumn_(0),
      temp_(busIf == 0), oldPos_(), oldInterfacePositions_(),
      offPageConnector_(0)
{
    setType(ENDPOINT_TYPE_BUS);
    busInterface_ = busIf;
    int squareSize = GridSize;

    // The shape is of form:
    // /\
    // ||
    QPolygonF shape;
    shape << QPointF(-squareSize/2, squareSize / 2)
          << QPointF(-squareSize/2, -squareSize)
          << QPointF(squareSize/2, -squareSize)
          << QPointF(squareSize/2, squareSize / 2)
          << QPointF(0, squareSize);
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
    offPageConnector_ = new DiagramOffPageConnector(this);
    offPageConnector_->setPos(0.0, -GridSize * 3);
    offPageConnector_->setFlag(ItemStacksBehindParent);
    offPageConnector_->setVisible(false);

    updateInterface();
}

DiagramInterface::~DiagramInterface() {
    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());

    if (column != 0)
    {
        column->removeItem(this);
    }
}

QString DiagramInterface::name() const
{
    return busInterface_->getName();
}

void DiagramInterface::setName( const QString& name ) {
	busInterface_->setName(name);

    updateInterface();
}

QSharedPointer<BusInterface> DiagramInterface::getBusInterface() const
{
    return busInterface_;
}

void DiagramInterface::updateInterface()
{
	Q_ASSERT(busInterface_);

    switch (busInterface_->getInterfaceMode()) {
    case General::MASTER:
        setBrush(QBrush(QColor(0x32,0xcb,0xcb)));
        break;
    case General::SLAVE:
        setBrush(QBrush(QColor(0x32,0x99,0x64)));
        break;
    case General::MIRROREDMASTER:
        setBrush(QBrush(QColor(0xcb,0xfd,0xfd)));
        break;
    case General::MIRROREDSLAVE:
        setBrush(QBrush(QColor(0x00,0xfd,00)));
        break;
    case General::SYSTEM:
        setBrush(QBrush(QColor(0xf9,0x11,0x11)));
        break;
    case General::MIRROREDSYSTEM:
        setBrush(QBrush(QColor(0xf9,0x9d,0xcb)));
        break;
    case General::MONITOR:
        setBrush(QBrush(QColor(0xfd,0xfd,0xfd)));
        break;
	// if undefined
	default:
		setBrush(QBrush(Qt::black));
		break;
    }

    nameLabel_->setHtml("<div style=\"background-color:#eeeeee; padding:10px 10px;\">"
                        + busInterface_->getName() + "</div>");

    qreal nameWidth = nameLabel_->boundingRect().width();
    
    // Check if the port is directed to the left.
    if (getDirection().x() < 0)
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the port is directed to the right.
    else
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }

    offPageConnector_->updateInterface();
    emit contentChanged();
}

bool DiagramInterface::isHierarchical() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: onConnect()
//-----------------------------------------------------------------------------
bool DiagramInterface::onConnect(ConnectionEndpoint const* other)
{
    // Update the name if the bus interface is defined but its name is empty.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT && busInterface_->getName() == "")
    {
        busInterface_->setName(other->getBusInterface()->getName());
        updateInterface();
    }

    // If the bus interface is already defined or the other endpoint
    // does not have a valid bus interface (unpackaged), we don't have to do anything.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT ||
        !other->getBusInterface()->getBusType().isValid())
    {
        return true;
    }

    // Determine the name for the interface.
    QString newBusIfName = other->getBusInterface()->getName();
    unsigned int index = 0;

    while (component_->getBusInterface(newBusIfName) != 0)
    {
        ++index;
        newBusIfName = other->getBusInterface()->getName() + "_" + QString::number(index);
    }

    // Create a new bus interface for the diagram interface.
    QSharedPointer<BusInterface> newBusIf(new BusInterface());

    newBusIf->setName(newBusIfName);
    newBusIf->setName(newBusIfName);
    newBusIf->setInterfaceMode(other->getBusInterface()->getInterfaceMode());
    newBusIf->setBusType(other->getBusInterface()->getBusType());
    newBusIf->setAbstractionType(other->getBusInterface()->getAbstractionType());

    if (!static_cast<DesignDiagram*>(scene())->getEditProvider().isPerformingUndoRedo())
    {
        // Ask the user if he wants to auto-copy the port maps from the other end point.
        QMessageBox msgBox(QMessageBox::Question, QCoreApplication::applicationName(),
                           tr("Do you want to auto-copy the port maps from the port or "
                              "perform manual setup instead?"), QMessageBox::NoButton,
                           (QWidget*)scene()->parent());
        QPushButton* btnCopy = msgBox.addButton(tr("Copy"), QMessageBox::YesRole);
        QPushButton* btnManual = msgBox.addButton(tr("Manual Setup"), QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == btnCopy)
        {
            // The user pressed copy, so duplicate all physical ports from the other end point 
            // and create the new port maps at the same time.
            Q_ASSERT(other->encompassingComp() != 0);
            QSharedPointer<Component> otherComp = other->encompassingComp()->componentModel();

            QList< QSharedPointer<General::PortMap> > const& portMaps = other->getBusInterface()->getPortMaps();

            // Duplicate the ports referenced by the port maps and duplicate the port maps.
            // Translation table keeps track of the mapped port names.
            QMap<QString, QString> nameMappings;
            QList< QSharedPointer<General::PortMap> > newPortMaps;

            foreach (QSharedPointer<General::PortMap> portMap, portMaps)
            {
                // Duplicate the port if it hasn't already been duplicated.
                if (!nameMappings.contains(portMap->physicalPort_))
                {
                    // Retrieve the port from the model.
                    Port* port = otherComp->getPort(portMap->physicalPort_);

                    // Check if the port was not found.
                    if (port == 0)
                    {
                        emit errorMessage(tr("Port %1 was not found in component %2 while "
                                             "copying the bus interface.").arg(portMap->physicalPort_,
                                                                               other->encompassingComp()->name()));
                        return false;
                    }

                    // The new port inherits the same name as the physical port in the other component.
                    QString newPortName = portMap->physicalPort_;
                    unsigned int runningNumber = 0;

                    // Make sure that the port name is unique.
                    while (component_->getPort(newPortName) != 0)
                    {
                        ++runningNumber;
                        newPortName = portMap->physicalPort_ + "_" + QString::number(runningNumber);
                    }

                    // Create the port and add it to the component.
                    QSharedPointer<Port> newPort(new Port(newPortName, *port));
                    component_->addPort(newPort);

                    // Update the translation table.
                    nameMappings.insert(portMap->physicalPort_, newPortName);
                }

                // Duplicate and add the port map to the bus interface.
                QSharedPointer<General::PortMap> newPortMap(new General::PortMap(*portMap));
                newPortMap->physicalPort_ = nameMappings.value(portMap->physicalPort_);
                newPortMaps.append(newPortMap);
            }

            newBusIf->setPortMaps(newPortMaps);
        }
        else if (msgBox.clickedButton() == btnManual)
        {
            // Otherwise open the port map tool.
            PortmapDialog dialog(lh_, component_, newBusIf.data(), other->getBusInterface().data(),
                                 (QWidget*)scene()->parent());

            // If the user canceled the dialog, do not make any connection.
            if (dialog.exec() == QDialog::Rejected)
            {
                return false;
            }
        }
    }

    busInterface_ = newBusIf;
    component_->addBusInterface(newBusIf);

    // Update the interface visuals.
    updateInterface();

    return true;
}

//-----------------------------------------------------------------------------
// Function: onDisonnect()
//-----------------------------------------------------------------------------
void DiagramInterface::onDisconnect(ConnectionEndpoint const*)
{
    // Check if there is still some connections left, the bus interface is not defined
    // or the interface is not temporary.
    if (!getConnections().empty() ||
        busInterface_->getInterfaceMode() == General::INTERFACE_MODE_COUNT || !temp_)
    {
        // Don't do anything.
        return;
    }

    // Otherwise undefine the interface.
    //undefine();

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: canConnect()
//-----------------------------------------------------------------------------
bool DiagramInterface::canConnect(ConnectionEndpoint const* other) const
{
    // This end point requires a bus interface connection.
    if (!other->isBus())
    {
        return false;
    }

    // Two hierarchical end points cannot be connected together.
    if (other->isHierarchical())
    {
        return false;
    }

    QSharedPointer<BusInterface> otherBusIf = other->getBusInterface();

    if (otherBusIf == 0)
    {
        return false;
    }

    // Connection is allowed if this interface's bus interface is not defined,
    // the other end point is unpackaged, or the abstraction definitions of the bus interfaces
    // in each end point are equal.
    return (busInterface_->getInterfaceMode() == General::INTERFACE_MODE_COUNT ||
            !otherBusIf->getBusType().isValid() ||
            (otherBusIf->getAbstractionType() == busInterface_->getAbstractionType() &&
             otherBusIf->getBusType() == busInterface_->getBusType()));
}

ComponentItem* DiagramInterface::encompassingComp() const
{
    return 0;
}

QSharedPointer<Component> DiagramInterface::getOwnerComponent() const {
	Q_ASSERT(component_);
	return component_;
}

QVariant DiagramInterface::itemChange(GraphicsItemChange change,
                                 const QVariant &value)
{
    switch (change) {

    case ItemPositionChange:
        {
            return snapPointToGrid(value.toPointF());
        }

    case ItemPositionHasChanged:
        {
            if (!parentItem())
                break;

        emit contentChanged();
        break;
    }

    case ItemScenePositionHasChanged:
        
        foreach (GraphicsConnection *interconnection, getConnections()) {
            interconnection->updatePosition();
        }

        break;

    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void DiagramInterface::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    // Discard mouse move if the diagram is protected.
    if (static_cast<DesignDiagram*>(scene())->isProtected())
    {
        return;
    }

    DiagramConnectionEndpoint::mouseMoveEvent(event);

    setPos(parentItem()->mapFromScene(oldColumn_->mapToScene(pos())));

    DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
    Q_ASSERT(column != 0);
    column->onMoveItem(this);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void DiagramInterface::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    DiagramConnectionEndpoint::mouseReleaseEvent(event);
    setZValue(0.0);

    if (oldColumn_ != 0)
    {
        DiagramColumn* column = dynamic_cast<DiagramColumn*>(parentItem());
        Q_ASSERT(column != 0);
        column->onReleaseItem(this);

        oldColumn_ = 0;

        QSharedPointer<QUndoCommand> cmd;

        // Check if the interface position was really changed.
        if (oldPos_ != scenePos())
        {
            cmd = QSharedPointer<QUndoCommand>(new ItemMoveCommand(this, oldPos_));
        }
        else
        {
            cmd = QSharedPointer<QUndoCommand>(new QUndoCommand());
        }

        // Determine if the other interfaces changed their position and create undo commands for them.
        QMap<QGraphicsItem*, QPointF>::iterator cur = oldInterfacePositions_.begin();

        while (cur != oldInterfacePositions_.end())
        {
            if (cur.key()->scenePos() != cur.value())
            {
                QUndoCommand* childCmd = new ItemMoveCommand(cur.key(), cur.value(), cmd.data());
            }

            ++cur;
        }

        oldInterfacePositions_.clear();

        // End the position update of the connections.
        foreach (GraphicsConnection* conn, getConnections())
        {
            conn->endUpdatePosition(cmd.data());
        }

        // Add the undo command to the edit stack only if it has changes.
        if (cmd->childCount() > 0 || oldPos_ != scenePos())
        {
            static_cast<DesignDiagram*>(scene())->getEditProvider().addCommand(cmd, false);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void DiagramInterface::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    DiagramConnectionEndpoint::mousePressEvent(event);
    setZValue(1001.0);

    oldColumn_ = dynamic_cast<DiagramColumn*>(parentItem());
    oldPos_ = scenePos();
    Q_ASSERT(oldColumn_ != 0);

    // Save the positions of the other diagram/ad-hoc interfaces.
    foreach (GraphicsColumn* column, oldColumn_->getLayout().getColumns())
    {
        if (column->getContentType() == COLUMN_CONTENT_IO)
        {
            foreach (QGraphicsItem* item, column->childItems())
            {
                if (item->type() == DiagramInterface::Type ||
                    item->type() == DiagramAdHocInterface::Type)
                {
                    oldInterfacePositions_.insert(item, item->scenePos());
                }
            }
        }
    }

    // Begin the position update of the connections.
    foreach (GraphicsConnection* conn, getConnections())
    {
        conn->beginUpdatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: setDirection()
//-----------------------------------------------------------------------------
void DiagramInterface::setDirection(QVector2D const& dir)
{
    DiagramConnectionEndpoint::setDirection(dir);

    // Update the position of the name label based on the direction.
    qreal nameWidth = nameLabel_->boundingRect().width();
    
    // Check if the interface is directed to the left.
    if (dir.x() < 0)
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 - nameWidth / 2.0);
    }
    // Otherwise the interface is directed to the right.
    else
    {
        nameLabel_->setPos(0, GridSize * 3.0 / 4.0 + nameWidth / 2.0);
    }
}

//-----------------------------------------------------------------------------
// Function: setTemporary()
//-----------------------------------------------------------------------------
void DiagramInterface::setTemporary(bool temp)
{
    temp_ = temp;
}

//-----------------------------------------------------------------------------
// Function: setTypes()
//-----------------------------------------------------------------------------
void DiagramInterface::setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode)
{
    // Destroy the old bus interface if it exists.
    if (busInterface_->getInterfaceMode() != General::INTERFACE_MODE_COUNT)
    {
        // Disconnect the connections.
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->onDisconnect(this);
            }
            else
            {
                conn->endpoint2()->onDisconnect(this);
            }
        }

        busInterface_->setInterfaceMode(General::INTERFACE_MODE_COUNT);
    }

    if (busType.isValid())
    {
        busInterface_->setBusType(busType);
        busInterface_->setAbstractionType(absType);
        busInterface_->setInterfaceMode(mode);

        setTemporary(false);

        // Update the interface visuals.
        updateInterface();

        // Undefined end points of the connections can now be defined.
        foreach(GraphicsConnection* conn, getConnections())
        {
            if (conn->endpoint1() != this)
            {
                conn->endpoint1()->onConnect(this);
                conn->endpoint2()->onConnect(conn->endpoint1());
            }
            else
            {
                conn->endpoint2()->onConnect(this);
                conn->endpoint1()->onConnect(conn->endpoint2());
            }
        }
    }
    else
    {
        setTemporary(true);

        // Update the interface visuals.
        updateInterface();
    }
}

//-----------------------------------------------------------------------------
// Function: define()
//-----------------------------------------------------------------------------
void DiagramInterface::define(QSharedPointer<BusInterface> busIf, bool addPorts,
                              QList< QSharedPointer<Port> > ports)
{
    // Add the ports to the top-level component.
    foreach (QSharedPointer<Port> port, ports)
    {
        component_->addPort(port);
    }

    // Add the bus interface to the component.
    component_->addBusInterface(busIf);
    busInterface_ = busIf;

    // Update the interface visuals.
    updateInterface();
}

//-----------------------------------------------------------------------------
// Function: undefine()
//-----------------------------------------------------------------------------
void DiagramInterface::undefine(bool removePorts)
{
    if (removePorts)
    {
        // Delete all ports related to the bus interface from the top-level component.
        QList< QSharedPointer<General::PortMap> > const& portMaps = busInterface_->getPortMaps();

        for (int i = 0; i < portMaps.size(); ++i)
        {
            component_->removePort(portMaps.at(i)->physicalPort_);
        }
    }

    // Remove the bus interface from the top-level component and destroy it.
    component_->removeBusInterface(busInterface_->getName());
    busInterface_.clear();
}

//-----------------------------------------------------------------------------
// Function: getPorts()
//-----------------------------------------------------------------------------
QList<Port*> DiagramInterface::getPorts() const
{
    Q_ASSERT(busInterface_ != 0);
    QList<Port*> ports;

    QList< QSharedPointer<General::PortMap> > const& portMaps = busInterface_->getPortMaps();

    foreach (QSharedPointer<General::PortMap> portMap, portMaps)
    {
        Port* port = component_->getPort(portMap->physicalPort_);

        if (port != 0)
        {
            ports.append(port);
        }
    }

    return ports;
}

void DiagramInterface::setInterfaceMode( General::InterfaceMode mode ) {
	Q_ASSERT(busInterface_);
	busInterface_->setInterfaceMode(mode);
	updateInterface();
}

QString DiagramInterface::description() const {
	Q_ASSERT(busInterface_);
	return busInterface_->getDescription();
}

void DiagramInterface::setDescription( const QString& description ) {
	Q_ASSERT(busInterface_);
	busInterface_->setDescription(description);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: getOffPageConnector()
//-----------------------------------------------------------------------------
ConnectionEndpoint* DiagramInterface::getOffPageConnector()
{
    return offPageConnector_;
}

//-----------------------------------------------------------------------------
// Function: DiagramInterface::isBus()
//-----------------------------------------------------------------------------
bool DiagramInterface::isBus() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: DiagramInterface::getPort()
//-----------------------------------------------------------------------------
Port* DiagramInterface::getPort() const
{
    return 0;
}
