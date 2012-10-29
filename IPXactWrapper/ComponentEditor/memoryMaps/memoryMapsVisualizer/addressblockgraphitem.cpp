/* 
 *  	Created on: 18.4.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockgraphitem.cpp
 *		Project: Kactus 2
 */

#include "addressblockgraphitem.h"
#include "registergraphitem.h"
#include <models/register.h>

#include <QBrush>
#include <QColor>

AddressBlockGraphItem::AddressBlockGraphItem( QSharedPointer<AddressBlock> addrBlock, 
											 QGraphicsItem *parent ):
MemoryVisualizationItem(parent),
addrBlock_(addrBlock) {

	Q_ASSERT(addrBlock_);
	setBrush(QBrush(QColor(80, 180, 255)));
}

AddressBlockGraphItem::~AddressBlockGraphItem() {
}

void AddressBlockGraphItem::refresh() {
	setName(addrBlock_->getName());

	QList<QSharedPointer<RegisterModel> >& regItems = addrBlock_->getRegisterData();
	foreach (QSharedPointer<RegisterModel> regItem, regItems) {

		QSharedPointer<Register> reg = regItem.dynamicCast<Register>();
		if (reg) {

			// create the item
			RegisterGraphItem* regGraph = new RegisterGraphItem(reg, this);

			// get the offset of the item
			int offset = regGraph->getOffset();

			// make sure the items are in correct order for the offset
			childItems_.insert(offset, regGraph);

			// tell child to check its children
			regGraph->refresh();
		}
	}

	// set the positions for the children
	MemoryVisualizationItem::reorganizeChildren();
}

int AddressBlockGraphItem::getOffset() const {
	return 0;
}

int AddressBlockGraphItem::getBitWidth() const {
	return 0;
}
