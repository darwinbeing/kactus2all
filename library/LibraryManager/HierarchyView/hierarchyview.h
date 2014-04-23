/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchyview.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H

#include <library/LibraryManager/vlnv.h>

#include <QTreeView>
#include <QPoint>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>

class LibraryInterface;
class HierarchyFilter;

/*! \brief HierarchyView displays the filtered contents from hierarchy filter.
 * 
 */
class HierarchyView : public QTreeView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param handler Pointer to the instance that manages the library.
	 * \param filter Pointer to the instance that does sorting and filtering.
	 *
	*/
	HierarchyView(QWidget *parent, 
		LibraryInterface* handler,
		HierarchyFilter* filter);
	
	//! \brief The destructor
	virtual ~HierarchyView();

	/*! \brief Select the item with given indexes in the view.
	 *
	 * \param indexes List in model indexes to select.
	 *
	*/
	void setSelectedIndexes(const QModelIndexList& indexes);

public slots:

	//! \brief Open a hierarchical component
	void onOpenDesign();

    //! \brief Open a memory design of a hierarchical component
    void onOpenMemoryDesign();

    //! \brief Open the SW design of a component
    void onOpenSWDesign();

    //! \brief Open the system design of a component.
    void onOpenSystemDesign();

	//! \brief Open component editor
	void onOpenComponent();

	//! \brief Create new component
	void onCreateComponent();

	//! \brief Create new design
	void onCreateDesign();

    //! \brief Create new SW design
    void onCreateSWDesign();

    //! \brief Create new system design
    void onCreateSystemDesign();

	//! \brief Called when user selects "Export item and sub-items" in contextMenu
	void onExportAction();

	/*! \brief Set the index to be the current index and select and expand it.
	 *
	 * \param index Specified the index to be set as the current index.
	 *
	*/
	virtual void setCurrentIndex(const QModelIndex& index);

	//! \brief Open bus in bus editor
	void onOpenBus();

	//! \brief Create new abstraction definition for selected bus definition.
	void onAddSignals();

	//! \brief Create new bus definition and abstraction definition.
	void onCreateBus();

    //! \brief Open COM definition in editor
    void onOpenComDef();

    //! \brief Create new COM definition.
    void onCreateComDef();

    //! \brief Open API definition in editor
    void onOpenApiDef();

    //! \brief Create new API definition.
    void onCreateApiDef();

	//! \brief Open the xml of the selected IP-Xact element.
	void onOpenXml();

    //! Opens the folder that contains the IP-XACT object.
    void onOpenContainingFolder();

    //! Called when the user chooses Show Errors from the context menu.
    void onShowErrors();

signals:
	//! \brief Open the selected hierarchical design
	void openDesign(const QModelIndex& index);

    //! \brief Open the memory design of a hierarchical component
    void openMemoryDesign(const QModelIndex& index);

    //! \brief Open the selected SW design
    void openSWDesign(const QModelIndex& index);

    //! \brief Open the selected system design
    void openSystemDesign(const QModelIndex& index);

	//! \brief Open the given object in the component editor
	void openComponent(const QModelIndex& index);

	//! \brief Create new component
	void createNewComponent(const QModelIndex& index);

	//! \brief create new bus
	void createNewBus(const QModelIndex& index);

	//! \brief Create new abstraction definition for given bus definition.
	void createNewAbsDef(const QModelIndex& index);

    //! \brief create new COM definition
    void createNewComDef(const QModelIndex& index);

    //! \brief create new API definition
    void createNewApiDef(const QModelIndex& index);

	//! \brief Create new design
	void createNewDesign(const QModelIndex& index);

    //! Create new SW design.
    void createNewSWDesign(QModelIndex const& index);

    //! Create new system design.
    void createNewSystemDesign(QModelIndex const& index);

	//! \brief Export the selected item and it's sub-items.
	void exportItem(const QModelIndex& index);

    //! Shows errors about the selected item.
    void showErrors(QModelIndex const& index);

	//! \brief Signal is emitted when component is selected in the view.
	void componentSelected(const VLNV& vlnv);

	//! \brief A drag operation has been initiated.
	void dragInitiated(const QModelIndex& index);

	//! \brief Send error message to user.
	void errorMessage(const QString& message);

	//! \brief Send notification to user.
	void noticeMessage(const QString& message);

protected:

	/*! \brief Handler for the context menu events.
     *
     * \param event Pointer to the contextMenuEvent to be handled.
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

	//! \brief The event handler for mouse double clicks
	virtual void mouseDoubleClickEvent(QMouseEvent * event);

	//! \brief The event handler for mousePress events
	virtual void mousePressEvent(QMouseEvent *event);

	//! \brief The event handler for mouseMove events
	virtual void mouseMoveEvent(QMouseEvent *event);

	//! \brief The event handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent * event);

private:
	//! \brief No copying
	HierarchyView(const HierarchyView& other);

	//! \brief No assignment
	HierarchyView& operator=(const HierarchyView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the instance that does the filtering and sorting
	HierarchyFilter* filter_;

	//! \brief The starting position for drag
	QPoint startPos_;

	//! \brief The index that is selected to be dragged
	QModelIndex dragIndex_;

	/**********************************************************************/
	// The actions for the contextMenu

	//! \brief Open the hierarchical design of a component
	QAction* openDesignAction_;

    //! \brief Open the memory design of a component
    QAction* openMemoryDesignAction_;

    //! \brief Open the SW design of a component
    QAction* openSWDesignAction_;

	//! \brief Open the component editor
	QAction* openCompAction_;

	//! \brief Create a new empty object, only copy the vlnv structure.
	QAction* createNewComponentAction_;

	//! \brief Create new design for a component
	QAction* createNewDesignAction_;

    //! \brief Create new SW design for a component
    QAction* createNewSWDesignAction_;

    //! \brief Create new system design for a component
    QAction* createNewSystemDesignAction_;

	//! \brief Export the item(s) to a new location in file system
	QAction* exportAction_;

    //! Show errors about the item.
    QAction* showErrorsAction_;

	//! \brief Open the bus editor
	QAction* openBusAction_;

	//! \brief Create new abstraction definition for bus definition.
	QAction* addSignalsAction_;

	//! \brief Open the COM definition editor
    QAction* openComDefAction_;

    //! \brief Open the API definition editor
    QAction* openApiDefAction_;

    //! \brief Open an existing system for editing.
	QAction* openSystemAction_;

	//! \brief Open an existing software platform.
	QAction* openPlatformAction_;

	//! \brief Open a software application
	QAction* openApplicationAction_;

    //! \brief Open end point to be edited
    QAction* openEndpointAction_;

    //! \brief Open the xml file to be viewed by the user.
	QAction* openXmlAction_;

    //! Action to open the containing folder.
    QAction* openContainingFolderAction_;
};

#endif // HIERARCHYVIEW_H
