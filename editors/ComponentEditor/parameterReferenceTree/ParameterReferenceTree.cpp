//-----------------------------------------------------------------------------
// File: ParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Tree widget for displaying parameter references.
//-----------------------------------------------------------------------------

#include "ParameterReferenceTree.h"

#include <IPXACTmodels/memorymapitem.h>

namespace
{
    const QString VALUENAME = QString("Value");
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::ParameterReferenceTree(QSharedPointer<Component> component,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QString const& targetID, QWidget *parent /* = 0 */):
QTreeWidget(parent),
component_(component),
targetID_(targetID),
expressionFormatter_(expressionFormatter)
{
    QStringList labels;
    labels << "Referencing elements" << "Referencing expression";
    setHeaderLabels(labels);

    setColumnCount(COLUMN_COUNT);
    uniformRowHeights();

    setupTree();

    //resizeColumnToContents(ITEM_NAME);
    setColumnWidth(ITEM_NAME, 200);
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::~ParameterReferenceTree()
//-----------------------------------------------------------------------------
ParameterReferenceTree::~ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::setupTree()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::setupTree()
{
    if (!component_.isNull())
    {
        if (referenceExistsInModelParameters())
        {
            createReferencesForModelParameters();
        }

        if (referenceExistsInParameters(component_->getParameters()))
        {
            createParameterReferences();
        }

        if (referenceExistsInMemoryMaps())
        {
            createReferencesForMemoryMaps();
        }

        if (referenceExistsInViews())
        {
            createReferencesForViews();
        }

        if (referenceExistsInPorts())
        {
            createReferencesForPorts();
        }

        if (topLevelItemCount() == 0)
        {
            createTopItem("No references found.");
        }
    }
    else
    {
        createTopItem("Component does not exist.");
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInParameters(QList<QSharedPointer<Parameter> > parameterList)
{
    foreach (QSharedPointer<Parameter> parameter, parameterList)
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::parameterHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::parameterHasReference(QSharedPointer<Parameter> parameter)
{
    if (parameter->getValue().contains(targetID_) || parameter->getBitWidth().contains(targetID_) ||
        parameter->getAttribute("arraySize").contains(targetID_) ||
        parameter->getAttribute("arrayOffset").contains(targetID_))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createParameterReferences()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createParameterReferences()
{
    QTreeWidgetItem* topParametersItem = createTopItem("Parameters");

    foreach (QSharedPointer<Parameter> parameter, component_->getParameters())
    {
        if (parameterHasReference(parameter))
        {
            QTreeWidgetItem* parameterItem = createMiddleItem(parameter->getName(), topParametersItem);
            createItemsForParameter(parameter, parameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInModelParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInModelParameters()
{
    foreach(QSharedPointer<Parameter> modelParameter, component_->getModelParameters())
    {
        if(parameterHasReference(modelParameter))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForModelParameters()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForModelParameters()
{
    QTreeWidgetItem* topModelParameterItem = createTopItem("Model Parameters");

    foreach (QSharedPointer<ModelParameter> modelParameter, component_->getModelParameters())
    {
        if (parameterHasReference(modelParameter))
        {
            QTreeWidgetItem* modelParameterItem = createMiddleItem(modelParameter->getName(),
                topModelParameterItem);
            createItemsForParameter(modelParameter, modelParameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInViews()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInViews()
{
    foreach(QSharedPointer<View> view, component_->getViews())
    {
        foreach (QSharedPointer<Parameter> parameter, view->getParameters())
        {
            if (parameterHasReference(parameter))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForViews()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForViews()
{
    QTreeWidgetItem* topViewsItem = createTopItem("Views");

    foreach (QSharedPointer<View> view, component_->getViews())
    {
        if (referenceExistsInParameters(view->getParameters()))
        {
            QTreeWidgetItem* viewItem = createMiddleItem(view->getName(), topViewsItem);

            foreach (QSharedPointer<Parameter> parameter, view->getParameters())
            {
                if (parameterHasReference(parameter))
                {
                    QTreeWidgetItem* parameterItem = createMiddleItem(parameter->getName(), viewItem);
                    createItemsForParameter(parameter, parameterItem);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInPorts()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInPorts()
{
    foreach(QSharedPointer<Port> port, component_->getPorts())
    {
        if (portHasreference(port))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::portHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::portHasreference(QSharedPointer<Port> port)
{
    if (port->getLeftBoundExpression().contains(targetID_) || port->getRightBoundExpression().contains(targetID_)
        || port->getDefaultValue().contains(targetID_))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForPorts()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForPorts()
{
    QTreeWidgetItem* topPortsItem = createTopItem("Ports");

    foreach (QSharedPointer<Port> port, component_->getPorts())
    {
        if (portHasreference(port))
        {
            QTreeWidgetItem* portItem = createMiddleItem(port->getName(), topPortsItem);
            createItemsForPort(port, portItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInMemoryMaps()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMemoryMaps()
{
    foreach (QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
    {
        if (referenceExistsInSingleMemoryMap(memoryMap))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSingleMemoryMap()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap)
{
    foreach(QSharedPointer<MemoryMapItem> addressBlockItem, memoryMap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = addressBlockItem.dynamicCast<AddressBlock>();

        if (addressBlock)
        {
            if (referenceExistsInAddressBlock(addressBlock))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInAddressBlock()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressBlock(QSharedPointer<AddressBlock> addressBlock)
{
    foreach (QSharedPointer<RegisterModel> registerItem, addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerItem.dynamicCast<Register>();

        if (targetRegister)
        {
            if (registerHasReference(targetRegister))
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::registerHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::registerHasReference(QSharedPointer<Register> targetRegister)
{
    if (targetRegister->getAddressOffset().contains(targetID_) ||
        targetRegister->getDimensionExpression().contains(targetID_) ||
        targetRegister->getSizeExpression().contains(targetID_))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForMemoryMaps()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForMemoryMaps()
{
    QTreeWidgetItem* topMemoryMapItem = createTopItem("Memory maps");

    foreach (QSharedPointer<MemoryMap> memoryMap, component_->getMemoryMaps())
    {
        if (referenceExistsInSingleMemoryMap(memoryMap))
        {
            createReferencesForSingleMemoryMap(memoryMap, topMemoryMapItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSingleMemoryMap()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleMemoryMap(QSharedPointer<MemoryMap> memoryMap,
    QTreeWidgetItem* topMemoryMapItem)
{
    QTreeWidgetItem* memoryMapTreeItem = createMiddleItem(memoryMap->getName(), topMemoryMapItem);

    QTreeWidgetItem* middleAddressBlocksItem = createMiddleItem("Address blocks", memoryMapTreeItem);

    colourItemGrey(middleAddressBlocksItem);

    foreach (QSharedPointer<MemoryMapItem> memorymapItem, memoryMap->getItems())
    {
        QSharedPointer<AddressBlock> addressBlock = memorymapItem.dynamicCast<AddressBlock>();

        if (addressBlock)
        {
            if (referenceExistsInAddressBlock(addressBlock))
            {
                createReferencesForSingleAddressBlock(addressBlock, middleAddressBlocksItem);

            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSingleAddressBlock()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleAddressBlock(QSharedPointer<AddressBlock> addressBlock,
    QTreeWidgetItem* middleAddressBlocksItem)
{
    QTreeWidgetItem* addressBlockItem = createMiddleItem(addressBlock->getName(),
        middleAddressBlocksItem);

    foreach (QSharedPointer<RegisterModel> registerModel,
        addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerModel.dynamicCast<Register>();

        if (targetRegister)
        {
            if (registerHasReference(targetRegister))
            {
                QTreeWidgetItem* registerItem = createMiddleItem(targetRegister->getName(),
                    addressBlockItem);
                createItemsForRegister(targetRegister, registerItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createTopItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createTopItem(QString const& itemName)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem();
    newItem->setText(ITEM_NAME, itemName);

    addTopLevelItem(newItem);

    colourItemGrey(newItem);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createMiddleItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* ParameterReferenceTree::createMiddleItem(QString const& itemnName, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemnName);

    return newItem;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForParameter()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForParameter(QSharedPointer<Parameter> parameter, QTreeWidgetItem* parent)
{
    if (parameter->getValue().contains(targetID_))
    {
        QString itemName = "Value";
        QString expression = parameter->getValue();
        createItem(itemName, expression, parent);
    }
    if (parameter->getBitWidth().contains(targetID_))
    {
        QString itemName = "Bit Width";
        QString expression = parameter->getBitWidth();
        createItem(itemName, expression, parent);
    }
    if (parameter->getAttribute("arraySize").contains(targetID_))
    {
        QString itemName = "Array Size";
        QString expression = parameter->getAttribute("arraySize");
        createItem(itemName, expression, parent);
    }
    if (parameter->getAttribute("arrayOffset").contains(targetID_))
    {
        QString itemName = "Array Offset";
        QString expression = parameter->getAttribute("arrayOffset");
        createItem(itemName, expression, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForPort()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForPort(QSharedPointer<Port> port, QTreeWidgetItem* parent)
{
    if (port->getLeftBoundExpression().contains(targetID_))
    {
        QString itemName = "Left (Higher) Bound";
        QString expression = port->getLeftBoundExpression();
        createItem(itemName, expression, parent);
    }
    if (port->getRightBoundExpression().contains(targetID_))
    {
        QString itemName = "Right (Lower) Bound";
        QString expression = port->getRightBoundExpression();
        createItem(itemName, expression, parent);
    }
    if (port->getDefaultValue().contains(targetID_))
    {
        QString itemName = "Default Value";
        QString expression = port->getDefaultValue();
        createItem(itemName, expression, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForRegister()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForRegister(QSharedPointer<Register> targetRegister,
    QTreeWidgetItem* parent)
{
    if (targetRegister->getAddressOffset().contains(targetID_))
    {
        QString itemName = "Offset";
        QString expression = targetRegister->getAddressOffset();
        createItem(itemName, expression, parent);
    }
    if (targetRegister->getDimensionExpression().contains(targetID_))
    {
        QString itemName = "Dimension";
        QString expression = targetRegister->getDimensionExpression();
        createItem(itemName, expression, parent);
    }
    if (targetRegister->getSizeExpression().contains(targetID_))
    {
        QString itemName = "Size";
        QString expression = targetRegister->getSizeExpression();
        createItem(itemName, expression, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItem()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItem
    (QString const& itemName, QString const& expression, QTreeWidgetItem* parent)
{
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parent);
    newItem->setText(ITEM_NAME, itemName);
    newItem->setText(ITEM_EXPRESSION, expressionFormatter_->formatReferringExpression(expression));
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::colourItemGrey()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::colourItemGrey(QTreeWidgetItem* item)
{
    for (int columnIndex = 0; columnIndex < COLUMN_COUNT; ++columnIndex)
    {
        item->setBackgroundColor(columnIndex, QColor(230, 230, 230));
    }
}
