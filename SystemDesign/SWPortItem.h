//-----------------------------------------------------------------------------
// File: SWPortItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 2.5.2012
//
// Description:
// Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------

#ifndef SWPORTITEM_H
#define SWPORTITEM_H

#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QSharedPointer>
#include <QVector2D>
#include <QPolygonF>

#include "SWConnectionEndpoint.h"

//-----------------------------------------------------------------------------
//! Graphics item for visualizing API/COM interfaces as ports of a component.
//-----------------------------------------------------------------------------
class SWPortItem : public SWConnectionEndpoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SW_PORT_ITEM };

    /*!
     *  Constructor which creates a graphics item for an API interface.
     */
    SWPortItem(QSharedPointer<ApiInterface> apiIf, QGraphicsItem *parent = 0);

    /*!
     *  Constructor which creates a graphics item for a COM interface.
     */
    SWPortItem(QSharedPointer<ComInterface> comIf, QGraphicsItem *parent = 0);

    /*!
     *  Destructor.
     */
	virtual ~SWPortItem();

    /*!
     *  Sets the port temporary or not temporary. Temporary port set its interfaces undefined
     *  automatically if the connections are removed.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp);

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // SWConnectionEndpoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Updates the endpoint interface.
     */
    virtual void updateInterface();

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*! \brief Returns the name of this port
     *
     */
    virtual QString name() const;

	/*!
     *  Sets the name for the port.
	 *
	 *      @param [in] name The name to set for the port.
	 */
	virtual void setName(const QString& name);

	/*!
     *  Returns the description of the port.
	 */
	virtual QString description() const;

	/*! 
     *  Sets the description for the port.
	 *
	 *      @param [in] description The description to set.
	 */
	virtual void setDescription(const QString& description);

    /*!
     *  Called when a connection between this and another end point is done.
     *
     *      @param [in] other The other end point of the connection.
     *
     *      @return False if there was an error in the connection. Otherwise true.
     */
    virtual bool onConnect(SWConnectionEndpoint const* other);

    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(SWConnectionEndpoint const* other);

    /*! 
     *  Returns true if this port can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(SWConnectionEndpoint const* other) const;

    /*! 
     *  Returns the encompassing component.
     */
    virtual SWComponentItem* encompassingComp() const;

	/*!
     *  Returns a pointer to the top component that owns this interface.
	 */
	virtual QSharedPointer<Component> getOwnerComponent() const;

    /*! 
     *  Returns the COM interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a COM interface.
     *               Use isCom() function to check for COM interface support.
     */
    virtual QSharedPointer<ComInterface> getComInterface() const;

    /*! 
     *  Returns the API interface model of the endpoint.
     *
     *      @remarks The function returns a null pointer if the endpoint is not a API interface.
     *               Use isApi() function to check for API interface support.
     */
    virtual QSharedPointer<ApiInterface> getApiInterface() const;

    /*!
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

protected:
    virtual QVariant itemChange(GraphicsItemChange change, QVariant const& value);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    void initialize();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! The COM interface, or null if not set.
    QSharedPointer<ComInterface> comInterface_;

    //! The API interface, or null if not set.
    QSharedPointer<ApiInterface> apiInterface_;

    //! Boolean flag for determining if the port is temporary or not.
    bool temp_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<SWPortItem*, QPointF> oldPortPositions_;
};

//-----------------------------------------------------------------------------

#endif // SWPORTITEM_H
