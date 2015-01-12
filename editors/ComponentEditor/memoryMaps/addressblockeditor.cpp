/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockeditor.cpp
 *		Project: Kactus 2
 */

#include "addressblockeditor.h"
#include <common/views/EditableTableView/editabletableview.h>
#include "addressblockmodel.h"
#include "addressblockdelegate.h"
#include <common/widgets/summaryLabel/summarylabel.h>
#include "addressblockproxy.h"
#include <library/LibraryManager/libraryinterface.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QVBoxLayout>

AddressBlockEditor::AddressBlockEditor(QSharedPointer<AddressBlock> addressBlock,
									   QSharedPointer<Component> component,
									   LibraryInterface* handler,
									   QWidget* parent /*= 0*/):
ItemEditor(component, handler, parent),
view_(new EditableTableView(this)),
proxy_(new AddressBlockProxy(this)),
model_(new AddressBlockModel(addressBlock, component->getChoices(), 
    QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(component)), this))
{

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Registers summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	//! \brief Enable import/export csv file
	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/registerList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

	view_->setItemDelegate(new AddressBlockDelegate(this));

	view_->sortByColumn(AddressBlockDelegate::OFFSET_COLUMN, Qt::AscendingOrder);

	connect(model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)),
		this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)),
		this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

AddressBlockEditor::~AddressBlockEditor() {
}

bool AddressBlockEditor::isValid() const {
	return model_->isValid();
}

void AddressBlockEditor::refresh() {
	view_->update();
}

void AddressBlockEditor::showEvent( QShowEvent* event ) {
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressblock.html");
}

QSize AddressBlockEditor::sizeHint() const {
	return QSize(AddressBlockEditor::WIDTH, AddressBlockEditor::HEIGHT);
}
