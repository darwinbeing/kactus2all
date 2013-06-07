/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "alterabspgenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>
#include <models/component.h>
#include <common/KactusAttribute.h>
#include "alterabspgeneratordialog.h"
#include <models/SWView.h>
#include <models/generaldeclarations.h>

#include <QtPlugin>
#include <QDir>

#include <QDebug>

AlteraBSPGenerator::AlteraBSPGenerator():
utility_(NULL) {
}

AlteraBSPGenerator::~AlteraBSPGenerator() {
}

QString const& AlteraBSPGenerator::getName() const {
	static QString name(tr("Altera BSP Generator"));
	return name;
}

QString const& AlteraBSPGenerator::getVersion() const {
	static QString version(tr("1.0"));
	return version;
}

QString const& AlteraBSPGenerator::getDescription() const {
	static QString description(tr("Generates board support package for CPU component."));
	return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* AlteraBSPGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon AlteraBSPGenerator::getIcon() const
{
    return QIcon(":icons/BSPicon24.png");
}

bool AlteraBSPGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
	QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/,
	QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/ ) const {
	
	// BSP package can only be run on component editor 
	if (libDesConf || libDes) {
		return false;
	}

	QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();

    if (comp == 0)
    {
        return false;
    }
	
	switch (comp->getComponentImplementation()) {
	
		// HW component must be CPU and contain at least one SW view which specifies the BSP command
	case KactusAttribute::KTS_HW: {
		return comp->hasSWViews() && comp->isCpu();
											}
	// only HW components can contain BSP
	default: {
		return false;
				}
	}

	// generator can be run if component contains at least one sw view
	return comp->hasSWViews();
}

void AlteraBSPGenerator::runGenerator( IPluginUtility* utility,
	QSharedPointer<LibraryComponent> libComp,
	QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
	QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ ) {

	utility_ = utility;
	Q_ASSERT(utility_);

	Q_ASSERT(libComp);
	Q_ASSERT(!libDesConf);
	Q_ASSERT(!libDes);

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);
	
	AlteraBSPGeneratorDialog dialog(utility_->getLibraryInterface(), comp, utility_->getParentWidget());

	int result = dialog.exec();

	// if user did not want to add the generated files to the component metadata.
	if (result == QDialog::Rejected) {
		return;
	}

	// the path to the component's xml file for creating the relative paths.
	QString xmlPath = utility_->getLibraryInterface()->getPath(*comp->getVlnv());

	// contains the file type settings for adding files
	QSettings settings;

	// if nothing is changed then no saving is necessary
	bool modified = false;

	// package the generated files to component metadata
	const QList<AlteraBSPGeneratorDialog::GenerationOptions>& genOptions = dialog.getCreatedDirs();
	foreach (const AlteraBSPGeneratorDialog::GenerationOptions& opt, genOptions) {

		modified = true;

		QString fileSetName = QString("%1_bsp").arg(opt.swViewName_);

		// find the file set and if none exists then create one
		QSharedPointer<FileSet> fileSet = comp->getFileSet(fileSetName);
		fileSet->setGroups("generatedFiles");
		fileSet->setDescription(tr("Contains the BSP files generated by Altera tools for SW view %1").arg(opt.swViewName_));

		// find the contents of the directory
		QDir genDir(opt.dirPath_);
		QFileInfoList entries = genDir.entryInfoList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files | QDir::Dirs);

		// search the file system for generated files
		foreach (QFileInfo entry, entries) {
			addEntry(entry, xmlPath, fileSet, settings);
		}

		QSharedPointer<SWView> swView = comp->findSWView(opt.swViewName_);
		Q_ASSERT(swView);
		swView->addFileSetRef(fileSetName);
	}

	if (modified) {
		// save the changes to the file sets and SW views
		utility_->getLibraryInterface()->writeModelToFile(comp);
	}
}

void AlteraBSPGenerator::addEntry(const QFileInfo &entry,
	const QString& xmlPath,
	QSharedPointer<FileSet> fileSet,
	QSettings& settings) {

	// unexisting entries are not packaged
	if (!entry.exists()) {
		return;
	}

	QString relPath = General::getRelativePath(xmlPath, entry.absoluteFilePath());
	Q_ASSERT(!relPath.isEmpty());

	if (entry.isFile()) {
		fileSet->addFile(relPath, settings);
	}
	else if (entry.isDir()) {
		fileSet->addDependency(relPath);
	}
}

QList<IPlugin::ExternalProgramRequirements> AlteraBSPGenerator::getProgramRequirements() {
	QList<IPlugin::ExternalProgramRequirements> list;

	// windows uses the batch file to run cygwin
#ifdef Q_OS_WIN32
	IPlugin::ExternalProgramRequirements batchFile;
	batchFile.name_ = AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::WIN_PATH];
	batchFile.filters_ = tr("Batch files (*.bat)");
	batchFile.description_ = tr("The batch file which starts the Nios II command shell for windows. "
		"Usually named as \"Nios II Command Shell.bat\".");
	list.append(batchFile);

	// others run the shell script
#else
	IPlugin::ExternalProgramRequirements shellFile;
	shellFile.name_ = AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::LINUX_PATH];
	shellFile.filters_ = tr("Shell scripts (*.sh)");
	shellFile.description_ = tr("The shell script which sets the environment variables for "
		"Nios II Command shell. Usually named as \"nios2_command_shell.sh\"");
	list.append(shellFile);
#endif

	return list;
}
