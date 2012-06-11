/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceseditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>

#include <QVBoxLayout>

AddressSpacesEditor::AddressSpacesEditor(QSharedPointer<Component> component):
ItemEditor(component),
view_(this),
model_(component, this),
proxy_(this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address spaces summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);

	view_.setModel(&proxy_);
	
// 	view_.setItemDelegate(new FileSetsDelegate(this));
// 	view_.setColumnWidth(0, FileSetsEditor::NAME_COLUMN_WIDTH);
// 	view_.setColumnWidth(1, FileSetsEditor::DESC_COLUMN_WIDTH);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
	connect(&model_, SIGNAL(addrSpaceAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(addrSpaceRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

AddressSpacesEditor::~AddressSpacesEditor() {
}

bool AddressSpacesEditor::isValid() const {
	return model_.isValid();
}

void AddressSpacesEditor::makeChanges() {
	// TODO remove this in final
}

void AddressSpacesEditor::refresh() {
	view_.update();
}
