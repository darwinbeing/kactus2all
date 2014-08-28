/* 
 *
 * 		filename: diagraminterconnection.cpp
 */

#include "HWConnection.h"
#include "HWComponentItem.h"
#include "HWMoveCommands.h"
#include "HWDesignDiagram.h"
#include "HWDesignWidget.h"

#include <common/GenericEditProvider.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/PortMap.h>

#include <cmath>

//-----------------------------------------------------------------------------
// Function: HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(ConnectionEndpoint* endpoint1,
                                               ConnectionEndpoint* endpoint2,
                                               bool autoConnect,
                                               QString const& name, 
                                               QString const& displayName,
                                               QString const& description,
                                               DesignDiagram* parent)
    : GraphicsConnection(endpoint1, endpoint2, autoConnect, name, displayName, description, parent), 
      widthLabel_(new QGraphicsTextItem(this)), vendorExtensions_()
{
    setWidthLabelDefaultFont();

    if (autoConnect)
    {
        updateWidthLabel();
    }
}

//-----------------------------------------------------------------------------
// Function: HWConnection()
//-----------------------------------------------------------------------------
HWConnection::HWConnection(QPointF const& p1, QVector2D const& dir1,
                                               QPointF const& p2, QVector2D const& dir2,
                                               QString const& displayName,
											   QString const& description,
											   DesignDiagram* parent)
    : GraphicsConnection(p1, dir1, p2, dir2, displayName, description, parent),
      widthLabel_(new QGraphicsTextItem(this)), vendorExtensions_()
{
    setWidthLabelDefaultFont();
}

//-----------------------------------------------------------------------------
// Function: ~HWConnection()
//-----------------------------------------------------------------------------
HWConnection::~HWConnection()
{
}

//-----------------------------------------------------------------------------
// Function: connectEnds()
//-----------------------------------------------------------------------------
bool HWConnection::connectEnds()
{
    if (!GraphicsConnection::connectEnds())
    {
        return false;
    }

    // If the connection is ad-hoc, take the default left and right bounds from the ports if they are undefined.
    if (getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_ADHOC)
    {
        if (getAdHocLeftBound(0) == -1)
        {
            setAdHocLeftBound(0, endpoint1()->getPort()->getLeftBound());
        }

        if (getAdHocRightBound(0) == -1)
        {
            setAdHocRightBound(0, endpoint1()->getPort()->getRightBound());
        }

        if (getAdHocLeftBound(1) == -1)
        {
            setAdHocLeftBound(1, endpoint2()->getPort()->getLeftBound());
        }

        if (getAdHocRightBound(1) == -1)
        {
            setAdHocRightBound(1, endpoint2()->getPort()->getRightBound());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setRoute()
//-----------------------------------------------------------------------------
void HWConnection::setRoute(QList<QPointF> path)
{
    GraphicsConnection::setRoute(path);
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::updatePosition()
//-----------------------------------------------------------------------------
void HWConnection::updatePosition()
{
    GraphicsConnection::updatePosition();
    updateWidthLabel();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::updateWidthLabel()
//-----------------------------------------------------------------------------
void HWConnection::updateWidthLabel()
{
    if (route().size() < 2)
    {
        return;
    }

    int totalWidth = 0;

    // for bus connections, the bus width must be calculated from the port maps.
    if (isBus())
    {
        totalWidth = calculateBusWidth();
    }
    else
    {
        // Otherwise the connection is for only one signal (i.e. ad-hoc).
        totalWidth = 1;
    }

    widthLabel_->setPlainText(QString::number(totalWidth));

    QPointF textPos = findWidthLabelPosition();
    widthLabel_->setPos(textPos);
}

//-----------------------------------------------------------------------------
// Function: setBusWidthVisible()
//-----------------------------------------------------------------------------
void HWConnection::setBusWidthVisible(bool visible)
{
    widthLabel_->setVisible(visible);
}

//-----------------------------------------------------------------------------
// Function: HWConnection::calculateBusWidth()
//-----------------------------------------------------------------------------
int HWConnection::calculateBusWidth() const
{
    int totalWidth = 0;

    foreach (QSharedPointer<PortMap> portMap1, endpoint1()->getBusInterface()->getPortMaps())
    {
        // Find the port map with the same logical port name from the other end point's port map.
        QSharedPointer<PortMap> portMap2;

        foreach (QSharedPointer<PortMap> portMap, endpoint2()->getBusInterface()->getPortMaps())
        {
            if (portMap->logicalPort() == portMap1->logicalPort())
            {
                portMap2 = portMap;
            }
        }

        if (portMap2 == 0)
        {
            continue;
        }

        QSharedPointer<Port> port1 = endpoint1()->getOwnerComponent()->getPort(portMap1->physicalPort());
        QSharedPointer<Port> port2 = endpoint2()->getOwnerComponent()->getPort(portMap2->physicalPort());

        if (!port1 || !port2)
        {
            continue;
        }
        
        // Calculate the intersection of the port bounds and add it to the total width.
        General::PortAlignment align =
            General::calculatePortAlignment(portMap1.data(), port1->getLeftBound(), port1->getRightBound(),
            portMap2.data(), port2->getLeftBound(), port2->getRightBound());

        int width = qMin(align.port1Left_, align.port2Left_) - qMax(align.port1Right_, align.port2Right_) + 1;
        totalWidth += width;
    }

    return totalWidth;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::isBus()
//-----------------------------------------------------------------------------
bool HWConnection::isBus() const
{
    return endpoint1()->isBus();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setVendorExtensions()
//-----------------------------------------------------------------------------
void HWConnection::setVendorExtensions(QList<QSharedPointer<VendorExtension> > const& vendorExtensions)
{
    vendorExtensions_ = vendorExtensions;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getVendorExtensions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > HWConnection::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getAdHocLeftBound()
//-----------------------------------------------------------------------------
int HWConnection::getAdHocLeftBound(int endpointIndex) const
{
    return portBounds_[endpointIndex].left_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::getAdHocRightBound()
//-----------------------------------------------------------------------------
int HWConnection::getAdHocRightBound(int endpointIndex) const
{
    return portBounds_[endpointIndex].right_;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setAdHocLeftBound()
//-----------------------------------------------------------------------------
void HWConnection::setAdHocLeftBound(int endpointIndex, int leftBound)
{
    portBounds_[endpointIndex].left_ = leftBound;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setAdHocRightBound()
//-----------------------------------------------------------------------------
void HWConnection::setAdHocRightBound(int endpointIndex, int rightBound)
{
    portBounds_[endpointIndex].right_ = rightBound;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: HWConnection::findWidthLabelPosition()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelPosition() const
{
    QPointF textPos = boundingRect().center();
    if (getRoutingMode() == ROUTING_MODE_OFFPAGE)
    {
        textPos.setX(textPos.x() - widthLabel_->boundingRect().width() * 0.5);
        textPos.setY(textPos.y() - 2 * GridSize);
    }
    else
    {
        // If the end points are more apart in horizontal direction, place the text above
        // the longest horizontal segment.
        if (qAbs(endpoint1()->scenePos().x() - endpoint2()->scenePos().x()) >=
            qAbs(endpoint1()->scenePos().y() - endpoint2()->scenePos().y()))
        {
            textPos = findWidthLabelHorizontally();
        }
        // Otherwise place the text beside the longest vertical segment.
        else
        {
            textPos = findWidthLabelVertically();
        }
    }
    return textPos;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::findWidthLabelHorizontally()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelHorizontally() const
{
    QPointF position;
    QList<QPointF> const& pathPoints = route();

    // Determine the longest horizontal segment.
    int longestIndex = 0;
    qreal longestLength = 0.0;

    for (int i = 0; i < route().size() - 1; ++i)
    {
        QPointF const& pt1 = pathPoints[i];
        QPointF const& pt2 = pathPoints[i + 1];
        qreal length = QVector2D(pt2 - pt1).length();

        if (qFuzzyCompare(pt1.y(), pt2.y()) && length > longestLength)
        {
            longestLength = length;
            longestIndex = i;
        }
    }

    QPointF const& pt1 = pathPoints[longestIndex];
    QPointF const& pt2 = pathPoints[longestIndex + 1];

    position.setX((pt1.x() + pt2.x()) * 0.5 - widthLabel_->boundingRect().width() * 0.5);
    position.setY(pt1.y() - 2 * GridSize);
    return position;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::findWidthLabelVertically()
//-----------------------------------------------------------------------------
QPointF HWConnection::findWidthLabelVertically() const
{
    QPointF position;
    QList<QPointF> const& pathPoints = route();

    // Determine the longest vertical segment.
    int longestIndex = 0;
    qreal longestLength = 0.0;

    for (int i = 0; i < pathPoints.size() - 1; ++i)
    {
        QPointF const& pt1 = pathPoints[i];
        QPointF const& pt2 = pathPoints[i + 1];
        qreal length = QVector2D(pt2 - pt1).length();

        if (qFuzzyCompare(pt1.x(), pt2.x()) && length > longestLength)
        {
            longestLength = length;
            longestIndex = i;
        }
    }

    Q_ASSERT(longestIndex > 0 && longestIndex + 2 < pathPoints.size());
    QPointF const& pt1 = pathPoints[longestIndex];
    QPointF const& pt2 = pathPoints[longestIndex + 1];

    // Place the text on the right side if the connection is fully on the left.
    if (pathPoints[longestIndex - 1].x() < pt1.x() && pathPoints[longestIndex + 2].x() < pt1.x())
    {
        position.setX(pt1.x() + GridSize);
    }
    // Place the text on the left side if the connection is fully on the right.
    else if (pathPoints[longestIndex - 1].x() > pt1.x() && pathPoints[longestIndex + 2].x() > pt1.x())
    {
        position.setX(pt1.x() - widthLabel_->boundingRect().width());
    }
    else
    {
        // Otherwise we have to determine on which side there is more space.
        if (QVector2D(pathPoints[longestIndex - 1] - pt1).lengthSquared() <
            QVector2D(pathPoints[longestIndex + 2] - pt2).lengthSquared())
        {
            position.setX(pt1.x() + GridSize);
        }
        else
        {
            position.setX(pt1.x() - widthLabel_->boundingRect().width());
        }
    }

    position.setY((pt1.y() + pt2.y()) * 0.5 - widthLabel_->boundingRect().height() * 0.5);
    return position;
}

//-----------------------------------------------------------------------------
// Function: HWConnection::setWidthLabelFont()
//-----------------------------------------------------------------------------
void HWConnection::setWidthLabelDefaultFont()
{
    QFont font = widthLabel_->font();
    font.setPointSize(8);
    font.setBold(true);
    widthLabel_->setFont(font);
}
