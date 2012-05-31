/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditor.cpp
 *		Project: Kactus 2
 */

#include "componenteditor.h"

#include "componenteditoritem.h"
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <vhdlGenerator/vhdlgenerator2.h>
#include <modelsimGenerator/modelsimgenerator.h>
#include <common/dialogs/comboSelector/comboselector.h>
#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <IPXactWrapper/ComponentEditor/itemvisualizer.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>

ComponentEditor::ComponentEditor(LibraryInterface* libHandler,
								 QSharedPointer<Component> component,
								 QWidget *parent):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
component_(component),
navigationSplitter_(Qt::Horizontal, this),
editorVisualizerSplitter_(Qt::Horizontal, &navigationSplitter_), 
navigationModel_(libHandler, this),
navigationView_(libHandler, *component->getVlnv(), &navigationSplitter_),
editorSlot_(&editorVisualizerSplitter_),
visualizerSlot_(&editorVisualizerSplitter_) {

	// these can be used when debugging to identify the objects
	setObjectName(tr("ComponentEditor"));
	navigationSplitter_.setObjectName(tr("NavigationSplitter"));
	editorVisualizerSplitter_.setObjectName(tr("EditorVisualizerSplitter"));
	editorSlot_.setObjectName(tr("EditorSlot"));
	visualizerSlot_.setObjectName(tr("VisualizerSlot"));

	Q_ASSERT(component_);
	Q_ASSERT(libHandler_);

	// set the name and type for the tab
	setDocumentName(QString("%1 (%2)").arg(component_->getVlnv()->getName()).arg(
		component_->getVlnv()->getVersion()));
	setDocumentType(tr("Component"));

	// the top layout contains only the navigation splitter
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&navigationSplitter_);
	layout->setContentsMargins(5, 5, 5, 5);

	// editor visualizer splitter contains the editor- and visualizer slots
	editorVisualizerSplitter_.addWidget(&editorSlot_);
	editorVisualizerSplitter_.addWidget(&visualizerSlot_);

	// navigation splitter contains the navigation tree and the other splitter
	navigationSplitter_.addWidget(&navigationView_);
	navigationSplitter_.addWidget(&editorVisualizerSplitter_);

	// set the component to be displayed in the navigation model
	navigationModel_.setComponent(component_);

	// connect the view with the model
	navigationView_.setModel(&navigationModel_);

	// The navigation tree takes 1/5 of the space available and editor and 
	// visualizer take the rest
	QList<int> navigationSize;
	navigationSize.append(ComponentTreeView::DEFAULT_WIDTH);
	navigationSize.append(4 * ComponentTreeView::DEFAULT_WIDTH);
	navigationSplitter_.setSizes(navigationSize);

	connect(&navigationView_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemActivated(const QModelIndex&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(onItemChanged()), Qt::UniqueConnection);

	// Open in unlocked mode by default only if the version is draft.
	setProtection(component_->getVlnv()->getVersion() != "draft");

	// when starting the component editor open the general editor
	onItemActivated(navigationModel_.index(0, 0, QModelIndex()));
}

ComponentEditor::~ComponentEditor() {
}

void ComponentEditor::setProtection( bool locked ) {
	TabDocument::setProtection(locked);

	// tell tree items to change the state of the editors
	navigationModel_.setLocked(locked);
}

VLNV ComponentEditor::getComponentVLNV() const {
	return *component_->getVlnv();
}

bool ComponentEditor::isHWImplementation() const {
	return true;
}

void ComponentEditor::refresh() {
	Q_ASSERT(!isModified());

	// remember the locked state
	bool locked = isProtected();
	
	// clear the slots 
	editorSlot_.setWidget(NULL);
	visualizerSlot_.setWidget(NULL);

	// rebuild the navigation tree
	navigationModel_.setComponent(component_);

	// the document is no longer modified
	setModified(false);
	TabDocument::refresh();

	// set the protection state to same as before refreshing
	setProtection(locked);

	// open the general editor
	onItemActivated(navigationModel_.index(0, 0, QModelIndex()));
}

bool ComponentEditor::validate( QStringList& errorList ) {
	return component_->isValid(errorList);
}

bool ComponentEditor::save() {
	// tell the editors to apply the changes to the model
	navigationModel_.makeEditorChanges();

	if (libHandler_->writeModelToFile(component_, false)) {
		return TabDocument::save();
	}
	else {
		emit errorMessage(tr("Component was not saved to disk."));
		return false;
	}
}

bool ComponentEditor::saveAs() {
	// tell the editors to apply the changes to the model
	navigationModel_.makeEditorChanges();

	// Ask the user for a new VLNV along with attributes and directory.
	KactusAttribute::ProductHierarchy prodHier = component_->getComponentHierarchy();
	KactusAttribute::Firmness firmness = component_->getComponentFirmness();

	VLNV vlnv;
	QString directory;

	if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, *component_->getVlnv(),
		prodHier, firmness, vlnv, directory)) {
		return false;
	}

	// save pointer to the old component
	QSharedPointer<Component> oldComponent = component_;

	// create copies of the objects so saving is not done to the original component
	component_ = QSharedPointer<Component>(new Component(*component_));

	// make sure the vlnv type is correct
	VLNV compVLNV = vlnv;
	compVLNV.setType(VLNV::COMPONENT);

	// update the vlnv
	component_->setVlnv(vlnv);

	// get the paths to the original xml file
	QFileInfo sourceInfo(libHandler_->getPath(*oldComponent->getVlnv()));
	QString sourcePath = sourceInfo.absolutePath();

	// update the file paths and copy necessary files
	component_->updateFiles(*oldComponent, sourcePath, directory);

	// Write the component to a file.
	if (libHandler_->writeModelToFile(directory, component_)) {
		setDocumentName(compVLNV.getName() + " (" + compVLNV.getVersion() + ")");
		return TabDocument::saveAs();
	}
	else {
		emit errorMessage(tr("Component was not saved to disk."));
		return false;
	}
}

void ComponentEditor::onItemActivated( const QModelIndex& index ) {

	ComponentEditorItem* item = static_cast<ComponentEditorItem*>(index.internalPointer());
	Q_ASSERT(item);

	QList<int> editorVisualizerSizes;

	ItemEditor* editor = item->editor();
	if (editor) {
		editorVisualizerSizes.append(1);
	}
	// if there is no editor then hide the editor slot
	else {
		editorVisualizerSizes.append(0);
	}
	editorSlot_.setWidget(editor);

	ItemVisualizer* visualizer = item->visualizer();
	if (visualizer) {
		editorVisualizerSizes.append(1);
	}
	// if there is no visualizer then hide the visualizer slot
	else {
		editorVisualizerSizes.append(0);
	}
	visualizerSlot_.setWidget(visualizer);

	editorVisualizerSplitter_.setSizes(editorVisualizerSizes);
}

bool ComponentEditor::onVhdlGenerate() {
	// if the component is hierarchical then it must be opened in design widget
	if (component_->isHierarchical()) {
		QMessageBox::information(this, tr("Kactus2 component editor"),
			tr("This component contains hierarchical views so you must open it"
			" in a design editor and run the vhdl generator there."));
		return false;
	}

	if (isModified() && askSaveFile()) {
		save();
	}

	QString fileName = libHandler_->getPath(*component_->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/");
	fileName += component_->getVlnv()->getName();
	fileName += QString(".vhd");

	QString path = QFileDialog::getSaveFileName(this,
		tr("Set the directory where the vhdl file is created to"),
		fileName, tr("Vhdl files (*.vhd)"));

	// if user clicks cancel then nothing is created
	if (path.isEmpty())
		return false;

	VhdlGenerator2 vhdlGen(libHandler_, this);

	// if errors are detected during parsing
	if (!vhdlGen.parse(component_, QString())) {
		return false;
	}

	// generate the vhdl code
	vhdlGen.generateVhdl(path);

	// check if the file already exists in the metadata
	QString basePath = libHandler_->getPath(*component_->getVlnv());
	QString relativePath = General::getRelativePath(basePath, path);
	if (!component_->hasFile(relativePath)) {

		// ask user if he wants to save the generated vhdl into object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated file to metadata?"),
			tr("Would you like to save the generated vhdl-file to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			// add a rtl view to the component_
			vhdlGen.addRTLView(path);

			libHandler_->writeModelToFile(component_);

			return true;
		}
	}

	return false;
}

bool ComponentEditor::onModelsimGenerate() {
	if (isModified() && askSaveFile()) {
		save();
	}

	// select a view to generate the modelsim script for
	QString viewName = ComboSelector::selectView(component_, this, QString(),
		tr("Select a view to generate the modelsim script for"));
	if (viewName.isEmpty()) {
		return false;
	}

	QString fileName = libHandler_->getPath(*component_->getVlnv());
	QFileInfo targetInfo(fileName);
	fileName = targetInfo.absolutePath();
	fileName += QString("/%1.%2.create_makefile").arg(
		component_->getVlnv()->getName()).arg(viewName);

	// ask user to select a location to save the makefile
	fileName = QFileDialog::getSaveFileName(this, 
		tr("Set the file name for the modelsim script."), fileName,
		tr("Modelsim scripts (*.do);;Shell cripts (*.sh)"));

	// if user clicked cancel
	if (fileName.isEmpty())
		return false;

	// construct the generator
	ModelsimGenerator generator(libHandler_, this);
	connect(&generator, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&generator, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	// parse the component and view / sub-designs
	generator.parseFiles(component_, viewName);

	// create the script file
	generator.generateMakefile(fileName);

	// check if the file already exists in the metadata
	QString basePath = libHandler_->getPath(*component_->getVlnv());
	QString relativePath = General::getRelativePath(basePath, fileName);
	if (!component_->hasFile(relativePath)) {

		// ask user if he wants to save the generated modelsim script into 
		// object metadata
		QMessageBox::StandardButton button = QMessageBox::question(this, 
			tr("Save generated modelsim script to metadata?"),
			tr("Would you like to save the generated modelsim script to IP-Xact"
			" metadata?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

		// if the generated file is saved
		if (button == QMessageBox::Yes) {

			QString xmlPath = libHandler_->getPath(*component_->getVlnv());

			// if the file was successfully added to the library
			if (generator.addMakefile2IPXact(component_, fileName, xmlPath)) {
				libHandler_->writeModelToFile(component_);
				return true;
			}
		}
	}
	return false;
}

void ComponentEditor::onItemChanged() {

	// this document has now been modified
	TabDocument::setModified(true);
	emit contentChanged();
}
