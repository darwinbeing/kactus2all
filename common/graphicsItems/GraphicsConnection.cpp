//-----------------------------------------------------------------------------
// File: GraphicsConnection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.6.2012
//
// Description:
// 
//-----------------------------------------------------------------------------

#include "GraphicsConnection.h"

#include "ComponentItem.h"
#include "ConnectionUndoCommands.h"

#include <common/GenericEditProvider.h>
#include <common/DesignDiagram.h>
#include <common/DiagramUtil.h>
#include <common/KactusColors.h>

#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

qreal const GraphicsConnection::MIN_LENGTH = 10;
qreal const GraphicsConnection::MIN_START_LENGTH = 20;

//-----------------------------------------------------------------------------
// Function: GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::GraphicsConnection(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2,
                                       bool autoConnect, QString const& name, QString const& displayName,
                                       QString const& description, DesignDiagram* parent)
    : QGraphicsPathItem(), 
      parent_(parent),
      name_(name), 
      description_(description),
      endpoint1_(0),
      endpoint2_(0), 
      pathPoints_(), 
      selected_(-1), 
      selectionType_(NONE),
      routingMode_(ROUTING_MODE_NORMAL),
      imported_(false),
      invalid_(false)
{
    setItemSettings();
    createRoute(endpoint1, endpoint2);

    if (endpoint1->isAdHoc())
    {
        setLineWidth(1);
    }

    if (autoConnect)
    {
        endpoint1_ = endpoint1;
        endpoint2_ = endpoint2;

        endpoint1->onConnect(endpoint2);
        endpoint2->onConnect(endpoint1);

        validate();

        endpoint1->addConnection(this);
        endpoint2->addConnection(this);

        if (name_.isEmpty())
        {
            name_ = createDefaultName();
        }
    }

    setDefaultColor();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::GraphicsConnection(QPointF const& p1, QVector2D const& dir1,
                                       QPointF const& p2, QVector2D const& dir2,
                                       QString const& displayName,
                                       QString const& description,
                                       DesignDiagram* parent)
    : QGraphicsPathItem(),
      parent_(parent),
      name_(),
      description_(description),
      endpoint1_(0), 
      endpoint2_(0),
      pathPoints_(),
      selected_(-1),
      selectionType_(NONE),
      routingMode_(ROUTING_MODE_NORMAL),
      imported_(false),
      invalid_(false)
{
    setItemSettings();
    createRoute(p1, p2, dir1, dir2);
}

//-----------------------------------------------------------------------------
// Function: ~GraphicsConnection()
//-----------------------------------------------------------------------------
GraphicsConnection::~GraphicsConnection()
{
    disconnectEnds();
    emit destroyed(this);
}

//-----------------------------------------------------------------------------
// Function: connectEnds()
//-----------------------------------------------------------------------------
bool GraphicsConnection::connectEnds()
{
    if (!scene())
        return false;

    prepareGeometryChange();

    // Disconnect the previous ends.
    disconnectEnds();

    // Find the new end points.
    endpoint1_ = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.first(), scene(), GridSize);
    Q_ASSERT(endpoint1_ != 0);

    endpoint2_ = DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.last(), scene(), GridSize);
    Q_ASSERT(endpoint2_ != 0);

    // Swap the end points in a way that the first one at least has an encompassing component.
    if (endpoint1_->encompassingComp() == 0)
    {
        std::swap(endpoint1_, endpoint2_);

        // The path points have to be reversed.
        for (int i = 0; i < pathPoints_.size() / 2; ++i)
        {
            pathPoints_.swap(i, pathPoints_.size() - 1 - i);
        }
    }

    // Make the connections and check for errors.
    if (!endpoint1_->onConnect(endpoint2_))
    {
        endpoint1_ = 0;
        return false;
    }

    if (!endpoint2_->onConnect(endpoint1_))
    {
        endpoint1_->onDisconnect(endpoint2_);
        endpoint1_ = 0;
        endpoint2_ = 0;
        return false;
    }

    validate();

    endpoint1_->addConnection(this);
    endpoint2_->addConnection(this);

    simplifyPath();
    setRoute(pathPoints_);
    name_ = createDefaultName();
    return true;
}

//-----------------------------------------------------------------------------
// Function: setRoute()
//-----------------------------------------------------------------------------
void GraphicsConnection::setRoute(QList<QPointF> path)
{
    if (path.size() < 2)
        return;

    if (endpoint1_)
    {
        QVector2D dir = QVector2D(path[1] - path[0]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (!endpoint1_->isDirectionFixed() && QVector2D::dotProduct(dir, endpoint1_->getDirection()) < 0)
        {
            endpoint1_->setDirection(dir);
        }
    }

    if (endpoint2_)
    {
        QVector2D dir = QVector2D(path[path.size() - 2] - path[path.size() - 1]).normalized();

        // Switch the direction of the end point if it is not correct.
        if (!endpoint2_->isDirectionFixed() && QVector2D::dotProduct(dir, endpoint2_->getDirection()) < 0)
        {
            endpoint2_->setDirection(dir);
        }
    }

    QListIterator<QPointF> i(path);

    QPainterPath painterPath(i.next());

    while (i.hasNext()) {
        painterPath.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(painterPath));

    pathPoints_ = path;
}

//-----------------------------------------------------------------------------
// Function: name()
//-----------------------------------------------------------------------------
QString const& GraphicsConnection::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: setName()
//-----------------------------------------------------------------------------
void GraphicsConnection::setName(QString const& name)
{
    name_ = name;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: description()
//-----------------------------------------------------------------------------
QString const& GraphicsConnection::description() const
{
    return description_;
}

//-----------------------------------------------------------------------------
// Function: setDescription()
//-----------------------------------------------------------------------------
void GraphicsConnection::setDescription(QString const& description)
{
    description_ = description;
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: endpoint1()
//-----------------------------------------------------------------------------
ConnectionEndpoint* GraphicsConnection::endpoint1() const
{
    return endpoint1_;
}

//-----------------------------------------------------------------------------
// Function: endpoint2()
//-----------------------------------------------------------------------------
ConnectionEndpoint* GraphicsConnection::endpoint2() const
{
    return endpoint2_;
}

//-----------------------------------------------------------------------------
// Function: updatePosition()
//-----------------------------------------------------------------------------
void GraphicsConnection::updatePosition()
{
    if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        QVector2D delta1 = QVector2D(endpoint1_->scenePos()) - QVector2D(pathPoints_.first());
        QVector2D delta2 = QVector2D(endpoint2_->scenePos()) - QVector2D(pathPoints_.last());
        QVector2D const& dir1 = endpoint1_->getDirection();
        QVector2D const& dir2 = endpoint2_->getDirection();

        // Recreate the route from scratch if there are not enough points in the path or
        // the route is too complicated when the position and direction of the endpoints is considered.
        if (pathPoints_.size() < 2 ||
            (pathPoints_.size() > 4 && qFuzzyCompare(QVector2D::dotProduct(dir1, dir2), -1.0) &&
            QVector2D::dotProduct(dir1, QVector2D(endpoint2_->scenePos() - endpoint1_->scenePos())) > 0.0))
        {
            createRoute(endpoint1_, endpoint2_);
            return;
        }

        // If the delta movement of both endpoints was the same, we can just
        // move all route points by the delta1.
        if (qFuzzyCompare(delta1, delta2))
        {
            if (!delta1.isNull())
            {
                for (int i = 0; i < pathPoints_.size(); ++i)
                {
                    pathPoints_[i] += delta1.toPointF();
                }

                setRoute(pathPoints_);
            }
        }
        // Otherwise check if either the first or the last point was moved.
        else if (!delta1.isNull() || !delta2.isNull())
        {
            bool pathOk = false;
            QVector2D delta = delta1;
            QVector2D dir = dir1;
            ConnectionEndpoint* endpoint = endpoint1_;
            int index0 = 0;
            int index1 = 1;
            int index2 = 2;
            int index3 = 3;

            if (!delta2.isNull())
            {
                delta = delta2;
                endpoint = endpoint2_;
                dir = dir2;
                index0 = pathPoints_.size() - 1;
                index1 = pathPoints_.size() - 2;
                index2 = pathPoints_.size() - 3;
                index3 = pathPoints_.size() - 4;
            }

            QVector2D seg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]).normalized();

            // Try to fix the first segment with perpendicular projection.
            if (pathPoints_.size() >= 4 && pathPoints_.size() < 7 && qFuzzyCompare(dir, seg1))
            {
                QVector2D perp = delta - QVector2D::dotProduct(delta, seg1) * seg1;
                pathPoints_[index1] += perp.toPointF();

                // The path is ok if the moved point is still in view (not behind the left edge).
                pathOk = pathPoints_[index1].x() >= 10.0;
            }

            // Handle the parallel part of the delta.
            pathPoints_[index0] = endpoint->scenePos();
            QVector2D newSeg1 = QVector2D(pathPoints_[index1] - pathPoints_[index0]);

            if (newSeg1.length() < MIN_START_LENGTH || !qFuzzyCompare(seg1, newSeg1.normalized()))
            {
                pathOk = false;
            }

            // Check for a special case when there would be intersecting parallel lines.
            if (pathOk && pathPoints_.size() >= 4)
            {
                QVector2D seg2 = QVector2D(pathPoints_[index2] - pathPoints_[index1]).normalized();
                QVector2D seg3 = QVector2D(pathPoints_[index3] - pathPoints_[index2]).normalized();

                if (QVector2D::dotProduct(seg1, seg2) < 0.0f ||
                    (seg2.isNull() && QVector2D::dotProduct(seg1, seg3) < 0.0f))
                {
                    pathOk = false;
                }
            }

            // Snap the middle path points to grid.
            for (int i = 1; i < pathPoints_.size() - 1; ++i)
            {
                pathPoints_[i] = snapPointToGrid(pathPoints_[i]);
            }

            // If the simple fix didn't result in a solution, just recreate the route.
            if (!pathOk)
            {
                createRoute(endpoint1_, endpoint2_);
            }
            else
            {
                simplifyPath();
                setRoute(pathPoints_);
            }
        }
    }
    else if (routingMode_ == ROUTING_MODE_OFFPAGE)
    {
        // Make a straight line from begin to end.
        QList<QPointF> route;
        route.append(endpoint1()->scenePos());
        route.append(endpoint2()->scenePos());

        setRoute(route);
    }
}

//-----------------------------------------------------------------------------
// Function: route()
//-----------------------------------------------------------------------------
QList<QPointF> const& GraphicsConnection::route() const
{
    return pathPoints_;
}

//-----------------------------------------------------------------------------
// Function: simplifyPath()
//-----------------------------------------------------------------------------
void GraphicsConnection::simplifyPath()
{
    if (pathPoints_.size() < 3)
        return;

    for (int i = 0; i < pathPoints_.size() - 2; ++i)
    {
        QVector2D pt0 = QVector2D(pathPoints_[i]);
        QVector2D pt1 = QVector2D(pathPoints_[i + 1]);
        QVector2D pt2 = QVector2D(pathPoints_[i + 2]);

        QVector2D delta1 = pt1 - pt0;
        QVector2D delta2 = pt2 - pt1;

        QVector2D deltaProj = QVector2D::dotProduct(delta2, delta1.normalized()) * delta1.normalized();

        // If the path was otherwise ok, just remove parallel lines.
        if (qFuzzyCompare(deltaProj, delta2))
        {
            pathPoints_.removeAt(i + 1);
            --i;
        }                                
    }

    selected_ = -1;
    selectionType_ = NONE;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void GraphicsConnection::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QPointF pos = snapPointToGrid(mouseEvent->pos());

    oldRoute_ = route();

    if (pathPoints_.first() == pos)
    {
        selectionType_ = END;
        selected_ = 0;
    }
    else if (pathPoints_.last() == pos)
    {
        selectionType_ = END;
        selected_ = pathPoints_.size()-1;
    }
    else if (pathPoints_.size() > 1)
    {
        for (int i = 0; i < pathPoints_.size()-1; i++)
        {
            if ((i == 0 && endpoint1_) || (i == pathPoints_.size()-2 && endpoint2_))
            {
                continue;
            }

            if ((qFuzzyCompare(pathPoints_[i].x(), pos.x()) && qFuzzyCompare(pathPoints_[i+1].x(), pos.x())) ||
                (qFuzzyCompare(pathPoints_[i].y(), pos.y()) && qFuzzyCompare(pathPoints_[i+1].y(), pos.y())))
            {
                selected_ = i;
                selectionType_ = SEGMENT;
            }
        }
    }
    else
    {
        selected_ = -1;
        selectionType_ = NONE;
    }

    QGraphicsPathItem::mousePressEvent(mouseEvent);
}

void GraphicsConnection::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Discard mouse move if the diagram is protected.
    if (parent_->isProtected())
    {
        return;
    }

    QPointF newPos = snapPointToGrid(mouseEvent->pos());

    if (selectionType_ == END)
    {
        // Disconnect the ends.
        disconnectEnds();

        if (pathPoints_.size() > 2) {
            if (selected_ == pathPoints_.size()-1) {
                if (qFuzzyCompare(pathPoints_[selected_-1].x(), pathPoints_[selected_-2].x())) {
                    pathPoints_[selected_-1].setY(newPos.y());
                } else {
                    pathPoints_[selected_-1].setX(newPos.x());
                }
            } else if (selected_ == 0) {
                if (qFuzzyCompare(pathPoints_[selected_+1].x(), pathPoints_[selected_+2].x())) {
                    pathPoints_[selected_+1].setY(newPos.y());
                } else {
                    pathPoints_[selected_+1].setX(newPos.x());
                }
            }
            pathPoints_[selected_] = newPos;
            setRoute(pathPoints_);
        } else {
            QPointF oldPos = pathPoints_[selected_];

            if (qFuzzyCompare(pathPoints_.first().x(), pathPoints_.last().x())) {
                pathPoints_[selected_].setY(newPos.y());
            } else {
                pathPoints_[selected_].setX(newPos.x());
            }
            if (pathPoints_.first() == pathPoints_.last())
                pathPoints_[selected_] = oldPos;
            else
                setRoute(pathPoints_);
        }
    }
    else if (selectionType_ == SEGMENT)
    {
        if (qFuzzyCompare(pathPoints_[selected_].x(), pathPoints_[selected_+1].x()))
        {
            // Clamp the movement delta.
            qreal delta = newPos.x() - pathPoints_[selected_].x();
            qreal prev = pathPoints_[selected_ - 1].x();
            qreal cur = pathPoints_[selected_].x();
            qreal next = pathPoints_[selected_ + 2].x();

            if (cur > next)
            {
                delta = std::max(delta, MIN_START_LENGTH + next - cur);
            }
            else
            {
                delta = std::min(delta, -MIN_START_LENGTH + next - cur);
            }

            if (cur > prev)
            {
                delta = std::max(delta, MIN_START_LENGTH + prev - cur);
            }
            else
            {
                delta = std::min(delta, -MIN_START_LENGTH + prev - cur);
            }

            pathPoints_[selected_].setX(pathPoints_[selected_].x() + delta);
            pathPoints_[selected_+1].setX(pathPoints_[selected_+1].x() + delta);
        }
        else if (qFuzzyCompare(pathPoints_[selected_].y(), pathPoints_[selected_+1].y()))
        {
            qreal prev = pathPoints_[selected_ - 1].y();
            qreal next = pathPoints_[selected_ + 2].y();

            // Change the route only if the next and previous segments would not be too short.
            if (abs(newPos.y() - prev) >= MIN_LENGTH && abs(newPos.y() - next) >= MIN_LENGTH)
            {
                pathPoints_[selected_].setY(newPos.y());
                pathPoints_[selected_+1].setY(newPos.y());
            }
        }

        setRoute(pathPoints_);
    }

    QGraphicsPathItem::mouseMoveEvent(mouseEvent);
}

void GraphicsConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (selectionType_ == END)
    {
        ConnectionEndpoint* endpoint1 =
            DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.first(), scene(), GridSize);
        ConnectionEndpoint* endpoint2 =
            DiagramUtil::snapToItem<ConnectionEndpoint>(pathPoints_.last(), scene(), GridSize);

        if (endpoint1 != 0 && endpoint2 != 0 &&
            endpoint1->canConnect(endpoint2) && endpoint2->canConnect(endpoint1))
        {
            connectEnds();
        }
    }
    else if (selectionType_ == SEGMENT)
    {
        simplifyPath();
        setRoute(pathPoints_);
    }

    if (route() != oldRoute_)
    {
        QSharedPointer<QUndoCommand> cmd(new ConnectionMoveCommand(this, oldRoute_));
        parent_->getEditProvider().addCommand(cmd);
    }

    QGraphicsPathItem::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: paint()
//-----------------------------------------------------------------------------
void GraphicsConnection::paint(QPainter* painter, QStyleOptionGraphicsItem const* option,
                         QWidget* widget)
{
    bool selected = option->state & QStyle::State_Selected;

    QStyleOptionGraphicsItem myoption = (*option);
    myoption.state &= !QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, &myoption, widget);

    if (!selected && routingMode_ == ROUTING_MODE_NORMAL)
    {
        drawOverlapGraphics(painter);
    }

    if (!endpoint1_)
    {
        painter->fillRect(QRectF(pathPoints_.first()-QPointF(2,2),
            pathPoints_.first()+QPointF(2,2)),
            QBrush(Qt::red));
    }

    if (!endpoint2_)
    {
        painter->fillRect(QRectF(pathPoints_.last()-QPointF(2,2),
            pathPoints_.last()+QPointF(2,2)),
            QBrush(Qt::red));
    }
}

//-----------------------------------------------------------------------------
// Function: setItemSettings()
//-----------------------------------------------------------------------------
void GraphicsConnection::setItemSettings()
{
    setZValue(-1000);
    setLineWidth(2);
    setFlag(ItemIsSelectable);
}

//-----------------------------------------------------------------------------
// Function: createRoute()
//-----------------------------------------------------------------------------
void GraphicsConnection::createRoute(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2)
{
    Q_ASSERT(endpoint1 != 0);
    Q_ASSERT(endpoint2 != 0);

    QPointF p1 = endpoint1->scenePos();
    QPointF p2 = endpoint2->scenePos();

    if (!endpoint1->isDirectionFixed())
    {
        if (p1.x() <= p2.x())
        {
            endpoint1->setDirection(QVector2D(1.0f, 0.0f));
        }
        else
        {
            endpoint1->setDirection(QVector2D(-1.0f, 0.0f));
        }
    }

    if (!endpoint2->isDirectionFixed())
    {
        if (p1.x() <= p2.x())
        {
            endpoint2->setDirection(QVector2D(-1.0f, 0.0f));
        }
        else
        {
            endpoint2->setDirection(QVector2D(1.0f, 0.0f));
        }
    }

    createRoute(p1, p2, endpoint1->getDirection(), endpoint2->getDirection());
}

//-----------------------------------------------------------------------------
// Function: createRoute()
//-----------------------------------------------------------------------------
void GraphicsConnection::createRoute(QPointF p1, QPointF p2,
                               QVector2D const& dir1, QVector2D const& dir2)
{
    pathPoints_.clear();

    // Convert the points to vectors.
    QVector2D startPos = QVector2D(p1);
    QVector2D endPos = QVector2D(p2);

    // Sets the start position as the current position.
    QVector2D curPos = startPos;
    QVector2D curDir = dir1;

    // Set the target position based on the end point's direction.
    QVector2D targetPos = QVector2D(p2) + dir2 * MIN_START_LENGTH;

    // Add the start position to the list of path points.
    pathPoints_ << curPos.toPointF();

    if (startPos != endPos)
    {
        // Find a route to the target.
        while (curPos != targetPos)
        {
            // Calculate the delta.
            QVector2D delta = targetPos - curPos;
            qreal dot = QVector2D::dotProduct(delta, curDir);
            qreal endDot = QVector2D::dotProduct(delta, dir2);

            // Calculate the projection onto the current direction and the perpendicular part.
            QVector2D proj = dot * curDir;
            QVector2D perp = delta - proj;

            // Check if we can draw a direct line to the target from the current position.
            if (dot > 0.0 && qFuzzyCompare(delta, proj) && endDot <= 0.0)
            {
                curPos = targetPos;
            }
            else 
            {
                // Otherwise draw at least some distance to the current direction.
                // Check if the target is not behind the current position/direction.
                if (qFuzzyCompare(curPos, startPos))
                {
                    if (dot > 0.0 && !(endDot > 0.0 && qFuzzyCompare(delta, endDot * dir2)))
                    {
                        // Draw the length of the projection to the current direction
                        // or at least the minimum length.
                        curPos = curPos + curDir * std::max(MIN_START_LENGTH, proj.length());
                    }
                    else
                    {
                        // Otherwise we just draw the minimum length thub.
                        curPos = curPos + curDir * MIN_START_LENGTH;
                    }
                }
                // Check if the target is in the opposite direction compared to the current
                // direction and we previously draw the starting thub.
                else if (dot < 0.0 && qFuzzyCompare(curPos, startPos + curDir * MIN_START_LENGTH))
                {
                    // Draw to the perpendicular direction at least the minimum length.
                    qreal length = std::max(perp.length(), MIN_LENGTH);
                    QVector2D dir = perp.normalized();

                    // Special case when the perpendicular vector would be zero-length.
                    if (dir.isNull())
                    {
                        // Rotate 90 degrees.
                        dir = QVector2D(curDir.y(), -curDir.x());
                    }

                    curPos = curPos + dir * length;
                    curDir = dir;
                }
                else
                {
                    // Otherwise we just draw to the perpendicular direction as much as we can.
                    if (!perp.isNull())
                    {
                        curPos = curPos + perp;
                        curDir = perp.normalized();
                    }
                    else
                    {
                        // If the perpendicular vector was zero-length, we rotate the current
                        // direction 90 degrees and draw a minimum length thub to the new direction.
                        curDir = QVector2D(curDir.y(), -curDir.x());
                        curPos = curPos + curDir * MIN_LENGTH;
                    }
                }

                // Check if we would end up in a position where we are right behind the target.
                QVector2D newDelta = targetPos - curPos;
                qreal endDot = QVector2D::dotProduct(newDelta, dir2);

                if (endDot > 0.0 && qFuzzyCompare(newDelta, endDot * dir2) &&
                    !qFuzzyCompare(curPos, startPos + curDir * MIN_START_LENGTH))
                {
                    // Make an adjustment to the current position.
                    curPos += curDir * MIN_LENGTH;
                }
            }

            // Add the newly calculated current position to the path.
            pathPoints_ << curPos.toPointF();
        }

        // Add the last segment if the target position was not the end position.
        if (!qFuzzyCompare(targetPos, endPos))
        {
            pathPoints_ << endPos.toPointF();
        }
    }

    simplifyPath();

    QListIterator<QPointF> i(pathPoints_);

    QPainterPath path(i.next());

    while (i.hasNext()) {
        path.lineTo(i.next());
    }

    QPainterPathStroker stroker;
    setPath(stroker.createStroke(path));
}

//-----------------------------------------------------------------------------
// Function: updateName()
//-----------------------------------------------------------------------------
QString GraphicsConnection::createDefaultName() const
{
    Q_ASSERT(endpoint1_ != 0);
    Q_ASSERT(endpoint2_ != 0);

    // Determine one of the end points as the starting point in a way that its
    // encompassing component is defined.
    ConnectionEndpoint* start = endpoint1_;
    ConnectionEndpoint* end = endpoint2_;

    if (start->encompassingComp() == 0)
    {
        std::swap(start, end);
        Q_ASSERT(start->encompassingComp() != 0);
    }

    // Update the name string.
    QString endCompName = "";

    if (end->encompassingComp() != 0)
    {
        endCompName = end->encompassingComp()->name() + "_";
    }

    return start->encompassingComp()->name() + "_" + start->name() + "_to_" + endCompName + end->name();
}

//-----------------------------------------------------------------------------
// Function: disconnectEnds()
//-----------------------------------------------------------------------------
void GraphicsConnection::disconnectEnds()
{
    // Discard existing connections.
    if (endpoint1_)
    {
        endpoint1_->removeConnection(this);
        endpoint1_->onDisconnect(endpoint2_);
        endpoint1_->setSelectionHighlight(false);
    }

    if (endpoint2_)
    {
        endpoint2_->removeConnection(this);
        endpoint2_->onDisconnect(endpoint1_);
        endpoint2_->setSelectionHighlight(false);
    }

    endpoint1_ = 0;
    endpoint2_ = 0;
}

//-----------------------------------------------------------------------------
// Function: itemChange()
//-----------------------------------------------------------------------------
QVariant GraphicsConnection::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change)
    {
    case ItemSelectedHasChanged:
        {
            bool selected = value.toBool();
            setDefaultColor();

            if (endpoint1_ != 0)
            {
                endpoint1_->setSelectionHighlight(selected);
            }

            if (endpoint2_ != 0)
            {
                endpoint2_->setSelectionHighlight(selected);
            }

            return value;
        }
    }

    return QGraphicsPathItem::itemChange(change, value);
}

//-----------------------------------------------------------------------------
// Function: beginUpdatePosition()
//-----------------------------------------------------------------------------
void GraphicsConnection::beginUpdatePosition()
{
    oldRoute_ = route();
}

//-----------------------------------------------------------------------------
// Function: endUpdatePosition()
//-----------------------------------------------------------------------------
QUndoCommand* GraphicsConnection::endUpdatePosition(QUndoCommand* parent)
{
    if (route() != oldRoute_)
    {
        return new ConnectionMoveCommand(this, oldRoute_, parent);
    }
    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: setRoutingMode()
//-----------------------------------------------------------------------------
void GraphicsConnection::setRoutingMode(RoutingMode style)
{
    if (routingMode_ != style)
    {
        routingMode_ = style;

        setDefaultColor();
        updatePosition();
    }
}

//-----------------------------------------------------------------------------
// Function: setDefaultColor()
//-----------------------------------------------------------------------------
void GraphicsConnection::setDefaultColor()
{
    QPen newPen = pen();

    if (isSelected())
    {
        newPen.setColor(KactusColors::DIAGRAM_SELECTION);
    }
    else if (invalid_)
    {
        newPen.setColor(KactusColors::BROKEN_CONNECTION);
    }
    else if (routingMode_ == ROUTING_MODE_NORMAL)
    {
        if (getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_COM)
        {
            newPen.setColor(KactusColors::COM_CONNECTION);
        }
        else
        {
            newPen.setColor(Qt::black);
        }
    }
    else
    {
        newPen.setColor(QColor(180, 180, 180));
    }

    setPen(newPen);
}

//-----------------------------------------------------------------------------
// Function: drawLineGap()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawLineGap(QPainter* painter, QLineF const& line1, QPointF const& pt)
{
    QVector2D dir(line1.dx(), line1.dy());
    dir.normalize();

    qreal length1 = QVector2D(pt - line1.p1()).length();
    qreal length2 = QVector2D(pt - line1.p2()).length();

    QPointF pt1 = (QVector2D(pt) + dir * std::min(length2, (qreal)GridSize / 2)).toPointF();
    QPointF pt2 = (QVector2D(pt) - dir * std::min(length1, (qreal)GridSize) / 2).toPointF();
    painter->drawLine(pt1, pt2);
}

//-----------------------------------------------------------------------------
// Function: drawOverlapGraphics()
//-----------------------------------------------------------------------------
void GraphicsConnection::drawOverlapGraphics(QPainter* painter)
{
    // Determine all items that collide with this item.
    foreach (QGraphicsItem* item, collidingItems())
    {
        QList<QPointF> const& route1 = pathPoints_;

        // Paint junction marks to those parts that go vertically and cross another connection.
        if (dynamic_cast<GraphicsConnection*>(item) != 0)
        {
            GraphicsConnection* conn = static_cast<GraphicsConnection*>(item);
            QList<QPointF> const& route2 = conn->route();

            for (int i = 0; i < route1.size() - 1; ++i)
            {
                // Discard horizontal segments.
                if (qFuzzyCompare(route1[i].y(), route1[i + 1].y()))
                {
                    continue;
                }

                QLineF line1(route1[i], route1[i + 1]);

                for (int j = 0; j < route2.size() - 1; ++j)
                {
                    // Discard vertical segments of the intersecting connections.
                    if (qFuzzyCompare(route2[j].x(), route2[j + 1].x()))
                    {
                        continue;
                    }

                    QLineF line2(route2[j], route2[j + 1]);

                    QPointF pt;
                    QLineF::IntersectType type = line1.intersect(line2, &pt);

                    if (type == QLineF::BoundedIntersection)
                    {
                        // If the connections share an endpoint, draw a black junction circle.
                        if (endpoint1() == conn->endpoint1() || endpoint2() == conn->endpoint2() ||
                            endpoint1() == conn->endpoint2() || endpoint2() == conn->endpoint1())
                        {
                            painter->setPen(QPen(Qt::black, 0));

                            QPainterPath circlePath;
                            circlePath.addEllipse(pt, 5.0, 5.0);

                            painter->fillPath(circlePath, QBrush(Qt::black));
                        }
                        else
                        {
                            // Otherwise draw a gray undercrossing line close to the intersection point.
                            // Drawing is performed using two lines, excluding the area close to
                            // the intersection point. This way the drawing is done correctly even though
                            // the connection is above the other connection.
                            QVector2D dir(line1.dx(), line1.dy());
                            dir.normalize();

                            qreal length1 = QVector2D(pt - line1.p1()).length();
                            qreal length2 = QVector2D(pt - line1.p2()).length();

                            qreal width = 3;

                            // If both lines are thick, we have to use a thicker width.
                            if (pen().width() >= 3 && conn->pen().width() >= 3)
                            {
                                ++width;
                            }

                            QPointF seg1Pt1 = (QVector2D(pt) + dir * std::min(length2, width)).toPointF();
                            QPointF seg1Pt2 = (QVector2D(pt) + dir * std::min(length2, (qreal)GridSize)).toPointF();

                            QPointF seg2Pt1 = (QVector2D(pt) - dir * std::min(length1, width)).toPointF();
                            QPointF seg2Pt2 = (QVector2D(pt) - dir * std::min(length1, (qreal)GridSize)).toPointF();

                            painter->setPen(QPen(QColor(160, 160, 160), pen().width() + 1));
                            painter->drawLine(seg1Pt1, seg1Pt2);
                            painter->drawLine(seg2Pt1, seg2Pt2);
                        }
                    }
                }
            }
        }
        else if (dynamic_cast<ComponentItem*>(item) != 0)
        {
            ComponentItem* comp = static_cast<ComponentItem*>(item);

            // Create the line objects for each edge of the diagram component rectangle.
            QLineF leftEdge(comp->rect().topLeft() + comp->scenePos(),
                comp->rect().bottomLeft() + comp->scenePos());

            QLineF rightEdge(comp->rect().topRight() + comp->scenePos(),
                comp->rect().bottomRight() + comp->scenePos());

            QLineF topEdge(comp->rect().topLeft() + comp->scenePos(),
                comp->rect().topRight() + comp->scenePos());

            QLineF bottomEdge(comp->rect().bottomLeft() + comp->scenePos(),
                comp->rect().bottomRight() + comp->scenePos());

            for (int i = 0; i < route1.size() - 1; ++i)
            {
                QLineF line1(route1[i], route1[i + 1]);

                // Check for intersections with the component rectangle's edges.
                QPointF pt, pt2;
                QLineF::IntersectType type1 = line1.intersect(leftEdge, &pt);
                QLineF::IntersectType type2 = line1.intersect(rightEdge, &pt2);

                painter->setPen(QPen(QColor(160, 160, 160), pen().width() + 1));

                // Left intersection.
                // TODO: Requires checking whether the segment is first or last and the endpoint is inside the component.
//                 if (type1 == QLineF::BoundedIntersection)
//                 {
//                     drawLineGap(painter, line1, pt);
//                 }
// 
//                 // Right intersection.
//                 if (type2 == QLineF::BoundedIntersection)
//                 {
//                     drawLineGap(painter, line1, pt2);
//                 }

                // Fill in the whole line segment under the component if the segment goes across the component
                // horizontally.
                if (type1 == QLineF::BoundedIntersection && type2 == QLineF::BoundedIntersection)
                {
                    drawLineGap(painter, line1, pt);
                    drawLineGap(painter, line1, pt2);
                    painter->drawLine(pt, pt2);
                }

                type1 = line1.intersect(topEdge, &pt);
                type2 = line1.intersect(bottomEdge, &pt2);

                // Top intersection.
                if (type1 == QLineF::BoundedIntersection)
                {
                    drawLineGap(painter, line1, pt);
                }

                // Bottom intersection.
                if (type2 == QLineF::BoundedIntersection)
                {
                    drawLineGap(painter, line1, pt2);
                }

                // Fill in the whole line segment under the component if the segment goes across the component
                // vertically.
                if (type1 == QLineF::BoundedIntersection && type2 == QLineF::BoundedIntersection)
                {
                    painter->drawLine(pt, pt2);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setLineWidth()
//-----------------------------------------------------------------------------
void GraphicsConnection::setLineWidth(int width)
{
    QPen newPen = pen();
    newPen.setWidth(width);
    setPen(newPen);
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getConnectionType()
//-----------------------------------------------------------------------------
ConnectionEndpoint::EndpointType GraphicsConnection::getConnectionType() const
{
    if (endpoint1() == 0 && endpoint2() == 0)
    {
        return ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED;
    }

    if (endpoint1()->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED)
    {
        return endpoint1()->getType();
    }
    else
    {
        return endpoint2()->getType();
    }
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getDiagram()
//-----------------------------------------------------------------------------
DesignDiagram* GraphicsConnection::getDiagram()
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::getRoutingMode()
//-----------------------------------------------------------------------------
GraphicsConnection::RoutingMode GraphicsConnection::getRoutingMode() const
{
    return routingMode_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setImported()
//-----------------------------------------------------------------------------
void GraphicsConnection::setImported(bool imported)
{
    imported_ = imported;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::isImported()
//-----------------------------------------------------------------------------
bool GraphicsConnection::isImported() const
{
    return imported_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::validate()
//-----------------------------------------------------------------------------
void GraphicsConnection::validate()
{
    invalid_ = !endpoint1_->isConnectionValid(endpoint2_) || !endpoint2_->isConnectionValid(endpoint1_);
    setDefaultColor();
    endpoint1_->updateInterface();
    endpoint2_->updateInterface();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setEndpoint1()
//-----------------------------------------------------------------------------
void GraphicsConnection::setEndpoint1(ConnectionEndpoint* endpoint1)
{
    if (endpoint1_ != 0)
    {
        // Disconnect from the previous endpoint.
        endpoint1_->removeConnection(this);
        endpoint1_->onDisconnect(endpoint2_);
    }

    // Connect to the new endpoint.
    endpoint1_ = endpoint1;
    endpoint1_->onConnect(endpoint2_);

    validate();

    endpoint1->addConnection(this);

    updatePosition();
    name_ = createDefaultName();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::setEndpoint2()
//-----------------------------------------------------------------------------
void GraphicsConnection::setEndpoint2(ConnectionEndpoint* endpoint2)
{
    if (endpoint2_ != 0)
    {
        // Disconnect from the previous endpoint.
        endpoint2_->removeConnection(this);
        endpoint2_->onDisconnect(endpoint1_);
    }

    // Connect to the new endpoint.
    endpoint2_ = endpoint2;
    endpoint2_->onConnect(endpoint1_);

    validate();

    endpoint2->addConnection(this);

    updatePosition();
    name_ = createDefaultName();
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::isInvalid()
//-----------------------------------------------------------------------------
bool GraphicsConnection::isInvalid() const
{
    return invalid_;
}

//-----------------------------------------------------------------------------
// Function: GraphicsConnection::hasDefaultName()
//-----------------------------------------------------------------------------
bool GraphicsConnection::hasDefaultName() const
{
    return (createDefaultName() == name_);
}
