/* 
 *	Created on:	2.4.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuildcommandeditor.cpp
 *	Project:		Kactus 2
*/

#include "swbuildcommandeditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "swbuildcommandmodel.h"
#include "swbuilddelegate.h"
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QVBoxLayout>

SWBuildCommandEditor::SWBuildCommandEditor(QSharedPointer<Component> component,
	QList<QSharedPointer<SWBuildCommand> >& swBuildCommands,
	QWidget *parent):
QGroupBox(tr("SW build commands"), parent),
view_(NULL),
proxy_(NULL),
model_(NULL),
component_(component) {

	view_ = new EditableTableView(this);
	// set view to be sortable
	view_->setSortingEnabled(true);
	// items can not be dragged
	view_->setItemsDraggable(false);

	proxy_ = new QSortFilterProxyModel(this);

	model_ = new SWBuildCommandModel(component, swBuildCommands, this);

	proxy_->setSourceModel(model_);

	view_->setItemDelegate(new SWBuildDelegate(this));
	view_->setModel(proxy_);
	// sort the view
	view_->sortByColumn(0, Qt::AscendingOrder);

	// Software components do not contain the command
	view_->setColumnHidden(SWBuildDelegate::COMMAND_COLUMN, component->getComponentImplementation() == KactusAttribute::SW);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

SWBuildCommandEditor::~SWBuildCommandEditor() {
}

bool SWBuildCommandEditor::isValid() const {
	return model_->isValid();
}

void SWBuildCommandEditor::refresh() {
	// Software components do not contain the command
	view_->setColumnHidden(SWBuildDelegate::COMMAND_COLUMN, component_->getComponentImplementation() == KactusAttribute::SW);
	view_->update();
}
