//-----------------------------------------------------------------------------
// File: vlnveditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 7.2.2011
//
// Description:
// VLNV editor group box.
//-----------------------------------------------------------------------------

#include "vlnveditor.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>
#include <common/validators/nameValidator/namevalidator.h>
#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/libraryitem.h>
#include <LibraryManager/libraryhandler.h>

#include <models/librarycomponent.h>
#include <models/abstractiondefinition.h>
#include <models/busdefinition.h>

#include "VLNVContentMatcher.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMimeData>
#include <QSharedPointer>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::VLNVEditor(VLNV::IPXactType type, 
					   LibraryInterface* libHandler, 
					   QWidget* parentWnd,
					   QWidget* parent,
					   bool compact):
QGroupBox(parent),
type_(type),
dataTree_(),
vendorEdit_(0),
vendorMatcher_(),
libraryEdit_(0),
libraryMatcher_(),
nameEdit_(0),
nameMatcher_(),
versionEdit_(0),
versionMatcher_(),
handler_(libHandler)
{
    Q_ASSERT(libHandler != 0);
    Q_ASSERT(type_ != VLNV::INVALID);

    // Set group box settings.
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setTitle(tr("VLNV"));

    // Initialize the widgets and the connections between widgets.
    initWidgets(parentWnd, compact);
    initConnections();

	// accept drops from drag & drop
	setAcceptDrops(true);

    updateFiltering();
}

//-----------------------------------------------------------------------------
// Function: ~VLNVEditor()
//-----------------------------------------------------------------------------
VLNVEditor::~VLNVEditor()
{
}

//-----------------------------------------------------------------------------
// Function: setVLNV()
//-----------------------------------------------------------------------------
void VLNVEditor::setVLNV(VLNV const* vlnv)
{
    // Set the fields according to the VLNV.
    vendorEdit_->setText(vlnv->getVendor());
    libraryEdit_->setText(vlnv->getLibrary());
    nameEdit_->setText(vlnv->getName());
    versionEdit_->setText(vlnv->getVersion());

    // Update the matcher items.
    updateLibraryMatcherItem();
}

void VLNVEditor::setVLNV( const VLNV& vlnv ) {
	setVLNV(&vlnv);
}

//-----------------------------------------------------------------------------
// Function: getVLNV()
//-----------------------------------------------------------------------------
VLNV VLNVEditor::getVLNV() const
{
    return VLNV(type_, vendorEdit_->text(), libraryEdit_->text(),
                nameEdit_->text(), versionEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool VLNVEditor::isValid() const
{
    return (!vendorEdit_->text().isEmpty() && !libraryEdit_->text().isEmpty() &&
            !nameEdit_->text().isEmpty() && !versionEdit_->text().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: updateMatcherItems()
//-----------------------------------------------------------------------------
void VLNVEditor::updateMatcherItems()
{
    vendorMatcher_->setDataNode(&dataTree_);
    updateLibraryMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: updateLibraryMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateLibraryMatcherItem()
{
    VLNVDataNode const* foundItem = dataTree_.findChild(vendorEdit_->text());
    libraryMatcher_->setDataNode(foundItem);

    // Update "recursively" also the name matcher item.
    updateNameMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: updateNameMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateNameMatcherItem()
{
    VLNVDataNode const* libItem = libraryMatcher_->getDataNode();
    VLNVDataNode const* foundItem = 0;

    if (libItem != 0)
    {
        foundItem = libItem->findChild(libraryEdit_->text());
    }

    nameMatcher_->setDataNode(foundItem);

    // Update "recursively" also the version matcher item.
    updateVersionMatcherItem();
}

//-----------------------------------------------------------------------------
// Function: updateVersionMatcherItem()
//-----------------------------------------------------------------------------
void VLNVEditor::updateVersionMatcherItem()
{
    VLNVDataNode const* nameItem = nameMatcher_->getDataNode();
    VLNVDataNode const* foundItem = 0;

    if (nameItem != 0)
    {
        foundItem = nameItem->findChild(nameEdit_->text());
    }

    versionMatcher_->setDataNode(foundItem);
}

//-----------------------------------------------------------------------------
// Function: initWidgets()
//-----------------------------------------------------------------------------
void VLNVEditor::initWidgets(QWidget* parentWnd, bool compact)
{
    // Create the labels.
    QLabel* vendorLabel = new QLabel(tr("Vendor:"), this);
    QLabel* libraryLabel = new QLabel(tr("Library:"), this);
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    QLabel* versionLabel = new QLabel(tr("Version:"), this);

    // Create the matchers.
    vendorMatcher_ = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(handler_));
    libraryMatcher_ = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(handler_));
    nameMatcher_ = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(handler_));
    versionMatcher_ = QSharedPointer<VLNVContentMatcher>(new VLNVContentMatcher(handler_));
    
    // Create the line edits.
    vendorEdit_ = new AssistedLineEdit(vendorMatcher_, parentWnd, this);
    vendorEdit_->setValidator(new NameValidator(this));
	vendorEdit_->setProperty("mandatoryField", true);

    libraryEdit_ = new AssistedLineEdit(libraryMatcher_, parentWnd, this);
    libraryEdit_->setValidator(new NameValidator(this));
	libraryEdit_->setProperty("mandatoryField", true);

    nameEdit_ = new AssistedLineEdit(nameMatcher_, parentWnd, this);
	nameEdit_->setValidator(new NameValidator(this, true));
	nameEdit_->setProperty("mandatoryField", true);

    versionEdit_ = new AssistedLineEdit(versionMatcher_, parentWnd, this);
	versionEdit_->setValidator(new NameValidator(this, true));
	versionEdit_->setProperty("mandatoryField", true);
    
    // Create the layout and add the widgets to it.
    if (compact)
    {
        QGridLayout* layout = new QGridLayout(this);
        layout->addWidget(vendorLabel, 0, 0, 1, 1);
        layout->addWidget(vendorEdit_, 0, 1, 1, 1);
        layout->addWidget(libraryLabel, 1, 0, 1, 1);
        layout->addWidget(libraryEdit_, 1, 1, 1, 1);
        layout->addWidget(nameLabel, 2, 0, 1, 1);
        layout->addWidget(nameEdit_, 2, 1, 1, 1);
        layout->addWidget(versionLabel, 3, 0, 1, 1);
        layout->addWidget(versionEdit_, 3, 1, 1, 1);
    }
    else
    {
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(vendorLabel);
        layout->addWidget(vendorEdit_);
        layout->addWidget(libraryLabel);
        layout->addWidget(libraryEdit_);
        layout->addWidget(nameLabel);
        layout->addWidget(nameEdit_);
        layout->addWidget(versionLabel);
        layout->addWidget(versionEdit_);
    }
}

//-----------------------------------------------------------------------------
// Function: initConnections()
//-----------------------------------------------------------------------------
void VLNVEditor::initConnections()
{
    // Connect the signals informing of changes in items.
    connect(vendorEdit_, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(versionEdit_, SIGNAL(textChanged(const QString&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    // Connect the signals informing the editing of items.
    connect(vendorEdit_, SIGNAL(textEdited(const QString&)),
        this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(textEdited(const QString&)),
        this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(textEdited(const QString&)),
        this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);
    connect(versionEdit_, SIGNAL(textEdited(const QString&)),
        this, SIGNAL(vlnvEdited()), Qt::UniqueConnection);

    // Connect the matcher update slots to the textChanged signals.
    connect(vendorEdit_, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateMatcherItems()), Qt::UniqueConnection);
    connect(vendorEdit_, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateLibraryMatcherItem()), Qt::UniqueConnection);
    connect(libraryEdit_, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateNameMatcherItem()), Qt::UniqueConnection);
    connect(nameEdit_, SIGNAL(textChanged(const QString&)),
        this, SLOT(updateVersionMatcherItem()), Qt::UniqueConnection);

    connect(static_cast<LibraryHandler*>(handler_), SIGNAL(refreshDialer()),
            this, SLOT(updateMatcherItems()), Qt::UniqueConnection);
}

bool VLNVEditor::isEmpty() const {
	return (vendorEdit_->text().isEmpty() && libraryEdit_->text().isEmpty() &&
		nameEdit_->text().isEmpty() && versionEdit_->text().isEmpty());
}

void VLNVEditor::setVendor( const QString& vendor ) {
	vendorEdit_->setText(vendor);
}

void VLNVEditor::setLibrary( const QString& library ) {
	libraryEdit_->setText(library);
}

void VLNVEditor::setName( const QString& name ) {
	nameEdit_->setText(name);
}

void VLNVEditor::setVersion( const QString& version ) {
	versionEdit_->setText(version);
}

void VLNVEditor::dropEvent( QDropEvent* event ) {
	// Retrieve the vlnv.
	VLNV *vlnv;
	memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));
	setVLNV(vlnv);
	event->acceptProposedAction();

	// for abs def and bus def there is additional option to set the paired vlnv editor
	switch (handler_->getDocumentType(*vlnv)) 
	{
	case VLNV::BUSDEFINITION: {
		
		// if there is only one abs def for the dropped bus def
		QList<VLNV> absDefVLNVs;
		if (handler_->getChildren(absDefVLNVs, *vlnv) == 1) {
			emit setAbsDef(absDefVLNVs.first());
		}

		// if there are more than one abs defs for the bus def
		else if (absDefVLNVs.size() > 1) {

			// if the signal is connected then inform user that the abs def must be
			// selected manually
			if (receivers(SIGNAL(setAbsDef(const VLNV&))) > 0) {
				QMessageBox::information(this, QCoreApplication::applicationName(),
					tr("More than one abstraction definitions exist for the dropped"
					" bus definition. Select one manually from the library."),
					QMessageBox::Close, QMessageBox::Close);
			}
		}
		break;
							  }
	case VLNV::ABSTRACTIONDEFINITION: {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(*vlnv);
		QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();
		Q_ASSERT(absDef);

		VLNV busDefVLNV = absDef->getBusType();
		emit setBusDef(busDefVLNV);
		break;
									  }
	
	}

    emit vlnvEdited();
}

void VLNVEditor::dragEnterEvent( QDragEnterEvent* event ) {
	if (event->mimeData()->hasFormat("data/vlnvptr")) {
		
		// Retrieve the vlnv.
		VLNV *vlnv;
		memcpy(&vlnv, event->mimeData()->data("data/vlnvptr").data(), sizeof(vlnv));

		// if the vlnv is of correct type
		if (handler_->getDocumentType(*vlnv) == type_)
			event->acceptProposedAction();
	}
}

//-----------------------------------------------------------------------------
// Function: setFirmnessFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setFirmnessFilter(bool on, KactusAttribute::Firmness firmness /*= KTS_TEMPLATE*/)
{
    dataTree_.setFirmnessFilter(on, firmness);
}

//-----------------------------------------------------------------------------
// Function: setHierarchyFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setHierarchyFilter(bool on, KactusAttribute::ProductHierarchy productHier /*= KactusAttribute::KTS_IP*/)
{
    dataTree_.setHierarchyFilter(on, productHier);
}

//-----------------------------------------------------------------------------
// Function: setImplementationFilter()
//-----------------------------------------------------------------------------
void VLNVEditor::setImplementationFilter(bool on, KactusAttribute::Implementation implementation /*= KTS_HW*/)
{
    dataTree_.setImplementationFilter(on, implementation);
}

//-----------------------------------------------------------------------------
// Function: updateFiltering()
//-----------------------------------------------------------------------------
void VLNVEditor::updateFiltering()
{
    dataTree_.clear();
    dataTree_.parse(handler_, type_);
}

void VLNVEditor::setMandatory( bool mandatory ) {
	vendorEdit_->setProperty("mandatoryField", mandatory);
	libraryEdit_->setProperty("mandatoryField", mandatory);
	nameEdit_->setProperty("mandatoryField", mandatory);
	versionEdit_->setProperty("mandatoryField", mandatory);
}
