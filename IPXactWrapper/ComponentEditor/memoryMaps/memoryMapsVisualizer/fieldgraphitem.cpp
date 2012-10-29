/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"

#include <QBrush>
#include <QColor>

FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field,
							   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
field_(field) {
	Q_ASSERT(field_);
	setBrush(QBrush(QColor(40, 140, 255)));
}

FieldGraphItem::~FieldGraphItem() {
}

void FieldGraphItem::refresh() {
	setName(field_->getName());
	// the size depends on how many bits the field contains
	int fieldWidth = field_->getBitWidth();
	setRect(0, 0, VisualizerItem::MIN_WIDTH * fieldWidth, VisualizerItem::ITEM_HEIGHT);
	reorganizeChildren();
}

int FieldGraphItem::getOffset() const {
	return field_->getBitOffset();
}

int FieldGraphItem::getBitWidth() const {
	return field_->getBitWidth();
}

