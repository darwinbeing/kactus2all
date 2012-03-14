/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceeditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceeditor.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QWidget>

AddressSpaceEditor::AddressSpaceEditor( QSharedPointer<Component> component,
									   void* dataPointer, 
									   QWidget *parent ):
ItemEditor(component, parent),
addrSpace_(static_cast<AddressSpace*>(dataPointer)),
nameBox_(this, tr("Name and description")),
general_(addrSpace_, this),
segments_(addrSpace_, this),
parameterEditor_(&addrSpace_->getParameters(), this),
visualizer_(this) {

	Q_ASSERT(addrSpace_);

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

	QVBoxLayout* scrollLayout = new QVBoxLayout(this);
	scrollLayout->addWidget(scrollArea);

	// create the top widget and set it under the scroll area
	QWidget* topWidget = new QWidget(scrollArea);
	topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout* layout = new QVBoxLayout();

	layout->addWidget(&nameBox_);
	connect(&nameBox_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&nameBox_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);

	layout->addWidget(&general_);
	connect(&general_, SIGNAL(contentChanged()), 
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&general_, SIGNAL(addressableUnitsChanged(int)),
		&visualizer_, SLOT(setByteSize(int)), Qt::UniqueConnection);
	connect(&general_, SIGNAL(widthChanged(int)),
		&visualizer_, SLOT(setRowWidth(int)), Qt::UniqueConnection);
	connect(&general_, SIGNAL(rangeChanged(const QString&)),
		&visualizer_, SLOT(setRange(const QString&)), Qt::UniqueConnection);

	layout->addWidget(&segments_);
	connect(&segments_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&segments_, SIGNAL(segmentAdded(QSharedPointer<Segment>)),
		&visualizer_, SLOT(addSegment(QSharedPointer<Segment>)), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(segmentRemoved(const QString&)),
		&visualizer_, SLOT(removeSegment(const QString&)), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(segmentRenamed(const QString&, const QString&)),
		&visualizer_, SLOT(renameSegment(const QString&, const QString&)), Qt::UniqueConnection);
	connect(&segments_, SIGNAL(segmentChanged(QSharedPointer<Segment>)),
		&visualizer_, SLOT(updateSegment(QSharedPointer<Segment>)), Qt::UniqueConnection);

	layout->addWidget(&parameterEditor_);
	connect(&parameterEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameterEditor_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&parameterEditor_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	// the horizontal layout contains the visualizer and other widgets
	QHBoxLayout* topLayout = new QHBoxLayout(topWidget);
	topLayout->addLayout(layout);
	topLayout->addWidget(&visualizer_);

	scrollArea->setWidget(topWidget);

	restoreChanges();
}

AddressSpaceEditor::~AddressSpaceEditor() {
}

bool AddressSpaceEditor::isValid() const {
	if (!nameBox_.isValid()) {
		return false;
	}
	else if (!general_.isValid()) {
		return false;
	}
	else if (!parameterEditor_.isValid()) {
		return false;
	}
	else if (!segments_.isValid()) {
		return false;
	}
	else {
		return true;
	}
}

void AddressSpaceEditor::makeChanges() {

	addrSpace_->setName(nameBox_.getName());
	addrSpace_->setDisplayName(nameBox_.getDisplayName());
	addrSpace_->setDescription(nameBox_.getDescription());
	general_.makeChanges();
	parameterEditor_.apply();
	segments_.makeChanges();
}

void AddressSpaceEditor::removeModel() {
	QSharedPointer<Component> component = ItemEditor::component();
	component->removeAddressSpace(addrSpace_->getName());
	addrSpace_ = 0;
}

void AddressSpaceEditor::restoreChanges() {
	nameBox_.setName(addrSpace_->getName());
	nameBox_.setDisplayName(addrSpace_->getDisplayName());
	nameBox_.setDescription(addrSpace_->getDescription());
	general_.restoreChanges();
	parameterEditor_.restore();
	segments_.restore();

	visualizer_.setByteSize(addrSpace_->getAddressUnitBits());
	visualizer_.setRowWidth(addrSpace_->getWidth());
	visualizer_.setRange(addrSpace_->getRange());
	visualizer_.setSegments(addrSpace_);
}
