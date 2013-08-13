/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.cpp
 *		Project: Kactus 2
 */

#include "registergraphitem.h"
#include "fieldgraphitem.h"
#include <models/field.h>
#include <common/utils.h>
#include "addressblockgraphitem.h"
#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <IPXactWrapper/ComponentEditor/visualization/fieldgapitem.h>

#include <common/graphicsItems/visualizeritem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

RegisterGraphItem::RegisterGraphItem(QSharedPointer<Register> reg,
									 QGraphicsItem* parent):
MemoryVisualizationItem(parent),
register_(reg) {

	Q_ASSERT(register_);
	QBrush brush(KactusColors::REGISTER_COLOR);
	setBrush(brush);
	ExpandableItem::setExpansionBrush(brush);

    setOverlappingTop(reg->getMSB());
    setOverlappingBottom(reg->getOffset());
}

RegisterGraphItem::~RegisterGraphItem() {
}

void RegisterGraphItem::refresh() {
	setName(register_->getName());
	
	// get the base of the parent address block
	quint64 base = 0;
	MemoryVisualizationItem* addrBlock = dynamic_cast<MemoryVisualizationItem*>(parentItem());
	if (addrBlock) {
		base = addrBlock->getOffset();
	}

	// get the offset of the register
	quint64 offset = Utils::str2Uint(register_->getAddressOffset());
	// calculate the start address of the register
	quint64 startAddress = base + offset;
	// show the start address of the register
	setLeftTopCorner(startAddress);

	// show the end address of the register
	unsigned int addrUnit = getAddressUnitSize();
	// prevent division by zero
	if (addrUnit == 0) {
		addrUnit = 1;
	}	
	unsigned int addrUnits = register_->getSize() / addrUnit;
	// Round upwards. Needed for unorthodox regsize, e.g. 30b/8=3 becomes 4 (Bytes)
	if (addrUnits * addrUnit < register_->getSize()) {
		addrUnits++;
	}

	quint64 endAddress = base + offset + addrUnits -1;
    
    setOverlappingTop(base);
    setOverlappingBottom(endAddress);
    
	// set the positions for the children
	reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 RegisterGraphItem::getOffset() const {
	Q_ASSERT(register_);
	
	// the register offset from the address block
	QString offsetStr = register_->getAddressOffset();
	quint64 regOffset = Utils::str2Uint(offsetStr);

	// the address block's offset
	AddressBlockGraphItem* blockItem = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(blockItem);
	quint64 blockOffset = blockItem->getOffset();

	// the total offset is the address block's offset added with register's offset
	return blockOffset + regOffset;
}

int RegisterGraphItem::getBitWidth() const {
	Q_ASSERT(register_);
	return register_->getWidth();
}

void RegisterGraphItem::reorganizeChildren() {

	// remove the gaps and update offsets of fields
	updateChildMap();

	// first find out the width for all items
	qreal width = itemTotalWidth();

	// if there are no children then this can not be expanded or collapsed
	if (childItems_.isEmpty()) {
		ExpandableItem::setShowExpandableItem(false);
	}
	// if there are children then display the expand/collapse i.value()
	else {
		ExpandableItem::setShowExpandableItem(true);
	}

	// save the new gap child items to be added to map
	QList<FieldGapItem*> gaps;

	// the offset where the previous block starts
	quint64 previousBlockStart = register_->getMSB();

    FieldGapItem* prevConflict = 0;

	qreal xCoordinate = rect().left();
	MemoryVisualizationItem* previous = NULL;
	for (QMap<quint64, MemoryVisualizationItem*>::iterator i = --childItems_.end();
		i != --childItems_.begin(); --i) { 

		Q_ASSERT(i.value());
        i.value()->setNotConflicted();

		// pointer to the possible gap to add between two fields
		FieldGapItem* gap = 0;

		// if there is a gap between the last block and this block
		if (previousBlockStart > i.value()->getLastAddress() + 1 ||
            previous == 0 && previousBlockStart == i.value()->getLastAddress() + 1) {

			// create the gap i.value()
			gap = new FieldGapItem(this);

			// set the first address of the gap
			gap->setStartAddress(i.value()->getLastAddress(), false);

			// if the gap is not at the start 
			if (previous) {
				// set the last address for the gap
				gap->setEndAddress(previousBlockStart, false);
			}
			else {
				// the gap starts from the end of the parent
				gap->setEndAddress(previousBlockStart, true);
			}			

			// set the gap to the end of the last i.value()
			gap->setPos(xCoordinate, rect().bottom());

			// update the x-coordinate to avoid setting a block under the gap
			xCoordinate += gap->rect().width();

			gap->setVisible(isExpanded());

			gaps.append(gap);
		}

        // if the block and the next block are overlapping.
        else if (previous && i.value()->getLastAddress() >= previousBlockStart )
        {

            // This and previous block overlap completely.
            if (i.value()->getOffset() == previous->getOffset() &&
                i.value()->getLastAddress() == previous->getLastAddress())
            {
                previous->setConflicted();
                i.value()->setConflicted();
            }
            // This block is completely inside the previous block.
            else if (i.value()->getOffset() >= previous->getOffset() &&
                i.value()->getLastAddress() <= previous->getLastAddress() )
            {
                i.value()->setConflicted();
                previous->setConflicted();
            }

            // The previous block is completely inside this block.
            else if (previous->getOffset() >= i.value()->getOffset() &&
                previous->getLastAddress() <= i.value()->getLastAddress())
            {
                i.value()->setConflicted();
                previous->setConflicted();
            }          

            // Partial overlapping.
            else
            {                
                gap = new FieldGapItem("conflicted", this);                
                gap->setConflicted();
                gap->setStartAddress(previous->getOffset(),true);
                gap->setEndAddress(i.value()->getLastAddress(),true);     

                // set the conflicted gap to the end of the last item.
                gap->setPos(xCoordinate, rect().bottom());

                // update the y-coordinate to avoid setting a block under the gap.
			    xCoordinate += gap->rect().width();

                gap->setVisible(isExpanded());

                gaps.append(gap);

                
                // Update display of the last address on this block.
                i.value()->setOverlappingTop(previousBlockStart - 1);

                // If previous block is completely overlapped by the preceding block and this block.
                if (prevConflict && prevConflict->getOffset() + 1 >= i.value()->getLastAddress())
                {
                    previous->setCompleteOverlap();
                }
                // Update display of last address on the previous block.
                else
                {
                    previous->setOverlappingBottom(i.value()->getLastAddress() + 1);
                }


                prevConflict = gap;
            }
        }

		if (previous) {
			QRectF previousRect;

			// if there was a gap
			if (gap) {

				// find the position where previous block ended
				previousRect = mapRectFromItem(previous, previous->itemTotalRect());

				// set the gap to be under the previous block
				gap->setPos(previousRect.right(), rect().bottom());

				// update the previous rect to the end of the gap
				previousRect = mapRectFromItem(gap, gap->itemTotalRect());	
			}
			// if there was no gap then use the previous defined block
			else {
				// set the next i.value() to start after the previous
				previousRect = mapRectFromItem(previous, previous->itemTotalRect());
			}

			xCoordinate = previousRect.right();
		}

		// update the last address of the block
		previousBlockStart = i.value()->getOffset();
		i.value()->setPos(xCoordinate, rect().bottom());
		previous = i.value();
	}

	// if the LSB bit(s) does not belong to any field
	if (previous && previous->getOffset() > 0) {
		// create the gap 
		FieldGapItem* gap = new FieldGapItem(this);

		// the gap starts from the start of the register
		gap->setStartAddress(0, true);

		// the gap ends to the start of the defined field
		gap->setEndAddress(previous->getOffset(), false);

		// increase the x-coordinate to avoid setting the next field on top of the gap
		xCoordinate += previous->rect().width();

		// gap is positioned after the last field
		gap->setPos(xCoordinate, rect().bottom());

		gap->setVisible(isExpanded());

		gaps.append(gap);
	}

	// add the gaps to the child items
	foreach (MemoryGapItem* gap, gaps) {
		childItems_.insert(gap->getOffset(), gap);
	}

	// update the width of this i.value() to match the width of all
	setRect(0, 0, width, VisualizerItem::ITEM_HEIGHT);

	// reorganize the text blocks of this register
	ExpandableItem::reorganizeChildren();
}

unsigned int RegisterGraphItem::getAddressUnitSize() const {
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);
	return addrBlock->getAddressUnitSize();
}

quint64 RegisterGraphItem::getLastAddress() const {
	AddressBlockGraphItem* addrBlock = static_cast<AddressBlockGraphItem*>(parentItem());
	Q_ASSERT(addrBlock);

	// how many bits one address unit takes
	unsigned int addrUnit = addrBlock->getAddressUnitSize();

	// prevent division by zero
	if (addrUnit == 0) {
		addrUnit = 1;
	}

	// the offset of the start of register
	quint64 offset = getOffset();

	// how many address unit are contained in the register
	unsigned int size = register_->getWidth() / addrUnit;
	if (size*addrUnit < register_->getWidth()) {
		size++; //Round truncated number upwards
	}
	// if size is not defined then never return a negative number
	if (size == 0) {
		return 0;
	}

	// the last address contained in the register
	return offset + size - 1;
}

qreal RegisterGraphItem::itemTotalWidth() const {
	
	// combine the width of the child items
	qreal width = 0;
	foreach (MemoryVisualizationItem* child, childItems_) {
		if (child->isVisible()) {
			width += child->itemTotalWidth();
		}
	}

	// if there were no children then return the default width
	if (width < VisualizerItem::MAX_WIDTH) {
		return VisualizerItem::MAX_WIDTH;
	}
	
	// return the combined width of the children
	return width;
}
