//-----------------------------------------------------------------------------
// File: DiagramAdHocPort.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 9.2.2012
//
// Description:
// Diagram graphics item for ad-hoc ports.
//-----------------------------------------------------------------------------

#ifndef DIAGRAMADHOCPORT_H
#define DIAGRAMADHOCPORT_H

#include <QSharedPointer>
#include <QVector2D>

#include "DiagramConnectionEndpoint.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

class DiagramInterconnection;
class DiagramComponent;
class DiagramOffPageConnector;
class LibraryInterface;
class Port;

//-----------------------------------------------------------------------------
//! DiagramAdHocPort class.
//-----------------------------------------------------------------------------
class DiagramAdHocPort : public DiagramConnectionEndPoint
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_ADHOC_PORT };

    /*!
     *  Constructor.
     */
    DiagramAdHocPort(Port* port, LibraryInterface* lh, QGraphicsItem* parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~DiagramAdHocPort();

    /*!
     *  Sets the port temporary or not temporary. Temporary port set its bus interface undefined
     *  automatically if the connections are removed.
     *
     *      @param [in] temp True if temporary; false if not temporary.
     */
    void setTemporary(bool temp);

    /*!
     *  Sets the bus and abstraction types and the interface mode for the end point.
     *
     *      @param [in] busType  The bus type (bus definition VLNV).
     *      @param [in] absType  The abstraction type (abstraction definition VLNV).
     *      @param [in] mode     The interface mode.
     */
    void setTypes(VLNV const& busType, VLNV const& absType, General::InterfaceMode mode);

    /*!
     *  Updates the graphics to match the IP-XACT port.
     */
    void updateInterface();

	int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // DiagramConnectionEndPoint implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Returns true if the draw direction is fixed and thus, cannot be changed.
     */
    virtual bool isDirectionFixed() const;

    /*!
     *  Returns the name of the ad-hoc port.
     */
    virtual QString name() const;

	/*!
     *  Sets the name of the ad-hoc port.
     *
     *      @param [in] name The name to set.
     */
	virtual void setName(const QString& name);

	/*!
     *  Returns the description of the port.
     */
	virtual QString description() const;

	/*!
     *  Sets the description of the port.
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
    virtual bool onConnect(DiagramConnectionEndPoint const* other);


    /*!
     *  Called when a connection has been removed from between this and another end point.
     *
     *      @param [in] other The other end point of the connection.
     */
    virtual void onDisconnect(DiagramConnectionEndPoint const* other);

    /*! 
     *  Returns true if this port can be connected to the given end point.
     *
     *      @param [in] other The end point to which to connect.
     */
    virtual bool canConnect(DiagramConnectionEndPoint const* other) const;

    /*! 
     *  Returns the encompassing port, if this DiagramComponent represents
     *  a bus interface on a component
     */
    virtual DiagramComponent *encompassingComp() const;

	/*!
     *  Returns a pointer to the top component that owns this interface
	 */
	virtual QSharedPointer<Component> ownerComponent() const;

    /*! 
     *  Returns the IP-XACT bus interface model of the port.
     */
    virtual QSharedPointer<BusInterface> getBusInterface() const;

    /*!
     *  Returns the ad-hoc port of the end point.
     *
     *      @remarks The function returns a null pointer if the end point is a bus interface.
     *               Use isBus() function to check for ad-hoc support (isBus() == false).
     */
    virtual Port* getPort() const;

    /*! 
     *  Returns true if the port represents a hierarchical connection.
     */
    virtual bool isHierarchical() const;

    /*!
     *  Returns true if the end point is a bus interface end point.
     */
    virtual bool isBus() const;

	/*!
     *  Sets the interface mode for the port.
     *
     *      @param [in] mode The mode to set.
     */
	virtual void setInterfaceMode(General::InterfaceMode mode);

    /*!
     *  Returns the corresponding off-page connector or a null pointer if the end point does not have one.
     */
    virtual DiagramConnectionEndPoint* getOffPageConnector();

protected:
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QGraphicsTextItem *nameLabel_;
    Port* port_;
    LibraryInterface* lh_;

    //! Boolean flag for determining if the port is temporary or not.
    bool temp_;

    //! The position of the port before mouse move.
    QPointF oldPos_;

    //! The old positions of the other component ports before mouse move.
    QMap<DiagramConnectionEndPoint*, QPointF> oldPortPositions_;

    //! The off-page connector.
    DiagramOffPageConnector* offPageConnector_;
};

//-----------------------------------------------------------------------------

#endif // DIAGRAMADHOCPORT_H
