//-----------------------------------------------------------------------------
// File: busifportmaptab.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 5.4.2011
//
// Description:
// Tab for editing and viewing component port maps.
//-----------------------------------------------------------------------------

#ifndef BUSIFPORTMAPTAB_H
#define BUSIFPORTMAPTAB_H


#include "portmaps/physlistview.h"
#include "portmaps/logicallistview.h"
#include "portmaps/physlistmodel.h"
#include "portmaps/PortMapsTreeModel.h"
#include "portmaps/logicallistmodel.h"
#include "portmaps/PortMapsTreeView.h"
#include <models/businterface.h>
#include <models/generaldeclarations.h>
#include <models/component.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitMappingModel.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/PortListSortProxyModel.h>
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitMappingView.h>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QTableView>
#include <QListView>
#include <QSortFilterProxyModel>
#include <QKeyEvent>
#include <QSharedPointer>

class LibraryInterface;
class PortMap;


/*! Editor to set the port maps of a bus interface.
 *
 */
class BusIfPortmapTab : public QWidget {
	Q_OBJECT

public:

	//! Connection mode specifies how multiple selections are handled
	enum ConnectionMode {
		ONE2ONE = 0, 
		ONE2MANY};

	/*! The constructor
	 *
	 * @param libHandler Pointer to the instance that manages the library.
	 * @param component Pointer to the component being edited.
	 * @param busif Pointer to the bus interface being edited.
	 * @param parent Pointer to the owner of the editor.
	 *
	*/
	BusIfPortmapTab(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		BusInterface* busif,
		QWidget* parent);
	
	//! The destructor
	virtual ~BusIfPortmapTab();

	/*! Check for the validity of the edited item.
	*
	* @return True if item is valid.
	*/
	virtual bool isValid() const;

	/*! Check for the validity of the edited item.
	*
    *      @param [inout] errorList   The list to add the possible error messages to.
    *
	*      @return True if item is valid.
	*/
	virtual bool isValid(QStringList& errorList) const;

	/*! Restore the changes made in the editor back to ones in the model.
	*
	*/
	virtual void refresh();

	/*! Set the abstraction type that defines the logical signals to use.
	 *
	 * @param vlnv
	 *
	 * @return void
	*/
	virtual void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

    /*!
     *  Sets a subset of component ports to be visible in the physical port list.
     *
     *      @param [in] ports   List of port names to show.
     */
    virtual void setPhysicalPorts(QStringList const& ports);

public slots:

	//! Handler for remove button clicks.
	void onRemove();
	
protected:

	//! Handler for key press events.
	virtual void keyPressEvent(QKeyEvent* event);

	//! Handler for widget's show event
    virtual void showEvent(QShowEvent* event);

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;

	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

private slots:

	//! The handler for refresh button clicks
	void onRefresh();

	//! The handler for connect button clicks
	void onConnect();

    //! The handler for connect bits button clicks.
    void onBitConnect();

    //! The handler for connecting port to first unmapped logical bits.
    void onMapPortToLastBit();

    // One to one and one to many removed as obsolete.
	//! Handler for when user changes the connection mode.    
	//void onConnectionModeChange();

	/*! Make connections between physical signals and logical signals.
	 *
	 * @param physicals List of physical signal names.
	 * @param logicals List of logical signal names.
	 *
	*/
	void onMakeConnections(const QStringList& physicals, const QStringList& logicals);

    /*!
     *  The handler for change in logical port selection.
     *
     *      @param [in] index   The index of the selected logical port.     
     */
    void onLogicalChanged(const QModelIndex& index);


    /*!
     *  The handler for changing bit field mapping visibility.
     *
     */
    void toggleMappingVisibility();

    //! Handler for show all button clicks.
    void onShowAll();

private:
	
	//! No copying
	BusIfPortmapTab(const BusIfPortmapTab& other);

	//! No assignment
	BusIfPortmapTab& operator=(const BusIfPortmapTab& other);

	//! Set up the layout of the GUI items
	void setupLayout();
   
	//! Specifies the connection mode
	ConnectionMode mode_;

	//! Pointer to the currently selected port map.
	QSharedPointer<General::PortMap> portMap_;

	//! Pointer to the bus interface being edited.
	BusInterface* busif_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! The proxy filter that does sorting for port maps.
	//QSortFilterProxyModel mapProxy_;

	//! The model to manage the port maps.
	PortMapsTreeModel model_;

	//! The view to display the port maps.
	PortMapsTreeView view_;

	//! The view to display the logical ports
	LogicalListView logicalView_;

	//! The model to present the logical ports.
	LogicalListModel logicalModel_;

    //! The label for bit-level mapping.
    QLabel mappingLabel_;

    //! The bit-level mapping view.
    BitMappingView mappingView_;

    //! The sorting proxy for bit-level mapping.
    QSortFilterProxyModel mappingProxy_;

    //! The model to present bit-level mapping.
    BitMappingModel mappingModel_;

	//! The view to display the physical ports.
	PhysListView physicalView_;

    //! The filter proxy for the physical ports.
    PortListSortProxyModel physProxy_;

	//! The model to present the physical ports.
	PhysListModel physModel_; 

	//! The button to clean the lists of physical and logical ports.
	QPushButton cleanButton_;

	//! The button to connect the selected logical and physical ports.
	QPushButton connectButton_;

    //! The button to clear port filter.
    QPushButton showAllButton_;

	//! The button to select the one to one connection mode.
    // One to one and one to many removed as obsolete.
	//QPushButton one2OneButton_;

	//! The button to select the one to many connection mode.
    // One to one and one to many removed as obsolete.
	//QPushButton one2ManyButton_;

    //! The button to show/hide bit-level mapping.
    QPushButton showHideMappingButton_;

    //! Editor for filtering of physical ports by name.
    QLineEdit* nameFilterEditor_;

    //! Button for filtering ports by direction in.
    QPushButton inButton_;

    //! Button for filtering ports by direction out.
    QPushButton outButton_;

    //! Checkbox for showing/hiding connected physical ports.
    QCheckBox hideConnectedBox_;

    //! Boolean for show/hide status of the bit-level mapping.
    bool showBitMapping_;
};

#endif // BUSIFPORTMAPTAB_H
