/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfaceseditor.cpp
 *		Project: Kactus 2
 */

#include "businterfaceseditor.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include "businterfacesdelegate.h"
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::BusInterfacesEditor()
//-----------------------------------------------------------------------------
BusInterfacesEditor::BusInterfacesEditor(LibraryInterface* handler,
										 QSharedPointer<Component> component, 
										 QWidget* parent /*= 0*/ ):
ItemEditor(component, handler, parent),
view_(this),
proxy_(this),
model_(handler, component, this) {

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Bus interfaces"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/busIfListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

    // Items can be dragged to change postitions. Drop is enabled for vlnv columns.
    view_.setItemsDraggable(true);
    view_.viewport()->setAcceptDrops(true); 
    view_.setDropIndicatorShown(true);   
    view_.setDragDropMode(QAbstractItemView::DropOnly);
    view_.setItemDelegate(new BusInterfacesDelegate(this));    
    
	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(busifRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(&model_, SIGNAL(busIfMoved(int, int)),
        this, SIGNAL(childMoved(int, int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(moveItem(const QModelIndex&, const QModelIndex&)),
        &model_, SLOT(onMoveItem(const QModelIndex&, const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::~BusInterfacesEditor()
//-----------------------------------------------------------------------------
BusInterfacesEditor::~BusInterfacesEditor() {
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::isValid()
//-----------------------------------------------------------------------------
bool BusInterfacesEditor::isValid() const {
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::refresh()
//-----------------------------------------------------------------------------
void BusInterfacesEditor::refresh() {
	view_.setModel(&model_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesEditor::showEvent()
//-----------------------------------------------------------------------------
void BusInterfacesEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/businterfaces.html");
}
