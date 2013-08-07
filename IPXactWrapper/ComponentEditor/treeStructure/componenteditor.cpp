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
#include <IPXactWrapper/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QApplication>

#include <QDebug>

ComponentEditor::ComponentEditor(LibraryInterface* libHandler,
                                 PluginManager& pluginMgr,
								 QSharedPointer<Component> component,
								 QWidget *parent):
TabDocument(parent, DOC_PROTECTION_SUPPORT),
libHandler_(libHandler),
component_(component),
navigationSplitter_(Qt::Horizontal, this),
editorVisualizerSplitter_(Qt::Horizontal, &navigationSplitter_), 
navigationModel_(libHandler, pluginMgr, this, parent),
navigationView_(libHandler, *component->getVlnv(), &navigationSplitter_),
proxy_(this),
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

    if (component_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
	    setDocumentType(tr("HW Component"));
    }
    else if (component_->getComponentImplementation() == KactusAttribute::KTS_SW)
    {
        setDocumentType(tr("SW Component"));
    }
    else if (component_->getComponentImplementation() == KactusAttribute::KTS_SYS)
    {
        setDocumentType(tr("Unmapped System"));
    }

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
    navigationSplitter_.setStretchFactor(1, 1);

	// set the component to be displayed in the navigation model
	navigationModel_.setComponent(component_);

    // Set source model for the proxy.
    proxy_.setSourceModel(&navigationModel_);

	// connect the view with the model (proxy) and sort.
	navigationView_.setModel(&proxy_);
    navigationView_.sortByColumn(0, Qt::AscendingOrder);

	// The navigation tree takes 1/5 of the space available and editor and 
	// visualizer take the rest
	QList<int> navigationSize;
	navigationSize.append(ComponentTreeView::DEFAULT_WIDTH);
	navigationSize.append(4 * ComponentTreeView::DEFAULT_WIDTH);
	navigationSplitter_.setSizes(navigationSize);

	// when starting the component editor open the general editor.
	onItemActivated(proxy_.index(0, 0, QModelIndex()));

	// navigation model may request an item to be expanded
	connect(&navigationModel_, SIGNAL(expandItem(const QModelIndex&)),
		&navigationView_, SLOT(expand(const QModelIndex&)), Qt::UniqueConnection);

	connect(&navigationView_, SIGNAL(activated(const QModelIndex&)),
		this, SLOT(onItemActivated(const QModelIndex&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		    this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&navigationModel_, SIGNAL(contentChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&navigationModel_, SIGNAL(helpUrlRequested(QString const&)),
            this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(errorMessage(const QString&)),
		this, SLOT(onErrorDialog(const QString&)), Qt::UniqueConnection);

    connect(&navigationModel_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            this, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

	connect(&navigationModel_, SIGNAL(selectItem(const QModelIndex&)),
		this, SLOT(onNavigationTreeSelection(const QModelIndex&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openDesign(const VLNV&, const QString&)),
		this, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openBus(const VLNV&, const VLNV&)),
		this, SIGNAL(openBus(const VLNV&, const VLNV&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openComDefinition(const VLNV&)),
		this, SIGNAL(openComDefinition(const VLNV&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		this, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(&navigationModel_, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		this, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);

    addRelatedVLNV(*component_->getVlnv());

	// Open in unlocked mode by default only if the version is draft.
	setProtection(component_->getVlnv()->getVersion() != "draft");
}

ComponentEditor::~ComponentEditor() {
}

void ComponentEditor::setProtection( bool locked ) {
	TabDocument::setProtection(locked);

	// tell tree items to change the state of the editors
	navigationModel_.setLocked(locked);
}

VLNV ComponentEditor::getDocumentVLNV() const {
	return *component_->getVlnv();
}

bool ComponentEditor::isHWImplementation() const {
	return component_->getComponentImplementation() == KactusAttribute::KTS_HW;
}

void ComponentEditor::refresh() {

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	// remember the locked state
	bool locked = isProtected();
	
	// clear the slots 
	editorSlot_.setWidget(NULL);
	visualizerSlot_.setWidget(NULL);

	// get the VLNV of the component
	VLNV compVLNV = *component_->getVlnv();

	// get the original model of the component
	QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(compVLNV);
	Q_ASSERT(libHandler_->getDocumentType(compVLNV) == VLNV::COMPONENT);
	QSharedPointer<Component> comp = libComp.staticCast<Component>();

	// rebuild the navigation tree
	navigationModel_.setComponent(comp);
	component_.clear();
	component_ = comp;

	// open the general editor.
	onItemActivated(proxy_.index(0, 0, QModelIndex()));

	// the document is no longer modified
	setModified(false);
	TabDocument::refresh();

	// set the protection state to same as before refreshing
	setProtection(locked);

	QApplication::restoreOverrideCursor();
}

bool ComponentEditor::validate( QStringList& errorList ) {
	return component_->isValid(errorList);
}

bool ComponentEditor::save() {

	if (libHandler_->writeModelToFile(component_, false)) {
		return TabDocument::save();
	}
	else {
		emit errorMessage(tr("Component was not saved to disk."));
		return false;
	}
}

bool ComponentEditor::saveAs() {

	// Ask the user for a new VLNV along with attributes and directory.
	KactusAttribute::ProductHierarchy prodHier = component_->getComponentHierarchy();
	KactusAttribute::Firmness firmness = component_->getComponentFirmness();

	VLNV vlnv;
	QString directory;

    if (component_->getComponentImplementation() == KactusAttribute::KTS_HW)
    {
	    if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, *component_->getVlnv(),
		                                   prodHier, firmness, vlnv, directory))
        {
		    return false;
	    }
    }
    else
    {
        if (!NewObjectDialog::saveAsDialog(parentWidget(), libHandler_, *component_->getVlnv(), vlnv, directory))
        {
                return false;
        }
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
    component_->setComponentHierarchy(prodHier);
    component_->setComponentFirmness(firmness);

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

    clearRelatedVLNVs();
    addRelatedVLNV(compVLNV);
}

void ComponentEditor::onNavigationTreeSelection( const QModelIndex& index ) {
	navigationView_.setCurrentIndex(index);
	onItemActivated(index);
}

void ComponentEditor::onItemActivated( const QModelIndex& index ) {

    // If tree proxy model index is used, the item must be retrieved from the source model.
    const ComponentEditorTreeProxyModel* indexModel = dynamic_cast<const ComponentEditorTreeProxyModel*>(index.model());
    ComponentEditorItem* item = 0;
    if ( indexModel == 0 )
    {
        item = static_cast<ComponentEditorItem*>(index.internalPointer());
    }
    else
    {
        item = static_cast<ComponentEditorItem*>(proxy_.mapToSource(index).internalPointer());
    }

    Q_ASSERT(item);

	QList<int> editorVisualizerSizes;

	ItemEditor* editor = item->editor();
	if (editor) {

		// the width of the previous editor
		QWidget* prevEditor = editorSlot_.getWidget();
		if (prevEditor) {
			int prevWidth = prevEditor->size().width();
			editorVisualizerSizes.append(prevWidth);
		}
		// if there was no previous editor then use the size hint
		else {
			editorVisualizerSizes.append(editor->sizeHint().width());
		}
		editor->refresh();
	}
	// if there is no editor then hide the editor slot
	else {
		editorVisualizerSizes.append(0);
	}
	editorSlot_.setWidget(editor);

	ItemVisualizer* visualizer = item->visualizer();
	if (visualizer) {

		// the width of the previous visualizer
		QWidget* prevVisualizer = visualizerSlot_.getWidget();
		if (prevVisualizer) {
			int prevWidth = prevVisualizer->size().width();
			editorVisualizerSizes.append(prevWidth);
		}
		// if there was no previous visualizer then use the size hint
		else {
			editorVisualizerSizes.append(visualizer->sizeHint().width());
		}
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

VLNV ComponentEditor::getIdentifyingVLNV() const {
	return getDocumentVLNV();
}
