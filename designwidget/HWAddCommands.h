//-----------------------------------------------------------------------------
// File: HWAddCommands.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 3.8.2011
//
// Description:
// Undo add commands for the design diagram.
//-----------------------------------------------------------------------------

#ifndef HWADDCOMMANDS_H
#define HWADDCOMMANDS_H

#include <models/generaldeclarations.h>

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QObject>

class HWConnection;
class BusPortItem;
class HWComponentItem;
class BusInterfaceItem;
class GraphicsColumn;
class GraphicsColumnLayout;
class ComponentItem;
class IGraphicsItemStack;
class Component;
class Port;
//-----------------------------------------------------------------------------
//! PortAddCommand class.
//-----------------------------------------------------------------------------
class PortAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to add a port.
     *      @param [in] pos        The position where to add the port.
     *      @param [in] parent     The parent command.
     */
    PortAddCommand(HWComponentItem* component, QPointF const& pos, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    PortAddCommand(PortAddCommand const& rhs);
    PortAddCommand& operator=(PortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The item.
    HWComponentItem* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    BusPortItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! ConnectionAddCommand class.
//-----------------------------------------------------------------------------
class ConnectionAddCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] scene  The scene.
     *      @param [in] conn   The interconnection to add.
     *      @param [in] parent The parent command.
     */
    ConnectionAddCommand(QGraphicsScene* scene, HWConnection* conn,
                         QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~ConnectionAddCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    ConnectionAddCommand(ConnectionAddCommand const& rhs);
    ConnectionAddCommand& operator=(ConnectionAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interconnection.
    HWConnection* conn_;

    //! The interface modes for the endpoints.
    General::InterfaceMode mode1_;
    General::InterfaceMode mode2_;

    //! The port maps for the hierarchical end point (if any).
    QList< QSharedPointer<General::PortMap> > portMaps_;

    //! The graphics scene.
    QGraphicsScene* scene_;

    //! Boolean flag for indicating if the connection should be deleted in the destructor.
    bool del_;

    //! If true, port copy operation is a part of this undo command.
    bool portsCopied_;
};

//-----------------------------------------------------------------------------
//! PortPasteCommand class.
//-----------------------------------------------------------------------------
class PortPasteCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
	 *      Creates the child commands for adding physical ports to the component model. 
	 *
     *      @param [in] destComponent  The component to which to copy a port.
     *      @param [in] srcComponent   The component from which the port is copied.
     *      @param [in] pos            The position where to copy the port.
	 *      @param [in] port           The port to paste.
     *      @param [in] parent         The parent command.
     */
    PortPasteCommand(HWComponentItem* destComponent,  QSharedPointer<Component> srcComponent, 
		QPointF const& pos, BusPortItem* port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~PortPasteCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
    PortPasteCommand(PortAddCommand const& rhs);
    PortPasteCommand& operator=(PortAddCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target item.
    HWComponentItem* component_;

    //! The port position.
    QPointF pos_;

    //! The diagram port.
    BusPortItem* port_;

    //! The graphics scene.
    QGraphicsScene* scene_;

	//! Boolean flag for indicating if the port should be deleted in the destructor.
    bool del_;
};

//-----------------------------------------------------------------------------
//! AddPhysicalPortCommand class.
//-----------------------------------------------------------------------------
class AddPhysicalPortCommand : public QUndoCommand
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which to copy a physical port.
	 *      @param [in] port       The port to paste.
     *      @param [in] parent     The parent command.
     */
   AddPhysicalPortCommand(QSharedPointer<Component>, QSharedPointer<Port> port, QUndoCommand* parent = 0);

    /*!
     *  Destructor.
     */
    ~AddPhysicalPortCommand();

    /*!
     *  Undoes the command.
     */
    virtual void undo();

    /*!
     *  Redoes the command.
     */
    virtual void redo();

private:
    // Disable copying.
   AddPhysicalPortCommand(AddPhysicalPortCommand const& rhs);
   AddPhysicalPortCommand& operator=(AddPhysicalPortCommand const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The target component model.
    QSharedPointer<Component> component_;

    //! The port to paste.
	QSharedPointer<Port> port_;
};

//-----------------------------------------------------------------------------

#endif // HWADDCOMMANDS_H
