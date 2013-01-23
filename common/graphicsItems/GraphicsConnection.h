//-----------------------------------------------------------------------------
// File: GraphicsConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.6.2012
//
// Description:
// Base class for graphical connections.
//-----------------------------------------------------------------------------

#ifndef GRAPHICSCONNECTION_H
#define GRAPHICSCONNECTION_H

#include <common/graphicsItems/ConnectionEndpoint.h>

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

class DesignDiagram;

//-----------------------------------------------------------------------------
//! Base class for graphical connections.
//-----------------------------------------------------------------------------
class GraphicsConnection : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! RoutingMode enumeration.
    //-----------------------------------------------------------------------------
    enum RoutingMode
    {
        ROUTING_MODE_NORMAL = 0,  //!< The connection uses rectlinear routing.
        ROUTING_MODE_OFFPAGE      //!< The connection goes straight from the location of the first
                                  //!< endpoint to the location of the second endpoint.
    };

    /*!
     *  Constructor.
     */
    GraphicsConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                       bool autoConnect, QString const& name, 
                       QString const& displayName, QString const& description,
                       DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended connection.
     */
    GraphicsConnection(QPointF const& p1, QVector2D const& dir1,
                       QPointF const& p2, QVector2D const& dir2,
                       QString const& displayName, QString const& description,
                       DesignDiagram* parent);

    /*!
     *  Destructor.
     */
    virtual ~GraphicsConnection();

    /*!
     *  Sets the routing mode.
     *
     *      @param [in] mode The routing mode.
     */
    void setRoutingMode(RoutingMode mode);

    /*!
     *  Sets the line width.
     *
     *      @param [in] width The line width in pixels.
     */
    void setLineWidth(int width);

    /*! 
     *  Connects the ends of the connection.
     *
     *      @remarks There must be valid endpoints below the start and end positions
     *               of the connection.
     */
    virtual bool connectEnds();

    /*!
     *  Sets the first endpoint. If the connection is already connected, the connection
     *  is disconnected from the old endpoint and connected to the new given endpoint, updating the
     *  route too.
     *
     *      @param [in] endpoint1 The first endpoint to set.
     */
    void setEndpoint1(ConnectionEndpoint* endpoint1);

    /*!
     *  Sets the first endpoint. If the connection is already connected, the connection
     *  is disconnected from the old endpoint and connected to the new given endpoint, updating the
     *  route too.
     *
     *      @param [in] endpoint2 The second endpoint to set.
     */
    void setEndpoint2(ConnectionEndpoint* endpoint2);

    /*!
     *  Validates the connection and draws it in red if it is not valid.
     */
    void validate();

    /*! 
     *  Disconnects the ends of the connection.
     */
    void disconnectEnds();

    /*!
     *  Sets the routing of the connection.
     *
     *      @param [in] path The route to set.
     */
    virtual void setRoute(QList<QPointF> path);

    /*!
     *  Returns the route of this connection.
     */
    QList<QPointF> const& route() const;

    /*! 
     *  Updates the end positions when connected endpoints are moved.
     */
    virtual void updatePosition();

    /*!
     *  Updates the name of the connection according to the default formatting.
     */
    QString createDefaultName() const;

    /*!
     *  Begins the position update of the connection.
     */
    void beginUpdatePosition();

    /*!
     *  Ends the position update of the connection and creates an undo command.
     *
     *      @param [in] parent The parent command.
     *
     *      @return The created (child) command.
     */
    QUndoCommand* endUpdatePosition(QUndoCommand* parent);

    /*!
     *  Set the name for the connection.
	 *
	 *      @param [in] name The name to set for the connection.
	 */
	void setName(QString const& name);

    /*!
     *  Sets the description for the connection.
	 *
     *      @param [in] description The description to set.
	 */
	void setDescription(const QString& description);

    /*!
     *  Sets the imported state.
     *
     *      @param [in] imported If true, the connection is an imported one.
     */
    void setImported(bool imported);

    /*!
     *  Returns the name of this connection
     */
    QString const& name() const;

	/*!
     *  Returns the description of the connection.
	 */
	QString const& description() const;

    /*!
     *  Returns true if the connection is an imported one.
     */
    bool isImported() const;

	/*!
     *  Returns the first endpoint connected.
     */
    ConnectionEndpoint* endpoint1() const;

    /*!
     *  Return the second endpoint connected.
     */
    ConnectionEndpoint* endpoint2() const;

    /*!
     *  Returns the parent diagram.
     */
    DesignDiagram* getDiagram();

    /*!
     *  Returns the type of the connection.
     */
    ConnectionEndpoint::EndpointType getConnectionType() const;

    /*!
     *  Returns the used routing mode.
     */
    RoutingMode getRoutingMode() const;

    /*!
     *  Returns true if the connection is invalid.
     */
    bool isInvalid() const;

    /*!
     *  Returns true if the connection uses the default name.
     */
    bool hasDefaultName() const;

signals:
    //! Signals that the connection has changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

    //! \brief This signal is emitted when this connection is destroyed.
    void destroyed(GraphicsConnection* connection);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    //-----------------------------------------------------------------------------
    //! Selection type enumeration.
    //-----------------------------------------------------------------------------
    enum SelectionType
    {
        END,
        SEGMENT,
        NONE
    };

    //! Minimum length for a line segment.
    static float const MIN_LENGTH;
    static float const MIN_START_LENGTH;

    /*!
     *  Sets the default graphics item settings.
     */
    void setItemSettings();

    /*!
     *  Creates an optimal route from point p1 to point p2, considering the given start and exit directions.
     */
    void createRoute(QPointF p1, QPointF p2, QVector2D const& dir1, QVector2D const& dir2);

    /*!
     *  Creates an optimal route between the given endpoints.
     */
    void createRoute(ConnectionEndpoint* endpoint1, ConnectionEndpoint* endpoint2);

    /*!
     *  Simplifies the path by removing parallel consecutive segments.
     */
    void simplifyPath();

    /*!
     *  Sets the default color based on the routing mode.
     */
    void setDefaultColor();

    /*!
     *  Draws specific helper graphics for overlapping graphics items.
     *
     *      @param [in] painter The painter.
     */
    void drawOverlapGraphics(QPainter* painter);

    /*!
     *  Draws a "gap" to a line with the currently selected pen.
     *
     *      @param [in] painter The painter.
     *      @param [in] line    The line to which to draw the gap.
     *      @param [in] pt      The intersection point where to draw the gap.
     */
    void drawLineGap(QPainter* painter, QLineF const& line, QPointF const& pt);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent diagram.
    DesignDiagram* parent_;

    //! The name of the connection.
    QString name_;

    //! The description of the connection.
    QString description_;

    //! The first endpoint.
    ConnectionEndpoint* endpoint1_;

    //! The second endpoint.
    ConnectionEndpoint* endpoint2_;

    //! The route path points.
    QList<QPointF> pathPoints_;

    //! The index of the segment that is currently selected.
    int selected_;

    //! The type of selection.
    SelectionType selectionType_;

    //! The old route for creating undo commands.
    QList<QPointF> oldRoute_;

    //! The routing mode.
    RoutingMode routingMode_;

    //! If true, the connection is an imported one.
    bool imported_;

    //! The default color.
    bool invalid_;
};

//-----------------------------------------------------------------------------

#endif // GRAPHICSCONNECTION_H
