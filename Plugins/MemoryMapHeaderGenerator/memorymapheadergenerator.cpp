/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "memorymapheadergenerator.h"

#include "localheadersavemodel.h"
#include "localheadersavedelegate.h"
#include "systemheadersavemodel.h"

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <library/LibraryManager/libraryinterface.h>

#include <common/dialogs/fileSaveDialog/filesavedialog.h>
#include <common/utils.h>

#include <IPXACTmodels/SWView.h>
#include <IPXACTmodels/SystemView.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/Interface.h>
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

#include <QtPlugin>
#include <QFileInfo>
#include <QFile>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDate>
#include <QSettings>
#include <QDir>
#include <QString>
#include <QDesktopServices>
#include <QUrl>
#include <QStringList>
#include <QMessageBox>

MemoryMapHeaderGenerator::MemoryMapHeaderGenerator():
utility_(NULL),
design_(),
operatedInterfaces_(),
sysGenSettings_() {
}

MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator() {
}

QString const& MemoryMapHeaderGenerator::getName() const {
	static QString name(tr("Memory Map Header Generator"));
	return name;
}

QString const& MemoryMapHeaderGenerator::getVersion() const {
	static QString version(tr("1.0"));
	return version;
}

QString const& MemoryMapHeaderGenerator::getDescription() const {
	static QString description(tr("Generates C-headers for memory maps of a component"));
	return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getVendor()
//-----------------------------------------------------------------------------
QString const& MemoryMapHeaderGenerator::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicence()
//-----------------------------------------------------------------------------
QString const& MemoryMapHeaderGenerator::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& MemoryMapHeaderGenerator::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MemoryMapHeaderGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MemoryMapHeaderGenerator::getIcon() const
{
    return QIcon(":icons/headerGenerator24.png");
}

bool MemoryMapHeaderGenerator::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
                                                     QSharedPointer<LibraryComponent const> libDesConf,
                                                     QSharedPointer<LibraryComponent const> /*libDes*/) const
{
	// make sure the object is a component
	QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
	if (!comp) {
		return false;
	}

	// if there is no design then header is generated for local memory maps
	if (!libDesConf) {
		return comp->hasLocalMemoryMaps();
	}

	// make sure the second parameter is for a design configuration object
	QSharedPointer<DesignConfiguration const> designConf = libDesConf.dynamicCast<DesignConfiguration const>();
	// the design configuration must be for HW or system
	if (designConf) {
		return comp->getComponentImplementation() == KactusAttribute::KTS_HW;
	}
	else {
		return false;
	}
}

void MemoryMapHeaderGenerator::runGenerator( IPluginUtility* utility,
	QSharedPointer<LibraryComponent> libComp,
	QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
	QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ ) {

	utility_ = utility;

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);

	QSharedPointer<Design> design = libDes.dynamicCast<Design>();

	// if there is no design object then create headers for local memory maps
	if (!design) {
		generateLocalMemMapHeaders(comp);
	}
	// if there is a design configuration
	else if (libDesConf) {
		Q_ASSERT(design);

		// the component knows the implementation of the view
		KactusAttribute::Implementation implementation = comp->getViewType(*libDesConf->getVlnv());

		QSharedPointer<DesignConfiguration> desConf = libDesConf.dynamicCast<DesignConfiguration>();
		Q_ASSERT(desConf);

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::KTS_HW) {
			generateGlobalHeaders(comp, design);
		}

		// the generator is run on a system component
		else {
			generateSystemHeaders(comp, desConf, design);
		}
	}

	// if there is a design but no design configuration
	else {
		Q_ASSERT(design);

		KactusAttribute::Implementation implementation = comp->getViewType(*design->getVlnv());

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::KTS_HW) {
			generateGlobalHeaders(comp, design);
		}

		// the generator is run on a system component without the configuration
		else {
			QMessageBox::warning(utility->getParentWidget(), QCoreApplication::applicationName(),
				tr("A system design opened without configuration.\nSystem design must always have a configuration."));
			return;
		}
	}
}

void MemoryMapHeaderGenerator::addHeaderFile( QSharedPointer<Component> component,
	const QFileInfo& fileInfo,
	const QString& filesetName,
	const QStringList& swViewNames,
	const QString& instanceId) const {
	
	QString xmlDir = utility_->getLibraryInterface()->getDirectoryPath(*component->getVlnv());
	
	// if the directory does not exist
	QDir ipXactDir(xmlDir);
	Q_ASSERT(ipXactDir.exists());
	Q_ASSERT(fileInfo.exists());

	// calculate the relative path 
	QString relPath =  ipXactDir.relativeFilePath(fileInfo.absoluteFilePath());

	// file set where the file is added to
	QSharedPointer<FileSet> fileSet = component->getFileSet(filesetName);
	Q_ASSERT(fileSet);

	fileSet->setGroups("generatedFiles");
	fileSet->setDescription(tr("Contains header files generated for the component.\n"
		"Do not rename this file set, name is used to find the generated headers."));

	// if instance id was specified
	if (!instanceId.isEmpty()) {

		// use the same id as the instance the file set is created for.
		fileSet->setFileSetId(instanceId);
	}

	QSettings settings;
	QSharedPointer<File> file = fileSet->addFile(relPath, settings);
	Q_ASSERT(file);

	file->setDescription(tr("A header file generated by Kactus2.\n"
		"This file contains the register and memory addresses defined in the memory map(s)"));
	file->setIncludeFile(true);

	// update the sw views of the component to contain the reference to the file set.
	foreach (QString swViewName, swViewNames) {
		QSharedPointer<SWView> swView = component->getSWView(swViewName);

		swView->addFileSetRef(filesetName);
	}

	// show the generated file to user
	QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
}

void MemoryMapHeaderGenerator::generateLocalMemMapHeaders( QSharedPointer<Component> comp ) {
	// the model which displays the local memory maps and their header paths
	LocalHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setComponent(comp);

	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);
	dialog.setItemDelegate(new LocalHeaderSaveDelegate(comp, this));

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	// if there are no files to add then there is no need to save the component
	bool changed = false;

	// get list of the selected paths and generate each header
	const QList<LocalHeaderSaveModel::SaveFileOptions*>& options = model.getHeaderOptions();
	foreach (LocalHeaderSaveModel::SaveFileOptions* headerOpt, options) {

		QFile file(headerOpt->fileInfo_.absoluteFilePath());

		// make sure the directory structure exists for the file
		QDir dir(headerOpt->fileInfo_.dir());
		QString dirName(dir.dirName());
		dir.cdUp();
		dir.mkpath(dirName);

		// open the file and erase all old contents if any exists

		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(headerOpt->fileInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream stream(&file);

		// write header comments of the file
		stream << "/*" << endl;
		stream << " * File: " << headerOpt->fileInfo_.fileName() << endl;
		stream << " * Created on: " << QDate::currentDate().toString("dd.MM.yyyy") << endl;

		QSettings settings;
		QString userName = settings.value("General/Username", Utils::getCurrentUser()).toString();

		stream << " * Generated by: " << userName << endl;
		stream << " *" << endl;
		stream << " * Description:" << endl;
		stream << " * Header file generated by Kactus2 from local memory map \"" << headerOpt->localMemMap_->getName() << "\"." << endl;
		stream << " * This file contains addresses of the memories and registers defined in the local memory map." << endl;
		stream << " * Source component: " << comp->getVlnv()->toString() << "." << endl;
		stream << "*/" << endl;
		stream << endl;

		// write the preprocessor guard
		QString headerGuard("__");
		headerGuard.append(comp->getVlnv()->toString("_").toUpper());
		headerGuard.append(QString("_%1_H").arg(headerOpt->localMemMap_->getName().toUpper()));
		stream << "#ifndef " << headerGuard << endl;
		stream << "#define " << headerGuard << endl << endl;

		// write the memory addresses
		headerOpt->localMemMap_->writeMemoryAddresses(stream, 0);

		// if the register names are unique then there is no need to add address block name
		QStringList regNames;
		if (headerOpt->localMemMap_->uniqueRegisterNames(regNames)) {
			headerOpt->localMemMap_->writeRegisters(stream, 0, false);
		}
		// if there are registers with same names then address block names must be
		// included in the defines
		else {
			headerOpt->localMemMap_->writeRegisters(stream, 0, true);
		}

		// end the preprocessor guard
		stream << "#endif /* " << headerGuard << " */" << endl << endl;

		// close the file after writing
		file.close();

		QStringList swViewRefs;

		// where user selected to add a reference to the generated file set
		QString swViewRef = headerOpt->swView_;
		Q_ASSERT(!swViewRef.isEmpty());

		// if user selected to add the reference to all SW views.
		if (swViewRef == tr("all")) {
			swViewRefs = comp->getSWViewNames();
		}

		// if user selected only a single view
		else {
			swViewRefs.append(swViewRef);
		}

		// add the file to the component's file sets
		addHeaderFile(comp, headerOpt->fileInfo_, QString("%1_header").arg(headerOpt->localMemMap_->getName()), swViewRefs);

		// a header file was added
		changed = true;
	}

	if (changed) {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}
}

void MemoryMapHeaderGenerator::generateGlobalHeaders( QSharedPointer<Component> comp, QSharedPointer<Design> design ) {
	
	design_ = design;
	operatedInterfaces_.clear();

	// the model which manages the dialog contents
	GlobalHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setDesign(comp, design);

	// create the dialog to display the headers to be generated
	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	// if there are no files to add then there is no need to save the component
	bool changed = false;

	const QList<GlobalHeaderSaveModel::SaveFileOptions*>& options = model.getHeaderOptions();
	foreach (GlobalHeaderSaveModel::SaveFileOptions* headerOpt, options) {
		QFile file(headerOpt->fileInfo_.absoluteFilePath());

		// make sure the directory structure exists for the file
		QDir dir(headerOpt->fileInfo_.dir());
		if (!dir.exists()) {
			QString dirName(dir.dirName());
			dir.cdUp();
			dir.mkpath(dirName);
		}

		// open the file and erase all old contents if any exists

		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(headerOpt->fileInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream stream(&file);

		// write header comments of the file
		stream << "/*" << endl;
		stream << " * File: " << headerOpt->fileInfo_.fileName() << endl;
		stream << " * Created on: " << QDate::currentDate().toString("dd.MM.yyyy") << endl;

		QSettings settings;
		QString userName = settings.value("General/Username", Utils::getCurrentUser()).toString();

		stream << " * Generated by: " << userName << endl;
		stream << " *" << endl;
		stream << " * Description:" << endl;
		stream << " * Header file generated by Kactus2 for instance \"" << headerOpt->instance_ << 
			"\" interface \"" << headerOpt->interface_ << "\"." << endl;
		stream << " * This file contains addresses of the memories and registers defined in the memory maps of connected components." << endl;
		stream << " * Source component: " << headerOpt->comp_.toString() << "." << endl;
		stream << "*/" << endl;
		stream << endl;

		// write the preprocessor guard
		QString headerGuard(QString("__%1_%2_H").arg(headerOpt->instance_.toUpper()).arg(headerOpt->interface_.toUpper()));
		stream << "#ifndef " << headerGuard << endl;
		stream << "#define " << headerGuard << endl << endl;

		// add the starting point to the list of operated interfaces
		Interface cpuMasterInterface(headerOpt->instance_, headerOpt->interface_);
		operatedInterfaces_.append(cpuMasterInterface);

		// start the address parsing from the cpu's interface
		parseInterface(0, stream, cpuMasterInterface);

		// end the preprocessor guard
		stream << "#endif /* " << headerGuard << " */" << endl << endl;
		
		// close the file after writing
		file.close();

		// add the file to the component's file sets
		addHeaderFile(comp, headerOpt->fileInfo_, headerOpt->instance_, QStringList(), headerOpt->instanceId_);

		// a header file was added
		changed = true;

		// the list must be cleared when moving to completely new master interface
		// so each header generation starts from scratch.
		operatedInterfaces_.clear();
	}

	if (changed) {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}

	// clear the members for next generation run
	design_.clear();
	operatedInterfaces_.clear();
}

void MemoryMapHeaderGenerator::parseInterface( qint64 offset, QTextStream& stream, const Interface& interface )
{
	Q_ASSERT(design_);
	Q_ASSERT(design_->containsHWInstance(interface.getComponentRef()));

	// parse the component containing the interface
	VLNV compVLNV = design_->getHWComponentVLNV(interface.getComponentRef());
	QSharedPointer<const LibraryComponent> libComp = utility_->getLibraryInterface()->getModelReadOnly(compVLNV);
	QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
	Q_ASSERT(comp);

	switch (comp->getInterfaceMode(interface.getBusRef())) {
	case General::MASTER: {
		// update the offset
		offset += Utils::str2Int(comp->getBusInterface(interface.getBusRef())->getMaster()->getBaseAddress());

		// ask the design for interfaces that are connected to this interface
		QList<Interface> connected = design_->getConnectedInterfaces(interface);
		
		// all connected interfaces are processed
		foreach (Interface targetInterface, connected) {

			// if the connected interface has already been processed before
			if (operatedInterfaces_.contains(targetInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(targetInterface);

			parseInterface(offset, stream, targetInterface);
		}
		break;
								 }
	case General::SLAVE: {

		// the slave contains the slave-specific data
		QSharedPointer<SlaveInterface> slave = comp->getBusInterface(interface.getBusRef())->getSlave();
		Q_ASSERT(slave);

		QSharedPointer<MemoryMap> memMap = comp->getMemoryMap(slave->getMemoryMapRef());

		// if the memory map exists and contains at least something
		if (memMap && memMap->containsSubItems()) {
			// write the identifier comment for the instance
			stream << "/*" << endl;
			stream << " * Instance: " << interface.getComponentRef() << " Interface: " << interface.getBusRef() << endl;
			stream << " * Instance base address: 0x" << QString::number(offset, 16) << endl;
			stream << " * Source component: " << comp->getVlnv()->toString() << endl;

			// if there is a description for the component instance
			QString instanceDesc = design_->getHWInstanceDescription(interface.getComponentRef());
			if (!instanceDesc.isEmpty()) {
				stream << " * Description:" << endl;
				stream << " * " << instanceDesc << endl;
			}
			
			stream << " * The defines for the memory map \"" << memMap->getName() << "\":" << endl;
			stream << "*/" << endl << endl;

			memMap->writeMemoryAddresses(stream, offset, interface.getComponentRef());

			// if the registers within the instance are unique then do not concatenate with address block name
			QStringList regNames;
			if (memMap->uniqueRegisterNames(regNames)) {
				memMap->writeRegisters(stream, offset, false, interface.getComponentRef());
			}
			else {
				memMap->writeRegisters(stream, offset, true, interface.getComponentRef());
			}
		}

		// if the slave contains a bridge to a master interface
		if (slave->hasBridge()) {

			// process each connected master-interface
			QStringList masterNames = slave->getMasterReferences();
			foreach (QString masterRef, masterNames) {
				
				// if the interface reference is not valid
				if (!comp->hasInterface(masterRef)) {
					continue;
				}

				// the interface for the connected master-bus interface
				Interface masterIF(interface.getComponentRef(), masterRef);

				// if the connected interface has already been processed before
				if (operatedInterfaces_.contains(masterIF)) {
					continue;
				}

				// add the interface to the list to avoid processing it again
				operatedInterfaces_.append(masterIF);

				parseInterface(offset, stream, masterIF);
			}
		}

		break;
								}
	case General::MIRROREDSLAVE: {
		// increase the offset by the remap address of the mirrored slave interface
		QString remapStr = comp->getBusInterface(interface.getBusRef())->getMirroredSlave()->getRemapAddress();

		// if the remap address is directly a number
		if (Utils::isNumber(remapStr)) {
			offset += Utils::str2Int(remapStr);
		}
		// if the remap address refers to a configurable element value
		else {

			// if the configurable element value is specified
			if (design_->hasConfElementValue(interface.getComponentRef(), remapStr)) {

				// increase the offset by the value set in the configurable elements
				QString confValue = design_->getConfElementValue(interface.getComponentRef(), remapStr);
				offset += Utils::str2Int(confValue);
			}
			// if the value is not set then use the default value from the component
			else {

				// increase the offset by the default value
				QString defValue = comp->getAllParametersDefaultValue(remapStr);
				offset += Utils::str2Int(defValue);
			}
		}

		// ask the design for interfaces that are connected to this interface
		QList<Interface> connected = design_->getConnectedInterfaces(interface);

		// all connected interfaces are processed
		foreach (Interface targetInterface, connected) {

			// if the connected interface has already been processed before
			if (operatedInterfaces_.contains(targetInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(targetInterface);

			parseInterface(offset, stream, targetInterface);
		}

		break;
										  }
	case General::MIRROREDMASTER: {
		// mirrored master interfaces are connected via channels
		// find the interfaces connected to the specified mirrored master interface
		QList<QSharedPointer<const BusInterface> > connectedInterfaces = comp->getChannelConnectedInterfaces(interface.getBusRef());

		// all interfaces that are connected via channel are processed
		foreach (QSharedPointer<const BusInterface> busif, connectedInterfaces) {
			Interface connectedInterface(interface.getComponentRef(), busif->getName());

			// if the interface connected via channel has already been processed before
			if (operatedInterfaces_.contains(connectedInterface)) {
				continue;
			}

			// add the interface to the list to avoid processing it again
			operatedInterfaces_.append(connectedInterface);

			parseInterface(offset, stream, connectedInterface);
		}
		break;
											}
	default: {
		return;
				}
	}
}

void MemoryMapHeaderGenerator::generateSystemHeaders(QSharedPointer<Component> comp,
	QSharedPointer<DesignConfiguration> desConf,
	QSharedPointer<Design> design ) {

	Q_ASSERT(comp);
	Q_ASSERT(desConf);
	Q_ASSERT(design);
	Q_ASSERT(utility_);

	// clear previous settings if any exist
	sysGenSettings_.clear();

	QSharedPointer<SystemView> sysView = comp->findSystemView(*desConf->getVlnv());
	Q_ASSERT(sysView);

	// file types of the files to add to systemHeaders includes
	QStringList usedFileTypes;
	usedFileTypes.append("cSource");
	usedFileTypes.append("cppSource");

	// find all CPU instances
	foreach (const ComponentInstance& instance, design->getComponentInstances()) {
		
		// parse the component for the instance
		VLNV instanceVLNV = instance.getComponentRef();
		QSharedPointer<const LibraryComponent> libComp = utility_->getLibraryInterface()->getModelReadOnly(instanceVLNV);
		QSharedPointer<const Component> instComponent = libComp.dynamicCast<const Component>();
		Q_ASSERT(instComponent);

		// if the instance is not CPU then move on to another instance
		if (!instComponent->isCpu()) {
			continue;
		}

		// create header settings object for the CPU instance
		SystemHeaderSaveModel::SysHeaderOptions opt(instance.getInstanceName(), instanceVLNV);

		opt.instanceId_ = instance.getUuid();

		// find the files of the instances active SW view
		QString activeView = desConf->getActiveView(instance.getInstanceName());
		if (!activeView.isEmpty() && instComponent->hasSWView(activeView)) {

			QSharedPointer<SWView> swView = instComponent->findSWView(activeView);
			QStringList fileSets = swView->getFileSetRefs();

			// the files that are included in the active view
			QStringList files = instComponent->getFilesFromFileSets(fileSets, usedFileTypes);

			// the source path for the relative file paths of the component
			QString sourcePath = utility_->getLibraryInterface()->getPath(instanceVLNV);

			// convert all relative paths from component to absolute path
			foreach (QString relPath, files) {
				QString absolutePath = General::getAbsolutePath(sourcePath, relPath);

				QFileInfo info(absolutePath);
				opt.includeFiles_.append(info);
			}
		}

		// add the settings for the CPU instance
		sysGenSettings_.append(opt);
	}

	// the model which manages the dialog contents
	SystemHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setObjects(comp, sysGenSettings_);

	// create the dialog to display the headers to be generated
	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	sysGenSettings_ = model.getObjects();

	searchInstanceFiles(comp, sysView->getHWViewRef());

	bool changed = false;

	// write the system headers for CPU instances
	foreach (const SystemHeaderSaveModel::SysHeaderOptions& opt, sysGenSettings_) {

		QFile file(opt.sysHeaderInfo_.absoluteFilePath());

		// make sure the directory structure exists for the file
		QDir dir(opt.sysHeaderInfo_.dir());
		if (!dir.exists()) {
			QString dirName(dir.dirName());
			dir.cdUp();
			dir.mkpath(dirName);
		}

		// open the file and erase all old contents if any exists

		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(opt.sysHeaderInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream stream(&file);

		stream << "/*" << endl;
		stream << " * File: " << opt.sysHeaderInfo_.fileName() << endl;
		stream << " * Created on: " << QDate::currentDate().toString("dd.MM.yyyy") << endl;

		QSettings settings;
		QString userName = settings.value("General/Username", Utils::getCurrentUser()).toString();

		stream << " * Generated by: " << userName << endl;
		stream << " *" << endl;
		stream << " * Description:" << endl;
		stream << " * This file includes the header files which are automatically generated by Kactus2." << endl;
		stream << " * This file allows indirect includes for generated files, thus allowing more flexible SW development." << endl;
		stream << " * Generated for system design: " << design->getVlnv()->toString() << endl;
		stream << " * Target instance: " << opt.instanceName_ << endl;
		stream << "*/" << endl;

		QString headerGuard(QString("%1_%2_H").arg(comp->getVlnv()->toString("_")).arg(opt.instanceName_));
		headerGuard = headerGuard.toUpper();

		stream << "#ifndef " << headerGuard << endl;
		stream << "#define " << headerGuard << endl << endl;

		// write include directive for each include file
		foreach (const QFileInfo& includeFile, opt.includeFiles_) {
			stream << "#include \"" << includeFile.absoluteFilePath() << "\"" << endl;
		}

		stream << endl << "#endif /* " << headerGuard << " */" << endl;

		file.close();

		changed = true;

		// add the generated header to the top component's file sets
		addHeaderFile(comp, opt.sysHeaderInfo_, opt.instanceName_, QStringList(), opt.instanceId_);
	}

	if (changed) {
		// save the changes to the file sets
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}

	// clear the settings after generation
	sysGenSettings_.clear();
}

void MemoryMapHeaderGenerator::searchInstanceFiles( QSharedPointer<const Component> component, const QString& hwViewName ) {
	
	// if the view is not found
	if (!component->hasView(hwViewName)) {
		return;
	}

	VLNV hierRef = component->getHierRef(hwViewName);

	// if the view is not hierarchical
	if (!hierRef.isValid()) {
		return;
	}

	QSharedPointer<const LibraryComponent> libDesConf = utility_->getLibraryInterface()->getModelReadOnly(hierRef);
	QSharedPointer<const DesignConfiguration> desConf = libDesConf.dynamicCast<const DesignConfiguration>();

	QSharedPointer<const Design> design;

	// if the hier ref was not for design configuration then try design
	if (!desConf) {
		design = libDesConf.dynamicCast<const Design>();
	}
	// if hier ref was for design conf then parse the design
	else {
		VLNV designVLNV = desConf->getDesignRef();
		QSharedPointer<const LibraryComponent> libDes = utility_->getLibraryInterface()->getModelReadOnly(designVLNV);
		design = libDes.dynamicCast<const Design>();
	}

	// if design was not found then nothing can be done
	if (!design) {
		return;
	}

	// list to add instances which should be searched hierarchically
	QList<MemoryMapHeaderGenerator::HierParsingInfo> instancesToParse;

	// file types of the files to add to systemHeaders includes
	QStringList usedFileTypes;
	usedFileTypes.append("cSource");
	usedFileTypes.append("cppSource");

	// check all instances of the design
	foreach (const ComponentInstance& instance, design->getComponentInstances()) {

		// no match for instance was yet found
		bool matched = false;

		// check if the instance matches one of the searched CPUs
		for (int i = 0; i < sysGenSettings_.size(); ++i) {
			SystemHeaderSaveModel::SysHeaderOptions& opt = sysGenSettings_[i];

			// if the CPU was already found before
			if (opt.found_) {
				continue;
			}
			
			// if the instance matches the searched CPU
			else if (instance.getUuid() == opt.instanceId_) {
				
				// CPU instance was found and do not need to be searched in other designs
				opt.found_ = true;

				// the instance matched and does not need to be searched lower in the hierarchy
				matched = true;

				QSharedPointer<const FileSet> instanceFileSet = component->findFileSetById(opt.instanceId_);
				// if the containing component does not contain file set for the instance
				if (!instanceFileSet) {
					break;
				}

				// get the relative paths to the files of specified file types
				QStringList files = instanceFileSet->getFiles(usedFileTypes);

				// the source path for the relative file paths of the component
				QString sourcePath = utility_->getLibraryInterface()->getPath(*component->getVlnv());

				// convert all relative paths from component to absolute path
				foreach (QString relPath, files) {

					QString absolutePath = General::getAbsolutePath(sourcePath, relPath);

					QFileInfo info(absolutePath);
					opt.includeFiles_.append(info);
				}

				// move on to next design instance to search for
				break;
			}
		}

		// if the instance did not match any of the CPU instances
		if (!matched) {

			// parse the component for the instance
			VLNV instanceVLNV = instance.getComponentRef();
			QSharedPointer<const LibraryComponent> libComp = utility_->getLibraryInterface()->getModelReadOnly(instanceVLNV);
			QSharedPointer<const Component> instComp = libComp.dynamicCast<const Component>();
			Q_ASSERT(instComp);

			// find the active view for the component instance
			QString activeView = desConf->getActiveView(instance.getInstanceName());

			// if there was no active view set
			if (activeView.isEmpty()) {
				QStringList hierViewNames = instComp->getHierViews();

				// if component only contains one hierarchical view then use it
				if (hierViewNames.size() == 1) {
					activeView = hierViewNames.first();
				}
				// otherwise it is unknown which view to use and move to next instance
				else {
					continue;
				}
			}

			// if active view was not found then should have moved on
			Q_ASSERT(!activeView.isEmpty());

			// add to the list of instances to parse hierarchically
			MemoryMapHeaderGenerator::HierParsingInfo info(instComp, activeView);
			instancesToParse.append(info);
		}
	}

	// check if more searching is needed
	bool allFound = true;
	foreach (const SystemHeaderSaveModel::SysHeaderOptions& opt, sysGenSettings_) {
		if (!opt.found_) {
			allFound = false;
		}
	}

	// if there is at least one more CPU instance to find
	if (!allFound) {
		foreach (MemoryMapHeaderGenerator::HierParsingInfo info, instancesToParse) {
			searchInstanceFiles(info.comp_, info.activeView_);
		}
	}
}

QList<IPlugin::ExternalProgramRequirement> MemoryMapHeaderGenerator::getProgramRequirements() {
	return QList<IPlugin::ExternalProgramRequirement>();
}


MemoryMapHeaderGenerator::HierParsingInfo::HierParsingInfo(QSharedPointer<const Component> comp, const QString& viewName ):
comp_(comp),
activeView_(viewName) {
}

MemoryMapHeaderGenerator::HierParsingInfo::HierParsingInfo( const HierParsingInfo& other ):
comp_(other.comp_),
activeView_(other.activeView_) {
}

MemoryMapHeaderGenerator::HierParsingInfo& MemoryMapHeaderGenerator::HierParsingInfo::operator=( const HierParsingInfo& other ) {
	if (&other != this) {
		comp_ = other.comp_;
		activeView_ = other.activeView_;
	}
	return *this;
}
