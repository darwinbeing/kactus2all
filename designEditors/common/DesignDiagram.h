//-----------------------------------------------------------------------------
// File: DesignDiagram.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#ifndef DESIGNDIAGRAM_H
#define DESIGNDIAGRAM_H

#include <QGraphicsScene>
#include <QStringList>
#include <QAction>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

#include <designEditors/HWDesign/AdHocEnabled.h>

#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/ColumnDesc.h>

#include <designEditors/common/DrawMode.h>

class MainWindow;
class LibraryInterface;
class GenericEditProvider;
class Component;
class ConnectionEndpoint;
class GraphicsConnection;
class GraphicsColumnLayout;
class ComponentItem;
class Design;
class VLNV;
class DesignWidget;

//-----------------------------------------------------------------------------
//! Base class for all design diagrams.
//-----------------------------------------------------------------------------
class DesignDiagram : public QGraphicsScene, public AdHocEnabled
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh            The library interface.
     *      @param [in] mainWnd       The main window.
     *      @param [in] editProvider  The edit provider.
     *      @param [in] parent        The parent widget.
     */
    DesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                  GenericEditProvider& editProvider, DesignWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~DesignDiagram();

    /*!
     *  Clears the scene.
     */
    virtual void clearScene();

    /*!
     *  Opens a design for editing.
     *
     *      @param [in] component  The component whose design to edit.
     *      @param [in] design     The design to edit.
     *      @param [in] designConf The design configuration if one exists.
     */
    bool setDesign(QSharedPointer<Component> component, QSharedPointer<Design> design,
                   QSharedPointer<DesignConfiguration> designConf = QSharedPointer<DesignConfiguration>());

    /*! 
     *  Creates a design based on the contents in the diagram.
     *
     *      @param [in] vlnv The vlnv for the design.
     *
     *      @return The created design.
     */
    virtual QSharedPointer<Design> createDesign(VLNV const& vlnv) const;

    /*!
     *  Reflects the changes in the design to the top-level component.
     */
    virtual void updateHierComponent() = 0;

    /*!
     *  Attaches the data source to an ad-hoc editor.
     */
    virtual void attach(AdHocEditor* editor);

    /*!
     *  Detaches the data source from the ad-hoc editor.
     */
    virtual void detach(AdHocEditor* editor);

    /*!
     *  Adds an instance name to the list of used instance names.
     *
     *      @param [in] name The name of the instance.
     *
     *      @remarks This function should be called when a component instance is added to the diagram.
     */
    void addInstanceName(QString const& name);

    /*!
     *  Removes the instance name from the list so the name can be used again in the design.
	 *
	 *      @param name The name of the removed instance.
	 *
     *      @remarks This function should be called when user removes component instance.
	 */
	void removeInstanceName(const QString& name);

	/*!
     *  Updates the list of instance names.
	 *
	 *      @param oldName The old name of the component instance.
	 *      @param newName The new name of the component instance.
	 */
	void updateInstanceName(const QString& oldName, const QString& newName);

    /*!
     *  Changes the state of a visibility control.
     *
     *      @param [in] name   The name of the visibility control.
     *      @param [in] state  The new state for the visibility control.
     */
    virtual void setVisibilityControlState(QString const& name, bool state);

    /*!
     *  Sets the draw mode of the diagram.
     *
     *      @param [in] mode The draw mode.
     */
    virtual void setMode(DrawMode mode);

    /*!
     *  Sets the block diagram locked/unlocked.
     *
     *      @param [in] locked If true, the diagram is locked. Otherwise it is unlocked.
     */
    void setProtection(bool locked);

    /*!
     *  Adds a column to the diagram's layout.
     *
     *      @param [in] desc The column description.
     */
    virtual void addColumn(ColumnDesc const& desc) = 0;

    /*!
     *  Returns the diagram column layout.
     */
    virtual GraphicsColumnLayout* getColumnLayout() = 0;

    /*!
     *  Picks the top-most component at the given position.
     *
     *      @param [in] pos The picking positions.
     *
     *      @return The top-most component at the given positions, or 0 if there is no component.
     */
    ComponentItem* getTopmostComponent(QPointF const& pos);

    /*!
     *  Returns the current draw mode.
     */
    DrawMode getMode() const;

    /*!
     *  Returns true if the diagram is being loaded.
     */
    bool isLoading() const;

    /*!
     *  Returns true if the diagram is in locked state.
     */
    bool isProtected() const;

    /*!
     *  Returns the library interface.
     */
    LibraryInterface* getLibraryInterface() const;

    /*!
     *  Returns the main window.
     */
    MainWindow* getMainWindow() const;

    /*!
     *  Returns the parent design widget.
     */
    DesignWidget* getParent();

    /*!
     *  Returns the edit provider.
     */
    GenericEditProvider& getEditProvider();

    /*!
     *  Returns the component whose design is being edited.
     */
    QSharedPointer<Component> getEditedComponent() const;

    /*!
     *  Returns the design configuration.
     */
    QSharedPointer<DesignConfiguration> getDesignConfiguration() const;

    /*!
     *  Returns a list of instances currently in the design.
     */
    QList<ComponentItem*> getInstances() const;

    /*!
     *  Returns the type of the given items if they all are of the same type.
     *
     *      @return The common type, or -1 if the items are of different type.
     */
    static int getCommonItemType(QList<QGraphicsItem*> const& items);

    /*!
     *  Sorts graphics items by X coordinate.
     */
    static bool sortByX(QGraphicsItem* lhs, QGraphicsItem* rhs);

public slots:
    //! Called when the diagram is shown.
    void onShow();

    //! Called when the view has been scrolled vertically.
    virtual void onVerticalScroll(qreal y) = 0;

    //! Called when a component instance is added to the diagram.
    virtual void onComponentInstanceAdded(ComponentItem* item);

    //! Called when a component instance is remove from the diagram.
    virtual void onComponentInstanceRemoved(ComponentItem* item);

    //! Called when a vendor extension is added to the design.
    virtual void onVendorExtensionAdded(QSharedPointer<VendorExtension> extension);

    //! Called when a vendor extension is removed from the design.
    virtual void onVendorExtensionRemoved(QSharedPointer<VendorExtension> extension);

signals:
    //! Emitted when component with given vlnv should be opened in editor.
    void openComponent(const VLNV& vlnv);

    //! \brief Emitted when the user double-clicks a hierarchical SW component.
    void openSWDesign(const VLNV& vlnv, const QString& viewName);

    //! Emitted when a C source file should be opened for editing.
    void openCSource(ComponentItem* compItem);

    //! Emitted when user double clicks on a hierarchical component.
    void openDesign(const VLNV& vlnv, const QString& viewName);

    //! Signaled when the bus with the given vlnv should be opened for editing.
    void openBus(VLNV const& vlnv, VLNV const& absDefVLNV, bool disableBusDef);

    //! Signaled when the draw mode has changed.
    void modeChanged(DrawMode mode);

    //! Signaled when the contents of the diagram have changed.
    void contentChanged();

    //! Sends an error message to the user.
    void errorMessage(const QString& errorMessage);

    //! Sends a notification to the user.
    void noticeMessage(const QString& noticeMessage);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

    //! Signaled when a new component is instantiated to the design.
    void componentInstantiated(ComponentItem* item);

    //! Signaled when a component instance is removed from the design.
    void componentInstanceRemoved(ComponentItem* item);

    //! Signaled when user selects a component on the draw board.
    void componentSelected(ComponentItem* component);

    //! Signaled when the user selects an interface on the draw board.
    void interfaceSelected(ConnectionEndpoint* endpoint);

    //! Signaled when the user selects a connection on the draw board.
	void connectionSelected(GraphicsConnection* connection);

    //! Signaled when all items are deselected.
    void clearItemSelection();

	//! Signaled when copy is selected from the context menu.
	void copyItem();

protected:
    /*!
     *  Creates a unique instance name for the given component.
     *
     *      @param [in] component The component whose instance is being created.
     *
     *      @return The name for the component instance.
     */
    QString createInstanceName(QSharedPointer<Component> component);

    /*!
     *  Creates a unique instance name with the given base name.
     *
     *      @param [in] baseName The base name for the instance.
     */
    QString createInstanceName(QString const& baseName);

    /*!
     *  Draws the diagram background.
     */
     virtual void drawBackground(QPainter* painter, QRectF const& rect);

    /*!
     *  Adds a new label to the design.
     *
     *      @param [in] position   The initial position of the label.
     */
    virtual void createLabel(QPointF const& position);

     /*!
     *  Called when an item has been selected in the diagram.
     *
     *      @param [in] newSelection The selected item.
     */
    virtual void onSelected(QGraphicsItem* newSelection) = 0;

	/*!
     *  Creates the context menu for function contextMenuEvent().
     *
     *      @param [in] pos Mouse position when the menu is requested.
	 *
	 *      @return The menu with allowed actions or 0 if no menu is allowed.
     */
	virtual QMenu* createContextMenu(QPointF const& pos);

	/*!
     *  Event handler for receiving context menu events (e.g. right mouse click).
     *
     *      @param [in] event The triggering event.
     */
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

private:
    // Disable copying.
    DesignDiagram(DesignDiagram const& rhs);
    DesignDiagram& operator=(DesignDiagram const& rhs);

    /*!
     *  Opens the given design for editing.
     *
     *      @param [in] design The design to open.
     */
    virtual void loadDesign(QSharedPointer<Design> design) = 0;

    //! Creates sticky notes from vendor extensions.
    void loadStickyNotes();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent design widget.
    DesignWidget* parent_;

    //! Library interface.
    LibraryInterface* lh_;

    //! A pointer to the main window.
    MainWindow* mainWnd_;

    //! The edit provider for undo/redo.
    GenericEditProvider& editProvider_;

    //! The component whose design is being edited.
    QSharedPointer<Component> component_;

    //! The design configuration.
    QSharedPointer<DesignConfiguration> designConf_;

    //! The current draw mode.
    DrawMode mode_;

    //! The list of used instance names.
    QStringList instanceNames_;

    //! If true, the diagram is being loaded.
    bool loading_;

    //! If true, the diagram is locked and cannot be modified.
    bool locked_;

	//! \brief Contains the XML header comments of the design.
	QStringList XMLComments_;

    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

//-----------------------------------------------------------------------------

#endif // DESIGNDIAGRAM_H
