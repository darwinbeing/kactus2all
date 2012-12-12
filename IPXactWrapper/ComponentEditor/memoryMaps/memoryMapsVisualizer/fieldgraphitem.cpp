/* 
 *  	Created on: 25.4.2012
 *      Author: Antti Kamppi
 * 		filename: fieldgraphitem.cpp
 *		Project: Kactus 2
 */

#include "fieldgraphitem.h"
#include "registergraphitem.h"

#include <QBrush>
#include <QColor>

FieldGraphItem::FieldGraphItem( QSharedPointer<Field> field,
							   QGraphicsItem* parent):
MemoryVisualizationItem(parent),
field_(field) {
	Q_ASSERT(field_);
	QBrush brush(QColor(120, 220, 255));
	setBrush(brush);
	ExpandableItem::setExpansionBrush(brush);
	setShowExpandableItem(false);
}

FieldGraphItem::~FieldGraphItem() {	
}

void FieldGraphItem::refresh() {
	setRect(0, 0, VisualizerItem::MIN_WIDTH, VisualizerItem::ITEM_HEIGHT);

	// the name depends on the size of the rectangle (if too small then name is chopped)
	setName(field_->getName());
	setLeftTopCorner(QString::number(field_->getMSB()));
	setRightTopCorner(QString::number(field_->getBitOffset()));
	ExpandableItem::reorganizeChildren();

	MemoryVisualizationItem* parentGraphItem = static_cast<MemoryVisualizationItem*>(parentItem());
	Q_ASSERT(parentGraphItem);
	parentGraphItem->refresh();
}

quint64 FieldGraphItem::getOffset() const {
	return field_->getBitOffset();
}

int FieldGraphItem::getBitWidth() const {
	return field_->getBitWidth();
}

unsigned int FieldGraphItem::getAddressUnitSize() const {
	RegisterGraphItem* regItem = static_cast<RegisterGraphItem*>(parentItem());
	Q_ASSERT(regItem);
	return regItem->getAddressUnitSize();
}

quint64 FieldGraphItem::getLastAddress() const {
	return field_->getMSB();
}

qreal FieldGraphItem::itemTotalWidth() const {
	return rect().width();
}

