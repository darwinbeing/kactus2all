/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapgraphitem.cpp
 *		Project: Kactus 2
 */

#include "memorymapgraphitem.h"
#include "addressblockgraphitem.h"

#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <IPXactWrapper/ComponentEditor/addressSpaces/addressSpaceVisualizer/addressspacevisualizationitem.h>
#include <common/KactusColors.h>

#include <QBrush>
#include <QColor>

#include <QDebug>

MemoryMapGraphItem::MemoryMapGraphItem(QSharedPointer<MemoryMap> memoryMap,
									   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
memoryMap_(memoryMap) {

	Q_ASSERT(memoryMap_);
	QBrush brush(KactusColors::MEM_MAP_COLOR);
	setDefaultBrush(brush);
}

MemoryMapGraphItem::~MemoryMapGraphItem() {
}

void MemoryMapGraphItem::refresh() {
	
	setName(memoryMap_->getName());
    setOverlappingTop(memoryMap_->getFirstAddress());
    setOverlappingBottom(memoryMap_->getLastAddress());

    // Set tooltip to show addresses in hexadecimals.
    setToolTip("<b>Name: </b>" + memoryMap_->getName() + "<br>" +
        "<b>AUB: </b>" + QString::number(memoryMap_->getAddressUnitBits()) + "<br>" +
        "<b>First address: </b>" + AddressSpaceVisualizationItem::addr2Str(getOffset(),getBitWidth()) + "<br>" +
        "<b>Last address: </b>" + AddressSpaceVisualizationItem::addr2Str(memoryMap_->getLastAddress(), 
        getBitWidth()));
  
	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();

	MemoryMapScene* memScene = static_cast<MemoryMapScene*>(scene());
	Q_ASSERT(memScene);
	memScene->rePosition();
}

quint64 MemoryMapGraphItem::getOffset() const {
	return memoryMap_->getFirstAddress();
}

int MemoryMapGraphItem::getBitWidth() const {
	return memoryMap_->getMaxWidth();
}

unsigned int MemoryMapGraphItem::getAddressUnitSize() const {
	return memoryMap_->getAddressUnitBits();
}

quint64 MemoryMapGraphItem::getLastAddress() const {
	return memoryMap_->getLastAddress();
}

qreal MemoryMapGraphItem::itemTotalWidth() const {

    return VisualizerItem::DEFAULT_WIDTH;

    /*
	qreal width = VisualizerItem::DEFAULT_WIDTH;

	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {

		// The larger width
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			width = qMax(width, childItem->itemTotalWidth());
		}
	}
	return width;*/
}

void MemoryMapGraphItem::setWidth( qreal width ) {
    setRect(0, 0, width, VisualizerItem::DEFAULT_HEIGHT);

    childWidth_ = width - MemoryVisualizationItem::CHILD_INDENTATION;

    MemoryVisualizationItem::reorganizeChildren();
}
