/* 
 *  	Created on: 17.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsvisualizer.cpp
 *		Project: Kactus 2
 */

#include "memorymapsvisualizer.h"
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapview.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapscene.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapgraphitem.h>
#include <common/graphicsItems/visualizeritem.h>
#include <common/graphicsItems/visualizeritem.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>
#include <QPointF>

MemoryMapsVisualizer::MemoryMapsVisualizer(QSharedPointer<Component> component, QWidget *parent):
ItemVisualizer(component, parent),
view_(new MemoryMapView(this)),
scene_(new MemoryMapScene(this)) {

	// display a label on top the table
	SummaryLabel* visualizationLabel = new SummaryLabel(tr("Memory maps visualization"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(visualizationLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	view_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	view_->setScene(scene_);
}

MemoryMapsVisualizer::~MemoryMapsVisualizer() {
}

void MemoryMapsVisualizer::addMemoryMapItem( MemoryMapGraphItem* memMapItem ) {
	scene_->addMemGraphItem(memMapItem);
	QPointF position = memMapItem->pos();
	view_->centerOn(position.x() - MemoryVisualizationItem::CHILD_INDENTATION, position.y());
    scene_->setSceneRect(scene_->itemsBoundingRect());
}

void MemoryMapsVisualizer::removeMemoryMapItem( MemoryMapGraphItem* memMapItem ) {
	scene_->removeMemGraphItem(memMapItem);
    scene_->setSceneRect(scene_->itemsBoundingRect());
}

QSize MemoryMapsVisualizer::minimumSizeHint() const {
	return QSize(VisualizerItem::DEFAULT_WIDTH + 80, VisualizerItem::DEFAULT_HEIGHT * 2);
}

QSize MemoryMapsVisualizer::sizeHint() const {
	return minimumSizeHint();
}
