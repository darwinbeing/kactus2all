//-----------------------------------------------------------------------------
// File: SystemColumn.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 31.5.2011
//
// Description:
// System column class for managing SW mapping components in a system design.
//-----------------------------------------------------------------------------

#ifndef SYSTEMCOLUMN_H
#define SYSTEMCOLUMN_H

#include "IGraphicsItemStack.h"

#include <common/ColumnTypes.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

#include <QGraphicsRectItem>
#include <QSet>

class SystemColumnLayout;
class SWConnection;

//-----------------------------------------------------------------------------
//! SystemColumn class.
//-----------------------------------------------------------------------------
class SystemColumn : public QObject, public QGraphicsRectItem, public IGraphicsItemStack
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_SYSTEM_COLUMN };

    enum
    {
        HEIGHT = 30
    };

    /*!
     *  Constructor.
     *
     *      @param [in] name          The column description.
     *      @param [in] layout        The parent column layout.
     *      @param [in] scene         The graphics scene.
     */
    SystemColumn(ColumnDesc const& desc, SystemColumnLayout* layout, QGraphicsScene* scene);

    /*!
     *  Destructor.
     */
    virtual ~SystemColumn();

    /*!
     *  Sets the name of the system column.
     *
     *      @param [in] name The name of the column.
     */
    void setName(QString const& name);

    /*!
     *  Sets the column description.
     */
    void setColumnDesc(ColumnDesc const& desc);

    /*!
     *  Sets the y coordinate offset.
     *
     *      @param [in] y The y coordinate offset.
     */
    void setOffsetY(qreal y);

    /*!
     *  Returns the name of the system column.
     */
    QString const& getName() const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

    /*!
     *  Returns the column description.
     */
    ColumnDesc const& getColumnDesc() const;

    /*!
     *  Returns the parent layout.
     */
    SystemColumnLayout& getLayout();

    /*!
     *  Returns true if the column is empty (i.e. not containing any items).
     */
    bool isEmpty() const;

    int type() const { return Type; }

    //-----------------------------------------------------------------------------
    // IGraphicsItemStack implementation.
    //-----------------------------------------------------------------------------

    /*!
     *  Adds an item to the system column.
     *
     *      @param [in] item  The item to add.
     *      @param [in] load  If true, the item is being loaded from a design.
     */
    void addItem(QGraphicsItem* item, bool load = false);

    /*!
     *  Removes an item from the system column.
     *
     *      @param [in] item the item to remove.
     */
    void removeItem(QGraphicsItem* item);

    /*!
     *  Called when an item is moved within the column.
     *
     *      @param [in] item       The item that has been moved.
     */
    void onMoveItem(QGraphicsItem* item);

    /*!
     *  Called when an item is released from being moved by mouse.
     *
     *      @param [in] item The item that has been released.
     */
    void onReleaseItem(QGraphicsItem* item);

    /*!
     *  Updates the item positions so that there are no violations of the stacking rule.
     */
    void updateItemPositions();

    /*!
     *  Maps the given local position to scene coordinates.
     */
    QPointF mapStackToScene(QPointF const& pos) const;

    /*!
     *  Maps the given scene position to local coordinates.
     */
    QPointF mapStackFromScene(QPointF const& pos) const;

    /*!
     *  Returns true if the stack is allowed to contain the given item.
     *
     *      @param [in] item The item to test for.
     */
    bool isItemAllowed(QGraphicsItem* item) const;

signals:
    //! Signals that the contents of the column have changed.
    void contentChanged();

protected:
    //! Called when the user presses the mouse over the column.
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user moves the column with the mouse.
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

    //! Called when the user release the mouse.
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
    // Disable copying.
    SystemColumn(SystemColumn const& rhs);
    SystemColumn& operator=(SystemColumn const& rhs);

    /*!
     *  Updates the name label.
     */
    void updateNameLabel();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        MIN_Y_PLACEMENT = 60,
        SPACING = 30,
        IO_SPACING = 18,
    };

    //! The parent column layout.
    SystemColumnLayout* layout_;

    //! The column description.
    ColumnDesc desc_;

    //! The column name label.
    QGraphicsTextItem* nameLabel_;

    //! The node items ordered from top to bottom.
    QList<QGraphicsItem*> items_;

    //! The old position of the column before mouse move.
    QPointF oldPos_;

    //! The connections that need to be also stored for undo.
    QSet<SWConnection*> conns_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMCOLUMN_H
