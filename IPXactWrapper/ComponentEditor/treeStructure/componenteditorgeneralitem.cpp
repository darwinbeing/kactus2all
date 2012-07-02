/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgeneralitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorgeneralitem.h"

ComponentEditorGeneralItem::ComponentEditorGeneralItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
editor_(libHandler, component) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorGeneralItem::~ComponentEditorGeneralItem() {
}

QString ComponentEditorGeneralItem::text() const {
	return tr("General");
}

bool ComponentEditorGeneralItem::isValid() const {
	// general editor is always valid because it contains no mandatory elements
	return true;
}

ItemEditor* ComponentEditorGeneralItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorGeneralItem::editor() const {
	return &editor_;
}

QString ComponentEditorGeneralItem::getTooltip() const {
	return tr("Contains the general settings of a component");
}
