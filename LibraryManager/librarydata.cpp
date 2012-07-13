/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#include "librarydata.h"
#include "libraryhandler.h"
#include "ipxactwidget.h"
#include "ipxactmodel.h"
#include "librarytreemodel.h"

#include "exceptions/invalid_file.h"

#include <common/widgets/ScanProgressWidget/scanprogresswidget.h>

#include <models/librarycomponent.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>
#include <models/component.h>
#include <models/design.h>
#include <models/designconfiguration.h>
#include <models/generatorchain.h>
#include <models/generaldeclarations.h>
#include <models/ComDefinition.h>
#include <models/ApiDefinition.h>

#include "vlnv.h"

#include <QFile>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QUrl>
#include <QDomDocument>
#include <QString>
#include <QObject>
#include <QBitArray>
#include <stdexcept>
#include <QWidget>
#include <QFileDialog>
#include <QColor>
#include <QSettings>
#include <QList>
#include <QProgressBar>
#include <QMap>
#include <QSharedPointer>
#include <QMutableMapIterator>
#include <QApplication>
#include <QCoreApplication>

#include <QDebug>

static const QString KACTUS_LIBRARY_DIRNAME(".kactus2");
static const QString KACTUS_LIBRARY_FILENAME(".librarySettings.ini");

LibraryData::LibraryData(LibraryHandler* parent, QMainWindow* mainWnd): 
QAbstractItemModel(parent),
mainWnd_(mainWnd),
libraryItems_(),
table_(),
handler_(parent) {

	connect(this, SIGNAL(errorMessage(const QString&)),
		parent, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(noticeMessage(const QString&)),
		parent, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
}

LibraryData::~LibraryData() {

	// clear all pointers
	libraryItems_.clear();
}

QModelIndex LibraryData::index(int row, int column,
		const QModelIndex& parent) const {

	// if invalid row
	if (row >= table_.size() || row < 0) {
		return QModelIndex();
	}
	// if invalid column
	else if (column < 0 || column > 4) {
		return QModelIndex();
	}
	// this is not hierarchical model so parent can not be valid
	else if (parent.isValid()) {
		return QModelIndex();
	}

	return createIndex(row, column, table_.value(row).data());
}

QModelIndex LibraryData::parent(const QModelIndex&) const {

	return QModelIndex();
}

int LibraryData::rowCount(const QModelIndex& parent) const {

	if (parent.isValid()) {
		return 0;
	}

	return table_.size();
}

int LibraryData::columnCount(const QModelIndex& parent) const {

	if (parent.isValid()) {
		return 0;
	}
	return 5;
}

QVariant LibraryData::data(const QModelIndex& index,
		int role) const {

	// if index is not valid
	if (!index.isValid()) {
		return QVariant();
	}
	// if row is invalid
	else if (index.row() >= table_.size() || index.row() < 0) {
		return QVariant();
	}
	// if column is invalid
	else if (index.column() > 4 || index.column() < 0) {
		return QVariant();
	}

	// if role is displayRole
	else if (role == Qt::DisplayRole) {

		// return the right value from vlnv identified by column
		return table_.at(index.row())->getElement(index.column());
	}

	// if role is tooltipRole
	else if (role == Qt::ToolTipRole) {
		switch (index.column()) {

		// VLNV type
		case 0: {
			return tr("The type of the IP-Xact document");
		}
		// VLNV vendor
		case 1: {

			return tr("The value of the vendor element in VLNV");
		}
		// VLNV Library
		case 2: {
			return tr("The value of the library element in VLNV");
		}
		// VLNV Name
		case 3: {
			return tr("The value of the name element in VLNV");
		}
		// VLNV Version
		default: {
			return tr("The value of the version element in VLNV");
		}
		}
	}

	// if role is foreground
	else if (role == Qt::ForegroundRole) {
		QColor textColor;

		if (table_.value(index.row())->documentIsValid()) {
			textColor = QColor("black");
		}

		// if the document is not valid then it is marked with red color
		else {
			textColor = QColor("red");
		}

		return QBrush(textColor);
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

Qt::ItemFlags LibraryData::flags(const QModelIndex& index) const {

	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant LibraryData::headerData(int section, Qt::Orientation orientation,
		int role) const {

	// if section is invalid
	if (section < 0 || section > 4) {
		return QVariant();
	}
	// only horizontal orientation is supported
	else if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	// if role is display role
	else if (role == Qt::DisplayRole) {

		switch (section) {

		// VLNV type
		case 0: {
			return tr("Type");
		}
		// VLNV vendor
		case 1: {
			return tr("Vendor");
		}
		// VLNV Library
		case 2: {
			return tr("Library");
		}
		// VLNV Name
		case 3: {
			return tr("Name");
		}
		// VLNV Version
		default: {
			return tr("Version");
		}
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}

QList<VLNV*> LibraryData::getItems() const {
    QList<VLNV*> vlnvs;

    foreach (QSharedPointer<VLNV> vlnv, table_)
    {
        vlnvs.append(vlnv.data());
    }

	return vlnvs;
}

void LibraryData::onExportItem(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	if (!vlnv->isValid())
		return;

	handler_->onExportItem(*vlnv);
}

void LibraryData::getDirectory(QStringList& list) {
	// empty the list
	list.clear();

	// create the dialog
	QFileDialog fileChooser(handler_, Qt::Dialog);

	// select directories
	fileChooser.setFileMode(QFileDialog::Directory);

	// only display directories
	fileChooser.setOptions(QFileDialog::ShowDirsOnly);

	// view as list
	fileChooser.setViewMode(QFileDialog::List);

	// the default directory is the users home directory
	fileChooser.setDirectory(QDir::toNativeSeparators(QDir::homePath()));

	// execute the dialog and if user clicks "ok" then add the selected files
	// to the list
	if (fileChooser.exec()) {
		list = fileChooser.selectedFiles();
	}
	return;
}

VLNV* LibraryData::getOriginalPointer( const VLNV& vlnv ) const {
	if (libraryItems_.contains(vlnv)) {
                QMap<VLNV, QString>::const_iterator i = libraryItems_.find(vlnv);
		VLNV* vlnvP = const_cast<VLNV*>(&i.key());
		return vlnvP;
	}
	else
		return 0;
}

const QString LibraryData::getPath( const VLNV& vlnv ) {

	if (libraryItems_.contains(vlnv))
		return libraryItems_.value(vlnv);
	else {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"was not found in the library.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));
		return QString();
	}
}

bool LibraryData::addVLNV( const VLNV& vlnv, const QString& path) {
	if (libraryItems_.contains(vlnv)) {
		emit errorMessage(tr("The VLNV \n"
			"Vendor: %1\n"
			"Library: %2\n"
			"Name: %3\n"
			"Version: %4\n"
			"Already existed in the library and was not added.").arg(
			vlnv.getVendor()).arg(vlnv.getLibrary()).arg(vlnv.getName()).arg(
			vlnv.getVersion()));
		return false;
	}
	QFileInfo fileInfo(path);
	if (!fileInfo.exists()) {
		emit errorMessage(tr("The file %1 was not found in file system").arg(path));
		return false;
	}

	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	// add the component to the library
	libraryItems_.insert(vlnv, path).key();
	table_.append(QSharedPointer<VLNV>(new VLNV(vlnv)));

    VLNV* vlnvP = table_.back().data();

	emit addVLNV(vlnvP);
	endInsertRows();

	return true;
}

bool LibraryData::contains( const VLNV& vlnv ) {
	// if vlnv is found and it is of correct type
	return libraryItems_.contains(vlnv);
}

void LibraryData::onRemoveVLNV( VLNV* vlnv ) {
	
	// if vlnv does not belong to library
	if (!libraryItems_.contains(*vlnv))
		return;

	int row = 0;

    foreach (QSharedPointer<VLNV> vlnvP, table_)
    {
        if (vlnv == vlnvP.data())
        {
            break;
        }

        ++row;
    }

	beginRemoveRows(QModelIndex(), row, row);

	// remove the vlnv, no delete operation is needed because VLNVs are statically
	// created
    libraryItems_.remove(*vlnv);
	table_.removeAt(row);

	endRemoveRows();

}

VLNV::IPXactType LibraryData::getType( const VLNV& vlnv ) const {

	if (!libraryItems_.contains(vlnv))
		return VLNV::INVALID;
	else {
		QMap<VLNV, QString>::const_iterator i = libraryItems_.find(vlnv);
		return i.key().getType();
	}
}

void LibraryData::onOpenComponent(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	if (!vlnv->isValid())
		return;

	if (vlnv->getType() == VLNV::COMPONENT)
		emit editItem(*vlnv);
}

void LibraryData::onDeleteItem(const QModelIndex& index) {
	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());

	// inform tree model that vlnv is to be removed
	emit removeVLNV(vlnv);

	onRemoveVLNV(vlnv);
}

void LibraryData::onOpenDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit openDesign(*vlnv);
}

void LibraryData::onOpenSWDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit openSWDesign(*vlnv);
}

void LibraryData::onOpenSystemDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit openSystemDesign(*vlnv);
}

void LibraryData::onCreateNewComponent( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createComponent(*vlnv);
}

void LibraryData::onCreateNewDesign( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createDesign(*vlnv);
}

void LibraryData::onCreateNewSWDesign( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit createSWDesign(*vlnv);
}

void LibraryData::onOpenBusDef( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit editItem(*vlnv);
}

void LibraryData::onCreateBusDef( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
	emit createBusDef(*vlnv);
}

void LibraryData::onOpenComDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit editItem(*vlnv);
}

void LibraryData::onCreateComDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit createComDef(*vlnv);
}

void LibraryData::onOpenApiDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit editItem(*vlnv);
}

void LibraryData::onCreateApiDef( const QModelIndex& index ) {

    if (!index.isValid())
        return;

    VLNV* vlnv = static_cast<VLNV*>(index.internalPointer());
    emit createApiDef(*vlnv);
}

void LibraryData::resetLibrary() {
	beginResetModel();
	emit resetModel();
	endResetModel();
}

void LibraryData::checkLibraryIntegrity( bool showProgress /*= true*/ ) {
	beginResetModel();

	table_.clear();

	int max = libraryItems_.size();
	int current = 0;
	int errors = 0;
	int failedObjects = 0;
	int syntaxErrors = 0;
	int vlnvErrors = 0;
	int fileErrors = 0;

	// create the progress bar that displays the progress of the check
	QProgressBar progBar;
	
	if (showProgress) {
		progBar.setRange(0, max);
		progBar.setValue(current);
		progBar.setOrientation(Qt::Horizontal);
		progBar.setFormat(tr("Processing item %v of %m (%p%)"));
		progBar.move(handler_->mapToGlobal(handler_->geometry().topRight()));
		progBar.setWindowTitle(tr("Checking integrity..."));
		progBar.show();
	}

    emit noticeMessage(tr("------ Library Integrity Check ------"));
	
	QMap<VLNV, QString>::iterator i = libraryItems_.begin();
	while (i != libraryItems_.end()) {

		// in the start assume that document is valid and if errors are 
		// found the set document as invalid
		bool wasValid = true;

		QSharedPointer<LibraryComponent> libComp = getModel(i.key());

		// if the object could not be parsed
		if (!libComp) {

			// remove the pair from the map and move on
			i = libraryItems_.erase(i);
			continue;
		}

		// get pointer to the vlnv
		VLNV vlnv = i.key();

		// inform the user of the object being processed

		// make sure the file exists
		QFileInfo topFile(i.value());
		if (!topFile.exists()) {
            emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
			emit errorMessage(tr("The file %1 for the document was not found.").arg(i.value()));
			++errors;
			++failedObjects;
			wasValid = false;
		}

		// check if the component is valid and if not then print errors of the component
		QStringList errorList;

		if (!libComp->isValid(errorList))
        {
            if (wasValid)
            {
                emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
            }

			foreach (QString error, errorList) {
				emit errorMessage(error);
			}
			errors += errorList.size();
			syntaxErrors += errorList.size();
			
			// if this was first failed test then increase number of failed items
			if (wasValid) {
				++failedObjects;
				wasValid = false;
			}
		}

		// check that all VLNVs needed by this model are found in the library
		QList<VLNV> vlnvList = libComp->getDependentVLNVs();

		for (int j = 0; j < vlnvList.size(); ++j)
        {
			// if the document referenced by this model is not found
			if (!libraryItems_.contains(vlnvList.at(j)))
            {
                if (wasValid)
                {
                    emit noticeMessage(tr("The following errors were found while processing item %1:").arg(vlnv.toString(":")));
                }

				emit errorMessage(
                    tr("The following dependent VLNV was not found in the library: %1").arg(vlnvList.at(j).toString()));
				
				++errors;
				++vlnvErrors;

				// if this was first failed test then increase number of failed items
				if (wasValid) {
					++failedObjects;
					wasValid = false;
				}
			}
		}

		// check all files referenced by this model
		QStringList filelist = libComp->getDependentFiles();
		for (int j = 0; j < filelist.size(); ++j) {

			// make sure that each file referenced by the model exists
			// in the file system
			QString path = General::getAbsolutePath(i.value(), filelist.at(j));

			// if the path did not exist
			if (path.isEmpty()) {

				// print the relative path because absolute path does not exist
				path = filelist.at(j);

				emit errorMessage(
					tr("\tFile %1 was not found in the file system.").arg(
					path));
				
				++errors;
				++ fileErrors;

				// if this was first failed test then increase number of failed items
				if (wasValid) {
					++failedObjects;
					wasValid = false;
				}
			}
		}

		// set the validity of the object
		vlnv.setDocumentValid(wasValid);

		table_.append(QSharedPointer<VLNV>(new VLNV(vlnv)));

		// update the progress bar
		++current;
		progBar.setValue(current);

		++i;
	}

	if (showProgress) {
		// the progress bar can now be hidden
		progBar.hide();
	}

	emit noticeMessage(
        tr("========== Library integrity check complete: found %1 errors within %2 item(s) ==========").arg(
		errors).arg(failedObjects));
	
	// if errors were found then print the summary of error types
	if (errors > 0) {
		emit noticeMessage(tr("Structural errors within item(s): %1").arg(syntaxErrors));
		emit noticeMessage(tr("Invalid VLNV references: %1").arg(vlnvErrors));
		emit noticeMessage(tr("Invalid file references: %1\n").arg(fileErrors));
	}

	// inform tree model that it needs to reset model also
	emit resetModel();

	endResetModel();
}

void LibraryData::parseLibrary( bool showProgress /*= true*/ ) {
	
	QApplication::setOverrideCursor(Qt::WaitCursor);

    Q_ASSERT(_CrtCheckMemory());

	beginResetModel();

	// clear the previous items in the library
	table_.clear();
	libraryItems_.clear();

	endResetModel();

	QSettings settings(this);

	// Load the library locations.
	QStringList locations = settings.value("library/locations", QStringList()).toStringList();

	// create the progress bar that displays the progress of the scan
	ScanProgressWidget progWidget(mainWnd_);
	int current = 1;

	if (showProgress) {
		progWidget.setRange(0, locations.size());
		//progWidget.move(handler_->mapToGlobal(handler_->geometry().topRight()));
		progWidget.show();
	}

	// search each directory
	foreach (QString location, locations) {
		QFileInfo locationInfo(location);

		progWidget.setPath(location);
		progWidget.setValue(current);

		// if the location is a directory
		if (locationInfo.isDir()) {
			parseDirectory(location);
		}
		// if the location is a direct file
		else if (locationInfo.isFile()) {
			parseFile(location);
		}
		
		// update the progress bar
		++current;
	}

	if (showProgress) {
		progWidget.hide();
	}

	// repaint so the progWidget disappears
	handler_->repaint();

	// check the integrity of the items in the library
	checkLibraryIntegrity(showProgress);

	QApplication::restoreOverrideCursor();
}

void LibraryData::parseDirectory( const QString& directoryPath ) {
	
	// if the path is empty
	if (directoryPath.isEmpty()) {
		return;
	}
	QFileInfo dirInfo(directoryPath);
	
	// if the directory does not exist
	if (!dirInfo.exists()) {
		return;
	}

	// it should always be for a directory
	Q_ASSERT(dirInfo.isDir());

	QDir dirHandler(directoryPath);

	// get list of files and folders
	QFileInfoList entryInfos(dirHandler.entryInfoList(QDir::NoDotAndDotDot |
		QDir::AllDirs | QDir::Files));

	foreach (QFileInfo entryInfo, entryInfos) {
		
		// if the file/directory does not exist
		if (!entryInfo.exists()) {
			continue;
		}

		// if the entry is an xml file
		if (entryInfo.isFile() && entryInfo.suffix() == QString("xml")) {
			parseFile(entryInfo.absoluteFilePath());
		}

		// if the entry is a sub-directory
		else if (entryInfo.isDir()) {
			parseDirectory(entryInfo.absoluteFilePath());
		}
	}
}

void LibraryData::parseFile( const QString& filePath ) {

	if (filePath.isEmpty()) {
		return;
	}

	QFile docFile(filePath);

	// if the file can not be opened
	if (!docFile.open(QFile::ReadOnly)) {
		emit errorMessage(tr("Could not open file %1 for reading.").arg(filePath));
		return;
	}
	
	QDomDocument doc;

	VLNV vlnv;

	// if the document can be read to QDomDocument
	if (doc.setContent(&docFile)) {
		IPXactModel newModel(doc, this);
		docFile.close();
		vlnv = newModel.getVLNV();
	}
	// content could not be read
	else {
		emit errorMessage(tr("The file %1 was not valid xml and could not be read.").arg(
			filePath));
		docFile.close();
		return;
	}

	// if the vlnv is not valid (the document was not IP-Xact)
	if (!vlnv.isValid()) {
		return;
	}
	
	// if the VLNV already exists in the library
	if (libraryItems_.contains(vlnv)) {
		emit noticeMessage(tr("VLNV %1:%2:%3:%4 was already found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		return;
	}

	// add the component to the library if parsing was successful
	libraryItems_.insert(vlnv, filePath);
}

QSharedPointer<LibraryComponent> LibraryData::getModel( const VLNV& vlnv ) {
	if (!libraryItems_.contains(vlnv)) {
		emit noticeMessage(tr("VLNV %1:%2:%3:%4 was not found in the library").arg(
			vlnv.getVendor()).arg(
			vlnv.getLibrary()).arg(
			vlnv.getName()).arg(
			vlnv.getVersion()));
		return QSharedPointer<LibraryComponent>();
	}

	VLNV toCreate = vlnv;
	// make sure the vlnv is of correct type
	toCreate.setType(libraryItems_.find(vlnv).key().getType());

	// get path to the document
	QString path = libraryItems_.value(toCreate);

	// if the file was not found
	if (path.isEmpty()) {
		return QSharedPointer<LibraryComponent>();
	}

	// create file handle and use it to read the IP-Xact document into memory
	QFile file(path);
	QDomDocument doc;
	if (!doc.setContent(&file)) {
		emit errorMessage(tr("The document %1 in file %2 could not be opened.").arg(
			toCreate.toString()).arg(path));
		return QSharedPointer<LibraryComponent>();
	}
	file.close();

	QSharedPointer<LibraryComponent> libComp;

	try {
		// create correct type of object and cast the pointer
		switch (toCreate.getType()) {
			case VLNV::BUSDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new BusDefinition(doc));
				break;
									  }
			case VLNV::COMPONENT: {
				libComp = QSharedPointer<LibraryComponent>(new Component(doc));
				break;
								  }
			case VLNV::DESIGN: {
				libComp = QSharedPointer<LibraryComponent>(new Design(doc));
				break;
							   }

			case VLNV::GENERATORCHAIN: {
				libComp = QSharedPointer<LibraryComponent>(new GeneratorChain(doc));
				break;
									   }
			case VLNV::DESIGNCONFIGURATION: {
				libComp = QSharedPointer<LibraryComponent>(new DesignConfiguration(doc));
				break;
											}

			case VLNV::ABSTRACTIONDEFINITION: {
				libComp = QSharedPointer<LibraryComponent>(new AbstractionDefinition(doc));
				break;
											  }

            case VLNV::COMDEFINITION: {
                libComp = QSharedPointer<LibraryComponent>(new ComDefinition(doc));
                break;
                                      }

            case VLNV::APIDEFINITION: {
                libComp = QSharedPointer<LibraryComponent>(new ApiDefinition(doc));
                break;
                                      }
			default: {
				emit noticeMessage(tr("Document was not supported type"));
				return QSharedPointer<LibraryComponent>();
					 }
		}
	}
	// if an exception occurred during the parsing
	catch (...) {
		emit errorMessage(
			tr("Error occurred during parsing of the document %1").arg(path));
		return QSharedPointer<LibraryComponent>();
	}

	return libComp;
}
