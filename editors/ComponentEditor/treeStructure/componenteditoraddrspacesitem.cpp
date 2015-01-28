/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoraddrspacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditoraddrspacesitem.h"
#include "componenteditoraddrspaceitem.h"
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/addressSpaces/addressspaceseditor.h>

ComponentEditorAddrSpacesItem::ComponentEditorAddrSpacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
addrSpaces_(component->getAddressSpaces())
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	foreach (QSharedPointer<AddressSpace> addrSpace, addrSpaces_) {
		QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
			new ComponentEditorAddrSpaceItem(addrSpace, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, this));	
		childItems_.append(addrItem);
	};
}

ComponentEditorAddrSpacesItem::~ComponentEditorAddrSpacesItem() {
}

QFont ComponentEditorAddrSpacesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!addrSpaces_.empty());
    return font;
}

QString ComponentEditorAddrSpacesItem::text() const {
	return tr("Address spaces");
}

ItemEditor* ComponentEditorAddrSpacesItem::editor() {
	if (!editor_) {
		editor_ = new AddressSpacesEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
		connect(editor_, SIGNAL(selectBusInterface(const QString&)),
			model_, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);
	}
	return editor_;
}

QString ComponentEditorAddrSpacesItem::getTooltip() const {
	return tr("Contains the address spaces specified for the component");
}

void ComponentEditorAddrSpacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorAddrSpaceItem> addrItem(
		new ComponentEditorAddrSpaceItem(addrSpaces_.at(index), model_, libHandler_, component_, referenceCounter_,
        parameterFinder_, expressionFormatter_, this));
	addrItem->setLocked(locked_);
	childItems_.insert(index, addrItem);
}
