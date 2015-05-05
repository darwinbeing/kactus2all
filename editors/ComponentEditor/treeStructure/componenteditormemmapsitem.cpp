/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormemmapsitem.h"
#include "componenteditormemmapitem.h"
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/memoryMaps/memorymapseditor.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsVisualizer/memorymapsvisualizer.h>

ComponentEditorMemMapsItem::ComponentEditorMemMapsItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
                                                       QSharedPointer<ReferenceCounter> referenceCounter,
                                                       QSharedPointer<ParameterFinder> parameterFinder,
                                                       QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                       QSharedPointer<ExpressionParser> expressionParser,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
memoryMaps_(component->getMemoryMaps()),
visualizer_(new MemoryMapsVisualizer()),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	setObjectName(tr("ComponentEditorMemMapsItem"));

	foreach (QSharedPointer<MemoryMap> memoryMap, memoryMaps_)
    {
		QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(new ComponentEditorMemMapItem(memoryMap, model,
            libHandler, component, referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, this));
		memoryMapItem->setVisualizer(visualizer_);
		childItems_.append(memoryMapItem);

        connect(this, SIGNAL(memoryRemapAdded(int, QSharedPointer<MemoryMap>)),
            memoryMapItem.data(), SLOT(onMemoryRemapAdded(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

        connect(this, SIGNAL(memoryRemapRemoved(int ,QSharedPointer<MemoryMap>)),
            memoryMapItem.data(), SLOT(onMemoryRemapRemoved(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);
	}
}

ComponentEditorMemMapsItem::~ComponentEditorMemMapsItem() {
	if (visualizer_) {
		delete visualizer_;
		visualizer_ = NULL;
	}
}

QFont ComponentEditorMemMapsItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!memoryMaps_.isEmpty());
    return font;
}

QString ComponentEditorMemMapsItem::text() const {
	return tr("Memory maps");
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorMemMapsItem::editor()
{
	if (!editor_)
    {
		editor_ = new MemoryMapsEditor(component_, parameterFinder_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(graphicsChanged()), this, SLOT(onGraphicsChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
		connect(editor_, SIGNAL(selectBusInterface(const QString&)),
            model_, SLOT(onSelectBusInterface(const QString&)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(changeInAddressUnitBitsOnRow(int)), 
            this, SLOT(addressUnitBitsChangedOnMemoryMap(int)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(memoryRemapAdded(int, QSharedPointer<MemoryMap>)),
            this, SIGNAL(memoryRemapAdded(int ,QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

        connect(editor_, SIGNAL(memoryRemapRemoved(int, QSharedPointer<MemoryMap>)),
            this, SIGNAL(memoryRemapRemoved(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

QString ComponentEditorMemMapsItem::getTooltip() const {
	return tr("Contains the memory maps of the component");
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::createChild( int index )
{
	QSharedPointer<ComponentEditorMemMapItem> memoryMapItem(new ComponentEditorMemMapItem(memoryMaps_.at(index),
        model_, libHandler_, component_, referenceCounter_, parameterFinder_, expressionFormatter_,
        expressionParser_, this));
	memoryMapItem->setLocked(locked_);
	childItems_.insert(index, memoryMapItem);
	
    connect(this, SIGNAL(memoryRemapAdded(int, QSharedPointer<MemoryMap>)),
        memoryMapItem.data(), SLOT(onMemoryRemapAdded(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

    connect(this, SIGNAL(memoryRemapRemoved(int ,QSharedPointer<MemoryMap>)),
        memoryMapItem.data(), SLOT(onMemoryRemapRemoved(int, QSharedPointer<MemoryMap>)), Qt::UniqueConnection);

	if (visualizer_)
    {
		memoryMapItem->setVisualizer(visualizer_);
	}
}

ItemVisualizer* ComponentEditorMemMapsItem::visualizer() {
	return visualizer_;
}

//-----------------------------------------------------------------------------
// Function: componenteditormemmapsitem::addressUnitBitsChangedOnMemoryMap()
//-----------------------------------------------------------------------------
void ComponentEditorMemMapsItem::addressUnitBitsChangedOnMemoryMap(int memoryMapIndex)
{
    QSharedPointer<ComponentEditorMemMapItem> childMemoryMap = 
        qobject_cast<QSharedPointer<ComponentEditorMemMapItem> > ( child(memoryMapIndex) );
    
    if (!childMemoryMap.isNull())
    {
        childMemoryMap->changeAdressUnitBitsOnAddressBlocks();
    }
}