/* 
 *  	Created on: 14.10.2011
 *      Author: Antti Kamppi
 * 		filename: connectioneditor.cpp
 *		Project: Kactus 2
 */

#include "connectioneditor.h"

#include "AdHocBoundsDelegate.h"

#include <designEditors/HWDesign/HWConnection.h>
#include <designEditors/HWDesign/HWConnectionEndpoint.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>
#include <designEditors/HWDesign/HWChangeCommands.h>
#include <designEditors/HWDesign/HWComponentItem.h>

#include <library/LibraryManager/libraryinterface.h>
#include <library/LibraryManager/vlnv.h>

#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/abstractiondefinition.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/ApiInterface.h>
#include <IPXACTmodels/ComInterface.h>

#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/GenericEditProvider.h>
#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>

#include <QVBoxLayout>
#include <QSharedPointer>
#include <QHeaderView>
#include <QList>
#include <QStringList>
#include <QBrush>
#include <QDockWidget>
#include <QSortFilterProxyModel>

#include <QDebug>

//! \brief The maximum height for the description editor.
static const int MAX_DESC_HEIGHT = 50;

ConnectionEditor::ConnectionEditor(QWidget *parent, LibraryInterface* handler):
QWidget(parent),
type_(this),
absType_(this),
instanceLabel_(tr("Connected interfaces:"), this),
connectedInstances_(this),
separator_(this),
nameLabel_(tr("Connection name:"), this),
nameEdit_(this),
descriptionLabel_(tr("Description:"), this),
descriptionEdit_(this),
portsLabel_(tr("Connected physical ports:"), this),
portWidget_(this),
dummyWidget_(this),
connection_(NULL),
handler_(handler),
adHocBoundsTable_(this),
adHocBoundsModel_(this)
{
	Q_ASSERT(parent);
	Q_ASSERT(handler);

	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	type_.setTitle(tr("Bus type VLNV"));
	type_.setFlat(false);
	absType_.setTitle(tr("Abstraction type VLNV"));
	absType_.setFlat(false);

	separator_.setFlat(true);

	// set validator for name edit
	nameEdit_.setValidator(new VhdlNameValidator(&nameEdit_));

	// There are always 2 columns.
	portWidget_.setColumnCount(2);
	portWidget_.setHorizontalHeaderItem(0, new QTableWidgetItem(tr("instance1")));
	portWidget_.setHorizontalHeaderItem(1, new QTableWidgetItem(tr("instance2")));
	portWidget_.horizontalHeader()->setStretchLastSection(true);
	portWidget_.horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	portWidget_.verticalHeader()->hide();
	portWidget_.setEditTriggers(QAbstractItemView::NoEditTriggers);

	// set the maximum height for the description editor
	descriptionEdit_.setMaximumHeight(MAX_DESC_HEIGHT);

    // Set settings for the table view.
    adHocBoundsTable_.setSortingEnabled(true);
    adHocBoundsTable_.setSelectionMode(QAbstractItemView::SingleSelection);
    adHocBoundsTable_.setItemDelegate(new AdHocBoundsDelegate(this));
    adHocBoundsTable_.verticalHeader()->hide();

    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(&adHocBoundsModel_);
    adHocBoundsTable_.setModel(proxy);

    adHocBoundsTable_.setColumnWidth(ADHOC_BOUNDS_COL_LEFT, 70);
    adHocBoundsTable_.setColumnWidth(ADHOC_BOUNDS_COL_RIGHT, 70);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(ADHOC_BOUNDS_COL_LEFT, QHeaderView::Fixed);
    adHocBoundsTable_.horizontalHeader()->setSectionResizeMode(ADHOC_BOUNDS_COL_RIGHT, QHeaderView::Fixed);


	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&type_);
	layout->addWidget(&absType_);
	layout->addWidget(&instanceLabel_);
	layout->addWidget(&connectedInstances_);
	layout->addWidget(&separator_);
	layout->addWidget(&nameLabel_);
	layout->addWidget(&nameEdit_);
	layout->addWidget(&descriptionLabel_);
	layout->addWidget(&descriptionEdit_);
	layout->addWidget(&portsLabel_);
	layout->addWidget(&portWidget_, 1);
    layout->addWidget(&adHocBoundsTable_, 1);
    layout->addWidget(&dummyWidget_, 1);

	clear();
}

ConnectionEditor::~ConnectionEditor() {
}

void ConnectionEditor::clear() {
	if (connection_) {
		disconnect(connection_, SIGNAL(destroyed(GraphicsConnection*)),
			this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
		connection_ = 0;
	}

	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)));
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));

	// clear the contents of the editors
	type_.setVLNV(VLNV(), true);
	absType_.setVLNV(VLNV(), true);
	connectedInstances_.clear();
	nameEdit_.clear();
	descriptionEdit_.clear();
	portWidget_.clearContents();
    adHocBoundsModel_.setConnection(0);

	// set objects as hidden
	type_.hide();
	absType_.hide();
	instanceLabel_.hide();
	connectedInstances_.hide();
	separator_.hide();
	nameLabel_.hide();
	nameEdit_.hide();
	descriptionLabel_.hide();
	descriptionEdit_.hide();
	portsLabel_.hide();
	portWidget_.hide();
    adHocBoundsTable_.hide();

	parentWidget()->setMaximumHeight(20);
}

void ConnectionEditor::refresh() {
	Q_ASSERT(connection_);
	setConnection(connection_);
}

void ConnectionEditor::setConnection( GraphicsConnection* connection ) {
	Q_ASSERT(connection);

	parentWidget()->raise();

	// disconnect the previous connection
	if (connection_) {
		disconnect(connection_, SIGNAL(destroyed(GraphicsConnection*)),
			this, SLOT(clear()));
		disconnect(connection_, SIGNAL(contentChanged()),
			this, SLOT(refresh()));
	}

	connection_ = connection;

	ConnectionEndpoint* endpoint1 = connection->endpoint1();
    ConnectionEndpoint* endpoint2 = connection->endpoint2();
	Q_ASSERT(endpoint1);

    if (endpoint1->isCom())
    {
        type_.setTitle(tr("COM type VLNV"));

        if (endpoint2->isCom())
        {
            type_.setVLNV(endpoint1->getComInterface()->getComType(), true);
        }
        else
        {
            type_.setVLNV(VLNV(), true);
        }
    }
    else if (endpoint1->isApi())
    {
        type_.setTitle(tr("API type VLNV"));

        if (endpoint2->isApi())
        {
            type_.setVLNV(endpoint1->getApiInterface()->getApiType(), true);
        }
        else
        {
            type_.setVLNV(VLNV(), true);
        }
    }
    else if (endpoint1->isBus())
    {
        type_.setTitle(tr("Bus type VLNV"));
	    type_.setVLNV(endpoint1->getBusInterface()->getBusType(), true);
	    absType_.setVLNV(endpoint1->getBusInterface()->getAbstractionType(), true);
    }
    else if (endpoint1->isAdHoc())
    {
        adHocBoundsModel_.setConnection(static_cast<HWConnection*>(connection_));
        adHocBoundsTable_.resizeRowsToContents();
    }

	QString endpoint1Name = endpoint1->name();
	QString endpoint2Name = connection->endpoint2()->name();

	// set the names of the connected instances
	connectedInstances_.setText(QString("%1 - %2").arg(endpoint1Name).arg(endpoint2Name));

	// set text for the name editor, signal must be disconnected when name is set 
	// to avoid loops 
	disconnect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)));
	nameEdit_.setText(connection->name());
	connect(&nameEdit_, SIGNAL(textEdited(const QString&)),
		this, SLOT(onNameChanged(const QString&)), Qt::UniqueConnection);

	// display the current description of the interface.
	disconnect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()));
	descriptionEdit_.setPlainText(connection->description());
	connect(&descriptionEdit_, SIGNAL(textChanged()),
		this, SLOT(onDescriptionChanged()), Qt::UniqueConnection);

	connect(connection, SIGNAL(destroyed(GraphicsConnection*)),
		this, SLOT(clear()), Qt::UniqueConnection);
	connect(connection, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);

    if (endpoint1->isBus())
    {
	    setPortMaps();
    }

    bool locked = static_cast<DesignDiagram*>(connection->scene())->isProtected();
	
	// if either end point is hierarchical then there is no description to set
    if (connection_->getConnectionType() != ConnectionEndpoint::ENDPOINT_TYPE_ADHOC &&
        (endpoint1->isHierarchical() || connection->endpoint2()->isHierarchical()))
    {		
		// description exists only for normal interconnections
		descriptionEdit_.setDisabled(true);
		descriptionLabel_.hide();
		descriptionEdit_.hide();

		// name exists for only normal interconnections
		nameLabel_.hide();
		nameEdit_.hide();
		nameEdit_.setDisabled(true);
	}
	else
    {
		descriptionEdit_.setEnabled(!locked);
		descriptionLabel_.show();
		descriptionEdit_.show();

		nameLabel_.show();
		nameEdit_.show();
		nameEdit_.setEnabled(!locked);
	}

    adHocBoundsTable_.setEnabled(!locked);

	// set the objects visible
    instanceLabel_.show();
	connectedInstances_.show();
    separator_.show();

    type_.setVisible(endpoint1->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     endpoint2->getType() != ConnectionEndpoint::ENDPOINT_TYPE_UNDEFINED &&
                     !endpoint1->isAdHoc());
    absType_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);

    portsLabel_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);
    portWidget_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_BUS);
    adHocBoundsTable_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_ADHOC);
    dummyWidget_.setVisible(connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_API ||
                            connection_->getConnectionType() == ConnectionEndpoint::ENDPOINT_TYPE_COM);

	parentWidget()->setMaximumHeight(QWIDGETSIZE_MAX);
}

void ConnectionEditor::onNameChanged( const QString& name ) {
	Q_ASSERT(connection_);

	disconnect(connection_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));	

	QSharedPointer<QUndoCommand> cmd(new ConnectionChangeCommand(
		connection_, name, descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(connection_->scene())->getEditProvider().addCommand(cmd);

	connect(connection_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void ConnectionEditor::onDescriptionChanged() {

	Q_ASSERT(connection_);

	disconnect(connection_, SIGNAL(contentChanged()),
		this, SLOT(refresh()));

	QSharedPointer<QUndoCommand> cmd(new ConnectionChangeCommand(
		connection_, nameEdit_.text(), descriptionEdit_.toPlainText()));
	static_cast<DesignDiagram*>(connection_->scene())->getEditProvider().addCommand(cmd);

	connect(connection_, SIGNAL(contentChanged()), 
		this, SLOT(refresh()), Qt::UniqueConnection);
}

void ConnectionEditor::setPortMaps() {
	Q_ASSERT(connection_);
	
	portWidget_.clearContents();

	// get the interface and component for end point 1
	QSharedPointer<BusInterface> busIf1 = connection_->endpoint1()->getBusInterface();
	Q_ASSERT(busIf1);
	QList<QSharedPointer<General::PortMap> > portMaps1 = busIf1->getPortMaps();
	QSharedPointer<Component> comp1 = connection_->endpoint1()->getOwnerComponent();
	Q_ASSERT(comp1);

	// get the interface and component for end point 2
	QSharedPointer<BusInterface> busIf2 = connection_->endpoint2()->getBusInterface();
	Q_ASSERT(busIf2);
	QList<QSharedPointer<General::PortMap> > portMaps2 = busIf2->getPortMaps();
	QSharedPointer<Component> comp2 = connection_->endpoint2()->getOwnerComponent();
	Q_ASSERT(comp2);

	// set the header for end point 1
	ComponentItem* diacomp1 = connection_->endpoint1()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp1) {
		portWidget_.horizontalHeaderItem(0)->setText(diacomp1->name());
	}
	// if was the interface of a top component
	else {
		portWidget_.horizontalHeaderItem(0)->setText(comp1->getVlnv()->getName());
	}

	// set the header for end point 2
	ComponentItem* diacomp2 = connection_->endpoint2()->encompassingComp();
	// if endpoint1 was a component instance
	if (diacomp2) {
		portWidget_.horizontalHeaderItem(1)->setText(diacomp2->name());
	}
	// if was the interface of a top component
	else {
		portWidget_.horizontalHeaderItem(1)->setText(comp2->getVlnv()->getName());
	}

	// get the abstraction def for the interfaces
	VLNV absDefVLNV = busIf1->getAbstractionType();
	QSharedPointer<AbstractionDefinition> absDef;
	if (handler_->getDocumentType(absDefVLNV) == VLNV::ABSTRACTIONDEFINITION) {
		QSharedPointer<LibraryComponent> libComp = handler_->getModel(absDefVLNV);
		absDef = libComp.staticCast<AbstractionDefinition>();
	}

	General::InterfaceMode interfaceMode1 = busIf1->getInterfaceMode();
	General::InterfaceMode interfaceMode2 = busIf2->getInterfaceMode();

	// turn off sorting when adding items
	portWidget_.setSortingEnabled(false);

	// set the size to be the max value 
	portWidget_.setRowCount(0);

	// get list of all used logical ports
	QStringList logicalNames;
	foreach (QSharedPointer<General::PortMap> map, portMaps1) {
		if (!logicalNames.contains(map->logicalPort_)) {
			logicalNames.append(map->logicalPort_);
		}
	}
	foreach (QSharedPointer<General::PortMap> map, portMaps2) {
		if (!logicalNames.contains(map->logicalPort_)) {
			logicalNames.append(map->logicalPort_);
		}
	}

	int row = 0;
	// find the physical ports mapped to given logical port
	foreach (QString logicalPort, logicalNames) {

		bool invalid = false;

		// check that the logical signal is contained in both interface modes used
		if (absDef) {
			if (!absDef->hasPort(logicalPort, interfaceMode1) ||
				!absDef->hasPort(logicalPort, interfaceMode2)) {
					invalid = true;
			}
		}

		foreach (QSharedPointer<General::PortMap> map1, portMaps1) {
			if (map1->logicalPort_ == logicalPort) {

				foreach (QSharedPointer<General::PortMap> map2, portMaps2) {
					if (map2->logicalPort_ == logicalPort) {
						addMap(row, invalid, map1, comp1, map2, comp2);
					}
				}
			}
		}
	}

	// finally set sorting back on
	portWidget_.setSortingEnabled(true);
}

void ConnectionEditor::addMap( int& row, bool invalid,
							  QSharedPointer<General::PortMap> portMap1,
							  QSharedPointer<Component> component1,
							  QSharedPointer<General::PortMap> portMap2,
							  QSharedPointer<Component> component2) {
	
	int phys1Left = 0;
	int phys1Right = 0;
	bool phys1Invalid = invalid;

	// if port is vectored on the port map
	if (portMap1->physicalVector_) {
		phys1Left = portMap1->physicalVector_->getLeft();
		phys1Right = portMap1->physicalVector_->getRight();

		// if the port is not found on the component
		if (!component1->hasPort(portMap1->physicalPort_)) {
			phys1Invalid = true;
		}
	}
	// if port is found on the component then use the port bounds
	else if (component1->hasPort(portMap1->physicalPort_)) {
		phys1Left = component1->getPortLeftBound(portMap1->physicalPort_);
		phys1Right = component1->getPortRightBound(portMap1->physicalPort_);
	}
	// port was not found on the component
	else {
		phys1Invalid = true;
	}


	int phys2Left = 0;
	int phys2Right = 0;
	bool phys2Invalid = invalid;
	// if port is vectored on the port map
	if (portMap2->physicalVector_) {
		phys2Left = portMap2->physicalVector_->getLeft();
		phys2Right = portMap2->physicalVector_->getRight();

		// if the port is not found on the component
		if (!component2->hasPort(portMap2->physicalPort_)) {
			phys2Invalid = true;
		}
	}
	// if port is found on the component then use the port bounds
	else if (component2->hasPort(portMap2->physicalPort_)) {
		phys2Left = component2->getPortLeftBound(portMap2->physicalPort_);
		phys2Right = component2->getPortRightBound(portMap2->physicalPort_);
	}
	// port was not found on the component
	else {
		phys2Invalid = true;
	}

	// check the sizes of the physical ports
	int size1 = phys1Left - phys1Right + 1;
	int size2 = phys2Left - phys2Right + 1;

	QTableWidgetItem* port1Item;
	QTableWidgetItem* port2Item;

	// if both have vectored logical signals
	if (portMap1->logicalVector_ && portMap2->logicalVector_) {
		
		// if the vectored ports don't have any common bounds
		if (portMap1->logicalVector_->getRight() > portMap2->logicalVector_->getLeft() ||
			portMap1->logicalVector_->getLeft() < portMap2->logicalVector_->getRight()) {
				return;
		}

		int logicalLeft = qMin(portMap1->logicalVector_->getLeft(), 
			portMap2->logicalVector_->getLeft());
		int logicalRight = qMax(portMap1->logicalVector_->getRight(),
			portMap2->logicalVector_->getRight());

		QString port1;
		QString port2;
		
		{
			// count how much the left bound of port 1 has to be adjusted down
			int downSize = abs(portMap1->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 1 has to be adjusted up
			int upSize = abs(logicalRight - portMap1->logicalVector_->getRight());

			// the actual size of the connected parts of the ports
			size1 = (phys1Left - downSize) - (phys1Right + upSize) + 1; 

			port1 = General::port2String(portMap1->physicalPort_,
				phys1Left - downSize, phys1Right + upSize);
		}
		{
			// count how much the left bound of port 2 has to be adjusted down
			int downSize = abs(portMap2->logicalVector_->getLeft() - logicalLeft);
			// count how must the right bound of  port 2 has to be adjusted up
			int upSize = abs(logicalRight - portMap2->logicalVector_->getRight());

			// the actual size of the connected parts of the ports
			size2 = (phys2Left - downSize) - (phys2Right + upSize) + 1;

			port2 = General::port2String(portMap2->physicalPort_,
				phys2Left - downSize, phys2Right + upSize);
		}

		// if the connected sizes of the ports don't match
		if (size1 != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}

		port1Item = new QTableWidgetItem(port1);

		port2Item = new QTableWidgetItem(port2);
	}
	// if port map1 has vectored logical signal
	else if (portMap1->logicalVector_ && !portMap2->logicalVector_) {

		// port 1 uses the original physical bounds
		QString port1 = General::port2String(portMap1->physicalPort_, phys1Left, phys1Right);
		port1Item = new QTableWidgetItem(port1);

		// port 2 uses the bounds of the logical port of port 1
		QString port2 = General::port2String(portMap2->physicalPort_, 
			portMap1->logicalVector_->getLeft(),
			portMap1->logicalVector_->getRight());
		
		// if the logical port and port 2 sizes don't match
		if (portMap1->logicalVector_->getSize() != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
		port2Item = new QTableWidgetItem(port2);
	}
	// if port map2 has vectored logical signal
	else if (!portMap1->logicalVector_ && portMap2->logicalVector_) {
		
		// port 1 uses the bounds of the logical port of port 2
		QString port1 = General::port2String(portMap1->physicalPort_, 
			portMap2->logicalVector_->getLeft(),
			portMap2->logicalVector_->getRight());

		// if the logical port and port 2 sizes don't match
		if (portMap2->logicalVector_->getSize() != size1) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
		port1Item = new QTableWidgetItem(port1);

		// port 2 uses the original physical bounds
		QString port2 = General::port2String(portMap2->physicalPort_, phys2Left, phys2Right);
		port2Item = new QTableWidgetItem(port2);
	}
	// if neither has vectored logical signal
	else {
		QString port1 = General::port2String(portMap1->physicalPort_, phys1Left, phys1Right);
		port1Item = new QTableWidgetItem(port1);
			
		QString port2 = General::port2String(portMap2->physicalPort_, phys2Left, phys2Right);
		port2Item = new QTableWidgetItem(port2);

		// if sizes don't match then both must be marked as invalid
		if (size1 != size2) {
			phys1Invalid = true;
			phys2Invalid = true;
		}
	}

	// set the flags for the items
	port1Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	port2Item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	// set the colors according to validity of items
	if (phys1Invalid) {
		port1Item->setForeground(QBrush(Qt::red));
	}
	else {
		port1Item->setForeground(QBrush(Qt::black));
	}
	if (phys2Invalid) {
		port2Item->setForeground(QBrush(Qt::red));
	}
	else {
		port2Item->setForeground(QBrush(Qt::black));
	}
	
	// add items to the port widget
	portWidget_.insertRow(row);
	portWidget_.setItem(row, 0, port1Item);
	portWidget_.setItem(row, 1, port2Item);
	++row;
}
