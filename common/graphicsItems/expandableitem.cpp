/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: expandableitem.cpp
 *		Project: Kactus 2
 */

#include "expandableitem.h"
#include "graphicsexpandcollapseitem.h"

ExpandableItem::ExpandableItem( QGraphicsItem* parent /*= 0*/ ):
VisualizerItem(parent),
expandCollapseItem_(new GraphicsExpandCollapseItem(this)) {

	connect(expandCollapseItem_, SIGNAL(stateChanged(bool)),
		this, SLOT(onExpandStateChange(bool)), Qt::UniqueConnection);

	// Set the position for the hide/show button
	expandCollapseItem_->setPos(10, 4);
}

ExpandableItem::~ExpandableItem() {
}

void ExpandableItem::onExpandStateChange( bool expanded ) {
	// if there are children
	QList<QGraphicsItem*> children = childItems();
	foreach (QGraphicsItem* child, children) {
		Q_ASSERT(child);

		// if the item is visualizer item
		VisualizerItem* childItem = dynamic_cast<VisualizerItem*>(child);
		if (childItem) {
			
			// if the items are to be shown
			if (expanded) {
				childItem->show();
			}
			// if the items are to be hidden
			else {
				childItem->hide();
			}
		}
	}

	// when items are hidden/shown then the children must be reorganized
	reorganizeChildren();
}
