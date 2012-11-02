/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapsitem.h"
#include "componenteditormemmapitem.h"
#include <IPXactWrapper/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memorymapseditor.h>
#include <IPXactWrapper/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

ComponentEditorMemMapsItem::ComponentEditorMemMapsItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
memoryMaps_(component->getMemoryMaps()),
editor_(new MemoryMapsEditor(component)),
visualizer_(new MemoryMapsVisualizer(component)) {

	setObjectName(tr("ComponentEditorMemMapsItem"));

	foreach (QSharedPointer<MemoryMap> memoryMap, memoryMaps_) {
		QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(new ComponentEditorMemMapItem(
			memoryMap, model, libHandler, component, this));
		memoryMapItem->setVisualizer(visualizer_);
		childItems_.append(memoryMapItem);
	}

	editor_->hide();

	connect(editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
	connect(editor_, SIGNAL(selectBusInterface(const QString&)),
		model, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);

	connect(visualizer_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorMemMapsItem::~ComponentEditorMemMapsItem() {
	if (editor_) {
		delete editor_;
		editor_ = NULL;
	}
	if (visualizer_) {
		delete visualizer_;
		visualizer_ = NULL;
	}
}

QString ComponentEditorMemMapsItem::text() const {
	return tr("Memory maps");
}

ItemEditor* ComponentEditorMemMapsItem::editor() {
	return editor_;
}

const ItemEditor* ComponentEditorMemMapsItem::editor() const {
	return editor_;
}

QString ComponentEditorMemMapsItem::getTooltip() const {
	return tr("Contains the memory maps of the component");
}

void ComponentEditorMemMapsItem::createChild( int index ) {
	QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(
		new ComponentEditorMemMapItem(memoryMaps_.at(index), model_, libHandler_, component_, this));	
	childItems_.insert(index, memoryMapItem);
}

ItemVisualizer* ComponentEditorMemMapsItem::visualizer() {
	return visualizer_;
}
