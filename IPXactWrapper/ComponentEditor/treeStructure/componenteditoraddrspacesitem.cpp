/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspacesitem.h"
#include "componenteditoraddrspaceitem.h"

ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
															 LibraryInterface* libHandler,
															 QSharedPointer<Component> component,
															 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpaces_(component->getAddressSpaces()), 
editor_(component) {

	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, this));	
		childItems_.append(addrItem);
	}

	connect(&editor_, SIGNAL(contentChanged()), 
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
}

ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem() {
}

QString ComponentEditorAddrSpacesItem::text() const {
	return tr("Address spaces");
}

ItemEditor* ComponentEditorAddrSpacesItem::editor() {
	return &editor_;
}

QString ComponentEditorAddrSpacesItem::getTooltip() const {
	return tr("Contains the address spaces specified for the component");
}
