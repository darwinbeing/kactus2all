//-----------------------------------------------------------------------------
// File: MemoryItem.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M��tt�
// Date: 17.9.2012
//
// Description:
// Declares the memory map item class.
//-----------------------------------------------------------------------------

#ifndef MEMORYITEM_H
#define MEMORYITEM_H

#include <common/diagramgrid.h>
#include <common/graphicsItems/GraphicsItemTypes.h>
#include <common/graphicsItems/IGraphicsItemStack.h>
#include <common/layouts/IVGraphicsLayout.h>

#include <QGraphicsRectItem>
#include <QSharedPointer>

class AddressBlock;
class Component;
class MemoryMap;
class LibraryInterface;
class IGraphicsItemStack;
class AddressSectionItem;

//-----------------------------------------------------------------------------
//! MemoryItem class.
//-----------------------------------------------------------------------------
class MemoryItem : public QObject, public QGraphicsRectItem, public IGraphicsItemStack
{
    Q_OBJECT 

public:
    enum { Type = GFX_TYPE_MEMORY_ITEM };

    /*!
     *  Constructor.
     *  
     *      @param [in] libInterface    The library interface.
     *      @param [in] component       The component.
     *      @param [in] memoryMap       The memory map.
     *      @param [in] parent          The parent graphics item.
     */
    MemoryItem(LibraryInterface* libInterface, QSharedPointer<Component> component,
                  QSharedPointer<MemoryMap> memoryMap, QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~MemoryItem();

    /*!
     *  Updates the component item to reflect the current state of the component model.
     */
    virtual void updateVisuals();

    /*!
     *  Returns the actual memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap();

    /*!
     *  Returns the actual memory map.
     */
    QSharedPointer<MemoryMap const> getMemoryMap() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface();

    /*!
     *  Returns the parent graphics item stack.
     */
    IGraphicsItemStack* getParentStack();

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

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

signals:
    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage) const;

	//! \brief Emitted right before this item is destroyed.
	void destroyed(MemoryItem* comp);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    /*!
     *  Updates the name label with the given text.
     *
     *      @param [in] text The text to display in the label.
     */
    virtual void updateNameLabel(QString const& text);

    /*!
     *  Gets the height.
     *  
     *      @return The height.
     */
    qreal getHeight() const;

    /*!
     *  Updates the size.
     */
    void updateSize();

private:
    // Disable copying.
    MemoryItem(MemoryItem const& rhs);
    MemoryItem& operator=(MemoryItem const& rhs);

    enum
    {
        WIDTH = 280,
        NAME_COLUMN_WIDTH = 50,
        MIN_HEIGHT = 120,
        SPACING = 10,
        SECTION_X = NAME_COLUMN_WIDTH / 2
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* libInterface_;

    //! The component which contains the memory map.
    QSharedPointer<Component> component_;

    //! The memory map.
    QSharedPointer<MemoryMap> memoryMap_;

    //! The name label.
    QGraphicsTextItem* nameLabel_;

    //! AUB label.
    QGraphicsTextItem* aubLabel_;

    //! The layout for the sections.
    QSharedPointer< IVGraphicsLayout<AddressSectionItem> > sectionLayout_;

    //! The address sections for the address blocks.
    QList<AddressSectionItem*> sections_;
};

//-----------------------------------------------------------------------------

#endif // MEMORYITEM_H
