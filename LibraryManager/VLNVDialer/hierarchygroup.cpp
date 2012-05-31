/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchygroup.cpp
 *		Project: Kactus 2
 */

#include "hierarchygroup.h"

#include <common/utils.h>

#include <QGridLayout>

HierarchyGroup::HierarchyGroup(QWidget *parent):
QGroupBox(tr("Product Hierarchy"), parent),
globalBox_(tr("Global"), this),
productBox_(tr("Product"), this),
boardBox_(tr("Board"), this),
chipBox_(tr("Chip"), this),
socBox_(tr("SoC"), this),
ipBox_(tr("IP"), this),
options_() {

	QGridLayout* layout = new QGridLayout(this);
	layout->addWidget(&globalBox_, 0, 0, 1, 1);
	layout->addWidget(&productBox_, 0, 1, 1, 1);
	layout->addWidget(&boardBox_, 0, 2, 1, 1);
	layout->addWidget(&chipBox_, 1, 0, 1, 1);
	layout->addWidget(&socBox_, 1, 1, 1, 1);
	layout->addWidget(&ipBox_, 1, 2, 1, 1);
	layout->setSpacing(0);
	layout->setContentsMargins(4, 4, 4, 4);

	globalBox_.setChecked(true);
	productBox_.setChecked(true);
	boardBox_.setChecked(true);
	chipBox_.setChecked(true);
	socBox_.setChecked(true);
	ipBox_.setChecked(true);

	connect(&globalBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onGlobalChange(int)), Qt::UniqueConnection);
	connect(&productBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onProductChange(int)), Qt::UniqueConnection);
	connect(&boardBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onBoardChange(int)), Qt::UniqueConnection);
	connect(&chipBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onChipChange(int)), Qt::UniqueConnection);
	connect(&socBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onSocChange(int)), Qt::UniqueConnection);
	connect(&ipBox_, SIGNAL(stateChanged(int)),
		this, SLOT(onIpChange(int)), Qt::UniqueConnection);
}

HierarchyGroup::~HierarchyGroup() {
}

void HierarchyGroup::onGlobalChange( int state ) {
	options_.global_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void HierarchyGroup::onProductChange( int state ) {
	options_.product_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void HierarchyGroup::onBoardChange( int state ) {
	options_.board_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void HierarchyGroup::onChipChange( int state ) {
	options_.chip_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void HierarchyGroup::onSocChange( int state ) {
	options_.soc_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}

void HierarchyGroup::onIpChange( int state ) {
	options_.ip_ = Utils::checkBoxState2Bool(state);
	emit optionsChanged(options_);
}