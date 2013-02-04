/* 
 *  	Created on: 15.8.2011
 *      Author: Antti Kamppi
 * 		filename: absdefgroup.cpp
 *		Project: Kactus 2
 */

#include "absdefgroup.h"
#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QModelIndexList>
#include <QCoreApplication>
#include <QFile>

AbsDefGroup::AbsDefGroup(LibraryInterface* handler, QWidget *parent): 
QGroupBox(tr("Signals (Abstraction Definition)"), parent),
newSignalButton_(tr("Add new signal"), this),
newSignalOptionsButton_(tr("Add new signal options"), this),
importButton_(tr("Import csv"), this),
exportButton_(tr("Export csv"), this),
portView_(this),
portModel_(this),
handler_(handler),
absDef_() {

	portView_.setModel(&portModel_);

	connect(&newSignalButton_, SIGNAL(clicked(bool)),
		&portModel_, SLOT(addSignal()), Qt::UniqueConnection);
	connect(&newSignalOptionsButton_, SIGNAL(clicked(bool)),
		this, SLOT(onAddSignalOptions()), Qt::UniqueConnection);
	connect(&importButton_, SIGNAL(clicked(bool)),
		this, SLOT(onImport()), Qt::UniqueConnection);
	connect(&exportButton_, SIGNAL(clicked(bool)),
		this, SLOT(onExport()), Qt::UniqueConnection);
	
	connect(&portModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&portModel_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	connect(&portView_, SIGNAL(removeItems(const QModelIndexList&)),
		&portModel_, SLOT(removeIndexes(const QModelIndexList&)), Qt::UniqueConnection);
	connect(&portView_, SIGNAL(copyItems(const QModelIndexList&)),
		&portModel_, SLOT(copyIndexes(const QModelIndexList&)), Qt::UniqueConnection);		

	setupLayout();
}

AbsDefGroup::~AbsDefGroup() {
}

void AbsDefGroup::onImport() {

	QString homePath;

	// if abs def is set then use its location in library
	if (absDef_) {
		const QString busPath = handler_->getDirectoryPath(*absDef_->getVlnv());
		homePath = QString("%1/logicalSignals.csv").arg(busPath);
	}
	// if there is no abs def then use default library location
	else {
		QSettings settings;
		homePath = settings.value(QString("Library/DefaultLocation"), 
			QCoreApplication::applicationDirPath()).toString();
	}

	QString path = QFileDialog::getOpenFileName(this,  tr("csv-files (*.csv)"), homePath);

	// if user clicked cancel
	if (path.isEmpty()) {
		return;
	}

	portModel_.importCSV(path);
}

void AbsDefGroup::onExport() {
	QString homePath;

	// if abs def is set then use its location in library
	if (absDef_) {
		const QString busPath = handler_->getDirectoryPath(*absDef_->getVlnv());
		homePath = QString("%1/logicalSignals.csv").arg(busPath);
	}
	// if there is no abs def then use default library location
	else {
		QSettings settings;
		homePath = settings.value(QString("Library/DefaultLocation"), 
			QCoreApplication::applicationDirPath()).toString();
	}

	QString path = QFileDialog::getSaveFileName(this, tr("Save a csv-file"), 
		homePath, tr("csv-files (*.csv)"));

	// if user clicked cancel
	if (path.isEmpty())
		return;

	portModel_.exportCSV(path);
}

void AbsDefGroup::onAddSignalOptions() {
	portModel_.addSignalOptions(portView_.selected());
}

void AbsDefGroup::save() {
	// tell port model to transfer the ports to the abstraction definition
	portModel_.save();
}

void AbsDefGroup::setupLayout() {
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(&newSignalButton_);
	buttonLayout->addWidget(&newSignalOptionsButton_);
	buttonLayout->addWidget(&importButton_);
	buttonLayout->addWidget(&exportButton_);
	buttonLayout->addStretch();

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(buttonLayout);
	topLayout->addWidget(&portView_, 1);
}

void AbsDefGroup::setAbsDef(QSharedPointer<AbstractionDefinition> absDef) {
	absDef_ = absDef;
	portModel_.setAbsDef(absDef);
}
