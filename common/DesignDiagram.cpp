//-----------------------------------------------------------------------------
// File: DesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 24.4.2012
//
// Description:
// Base class for all design diagrams.
//-----------------------------------------------------------------------------

#include "DesignDiagram.h"

#include "DesignWidget.h"

#include <common/utils.h>
#include <common/diagramgrid.h>
#include <common/GenericEditProvider.h>
#include <common/graphicsItems/ComponentItem.h>

#include <AdHocEditor/AdHocEditor.h>

#include <LibraryManager/libraryinterface.h>

#include <models/designconfiguration.h>
#include <models/component.h>

#include <QWidget>
#include <QPainter>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: DesignDiagram::DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::DesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                             GenericEditProvider& editProvider, DesignWidget* parent)
    : QGraphicsScene(parent),
      parent_(parent),
      lh_(lh),
      mainWnd_(mainWnd),
      editProvider_(editProvider),
      component_(),
      designConf_(),
      mode_(MODE_SELECT),
      instanceNames_(),
      loading_(false),
      locked_(false),
	  XMLComments_()
{
    setSceneRect(0, 0, 100000, 100000);

    connect(this, SIGNAL(componentInstantiated(ComponentItem*)),
        this, SLOT(onComponentInstanceAdded(ComponentItem*)), Qt::UniqueConnection);
    connect(this, SIGNAL(componentInstanceRemoved(ComponentItem*)),
        this, SLOT(onComponentInstanceRemoved(ComponentItem*)), Qt::UniqueConnection);		
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::~DesignDiagram()
//-----------------------------------------------------------------------------
DesignDiagram::~DesignDiagram()
{
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void DesignDiagram::clearScene()
{
    clear();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setDesign()
//-----------------------------------------------------------------------------
bool DesignDiagram::setDesign(QSharedPointer<Component> component, QSharedPointer<Design> design,
                              QSharedPointer<DesignConfiguration> designConf)
{
	// save the XML header from the design
	XMLComments_ = design->getTopComments();

    // Clear the edit provider.
    editProvider_.clear();

    // clear the previous design configuration
    designConf_.clear();

    // Deselect items.
    emit clearItemSelection();

    // Clear the scene.
    clearScene();
    getParent()->clearRelatedVLNVs();
    getParent()->addRelatedVLNV(*component->getVlnv());

    // Set the new component and open the design.
    component_ = component;
    designConf_ = designConf;

    loading_ = true;
    loadDesign(design);
    loading_ = false;

    return true;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::attach()
//-----------------------------------------------------------------------------
void DesignDiagram::attach(AdHocEditor* editor)
{
    connect(this, SIGNAL(contentChanged()), editor, SLOT(onContentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(destroyed(QObject*)), editor, SLOT(clear()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::detach()
//-----------------------------------------------------------------------------
void DesignDiagram::detach(AdHocEditor* editor)
{
    disconnect(editor);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::addInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::addInstanceName(QString const& name)
{
    instanceNames_.append(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::removeInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::removeInstanceName(const QString& name)
{
    instanceNames_.removeAll(name);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::updateInstanceName()
//-----------------------------------------------------------------------------
void DesignDiagram::updateInstanceName(const QString& oldName, const QString& newName)
{
    instanceNames_.removeAll(oldName);
    instanceNames_.append(newName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setMode()
//-----------------------------------------------------------------------------
void DesignDiagram::setMode(DrawMode mode)
{
    if (mode_ != mode)
    {
        mode_ = mode;

        if (mode_ != MODE_SELECT)
        {
            emit clearItemSelection();
        }

        emit modeChanged(mode);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setProtection()
//-----------------------------------------------------------------------------
void DesignDiagram::setProtection(bool locked)
{
    locked_ = locked;
    clearSelection();
    emit clearItemSelection();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMode()
//-----------------------------------------------------------------------------
DrawMode DesignDiagram::getMode() const
{
    return mode_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::isProtected()
//-----------------------------------------------------------------------------
bool DesignDiagram::isProtected() const
{
    return locked_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* DesignDiagram::getLibraryInterface() const
{
    return lh_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getMainWindow()
//-----------------------------------------------------------------------------
MainWindow* DesignDiagram::getMainWindow() const
{
    return mainWnd_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditProvider()
//-----------------------------------------------------------------------------
GenericEditProvider& DesignDiagram::getEditProvider()
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getEditedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> DesignDiagram::getEditedComponent() const
{
    return component_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> DesignDiagram::getDesignConfiguration() const
{
    return designConf_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceAdded()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceAdded(ComponentItem* item)
{
    instanceNames_.append(item->name());
    getParent()->addRelatedVLNV(*item->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onComponentInstanceRemoved()
//-----------------------------------------------------------------------------
void DesignDiagram::onComponentInstanceRemoved(ComponentItem* item)
{
    instanceNames_.removeAll(item->name());
    getParent()->removeRelatedVLNV(*item->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QSharedPointer<Component> component)
{
    QString instanceName = component->getVlnv()->getName();
    instanceName.remove(QString(".comp"));
    return createInstanceName(instanceName);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createInstanceName()
// Forms a unique identifier for a component instance
//-----------------------------------------------------------------------------
QString DesignDiagram::createInstanceName(QString const& baseName)
{
    QSettings settings; // this reads the application settings automatically
    QString format = settings.value("Policies/InstanceNames", "").toString();
    if (format == "")
    {
        format = "$ComponentName$_$InstanceNumber$";
    }

    // Determine a unique name by using a running number.
    int runningNumber = 0;
    
    QString name = format;
    Utils::replaceMagicWord(name, "ComponentName", baseName);
    Utils::replaceMagicWord(name, "InstanceNumber", QString::number(runningNumber));

    while (instanceNames_.contains(name))
    {
        ++runningNumber;
        
        name = format;
        Utils::replaceMagicWord(name, "ComponentName", baseName);
        Utils::replaceMagicWord(name, "InstanceNumber", QString::number(runningNumber));
    }

    instanceNames_.append(name);
    return name;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::drawBackground()
//-----------------------------------------------------------------------------
void DesignDiagram::drawBackground(QPainter* painter, QRectF const& rect)
{
    painter->setWorldMatrixEnabled(true);
    //painter->setBrush(Qt::lightGray);
    painter->setPen(QPen(Qt::black, 0));

    qreal left = int(rect.left()) - (int(rect.left()) % GridSize );
    qreal top = int(rect.top()) - (int(rect.top()) % GridSize );

    for (qreal x = left; x < rect.right(); x += GridSize ) {
        for (qreal y = top; y < rect.bottom(); y += GridSize )
            painter->drawPoint(x, y);
    }
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createContextMenu()
//-----------------------------------------------------------------------------
QMenu* DesignDiagram::createContextMenu(QPointF const& /*pos*/){
	return 0;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::contextMenuEvent()
//-----------------------------------------------------------------------------
void DesignDiagram::contextMenuEvent(QGraphicsSceneContextMenuEvent* event){
	
	QMenu* menu = createContextMenu(event->scenePos());
	if ( menu != 0 )
	{
	menu->exec(event->screenPos());
	delete menu;
	}
	event->accept();
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::onShow()
//-----------------------------------------------------------------------------
void DesignDiagram::onShow()
{
    // Retrieve the new selection.
    QGraphicsItem *newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    onSelected(newSelection);
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::setVisibilityControlState()
//-----------------------------------------------------------------------------
void DesignDiagram::setVisibilityControlState(QString const& /*name*/, bool /*state*/)
{
    // Empty default implementation.
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getInstances()
//-----------------------------------------------------------------------------
QList<ComponentItem*> DesignDiagram::getInstances() const
{
    // the list to store the diagram components to
    QList<ComponentItem*> instances;

    // ask for all graphics items.
    QList<QGraphicsItem*> graphItems = items();
    foreach (QGraphicsItem* graphItem, graphItems) {

        // make dynamic type conversion
        ComponentItem* diagComp = dynamic_cast<ComponentItem*>(graphItem);
        // if the item was a diagram component then add it to the list.
        if (diagComp) 
            instances.append(diagComp);
    }
    return instances;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::isLoading()
//-----------------------------------------------------------------------------
bool DesignDiagram::isLoading() const
{
    return loading_;
}

//-----------------------------------------------------------------------------
// Function: SystemDesignDiagram::getTopmostComponent()
//-----------------------------------------------------------------------------
ComponentItem* DesignDiagram::getTopmostComponent(QPointF const& pos)
{
    QList<QGraphicsItem*> itemList = items(pos);

    foreach (QGraphicsItem* item, itemList)
    {
        ComponentItem* compItem = dynamic_cast<ComponentItem*>(item);

        if (compItem != 0)
        {
            return compItem;
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::getParent()
//-----------------------------------------------------------------------------
DesignWidget* DesignDiagram::getParent()
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: DesignDiagram::createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> DesignDiagram::createDesign( VLNV const& vlnv ) const {
	QSharedPointer<Design> design(new Design(vlnv));
	design->setTopComments(XMLComments_);
	return design;
}