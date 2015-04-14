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
#include <IPXACTmodels/businterface.h>

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
        if (referenceExistsInModelParameters(component_->getModelParameters()))
        {
            QTreeWidgetItem* topModelParameterItem = createTopItem("Model Parameters");
            createReferencesForModelParameters(component_->getModelParameters(), topModelParameterItem);
        }

        if (referenceExistsInParameters(component_->getParameters()))
        {
            QTreeWidgetItem* topParametersItem = createTopItem("Parameters");
            createParameterReferences(component_->getParameters(), topParametersItem);
        }

        if (referenceExistsInMemoryMaps())
        {
            createReferencesForMemoryMaps();
        }

        if (referenceExistsInAddressSpaces())
        {
            createReferencesForAddressSpaces();
        }

        if (referenceExistsInViews())
        {
            createReferencesForViews();
        }

        if (referenceExistsInPorts())
        {
            createReferencesForPorts();
        }

        if (referenceExistsInBusInterfaces())
        {
            createReferencesForBusInterfaces();
        }

        if (referenceExistsInRemapStates())
        {
            createReferencesForRemapStates();
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
bool ParameterReferenceTree::referenceExistsInParameters(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameterList)
{
    foreach (QSharedPointer<Parameter> parameter, *parameterList)
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
    if (parameter->getValue().contains(targetID_) ||
        parameter->getBitWidthLeft().contains(targetID_) || parameter->getBitWidthRight().contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayLeft").contains(targetID_) ||
        parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createParameterReferences()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createParameterReferences(
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<Parameter> parameter, *parameters)
    {
        if (parameterHasReference(parameter))
        {
            QTreeWidgetItem* parameterItem = createMiddleItem(parameter->getName(), parentItem);
            createItemsForParameter(parameter, parameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInModelParameters()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInModelParameters(
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters)
{
    foreach(QSharedPointer<Parameter> modelParameter, *modelParameters)
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
void ParameterReferenceTree::createReferencesForModelParameters(
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParametes, QTreeWidgetItem* parentItem)
{
    foreach (QSharedPointer<ModelParameter> modelParameter, *modelParametes)
    {
        if (parameterHasReference(modelParameter))
        {
            QTreeWidgetItem* modelParameterItem = createMiddleItem(modelParameter->getName(), parentItem);
            createItemsForParameter(modelParameter, modelParameterItem);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInAddressSpaces()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressSpaces()
{
    foreach(QSharedPointer<AddressSpace> addressSpace, component_->getAddressSpaces())
    {
        if (referenceExistsInSingleMemoryMap(addressSpace->getLocalMemoryMap()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInViews()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInViews()
{
    foreach(QSharedPointer<View> view, component_->getViews())
    {
        if (referenceExistsInView(view))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInView()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInView(QSharedPointer<View> view)
{
    foreach (QSharedPointer<Parameter> parameter, *view->getParameters())
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    foreach(QSharedPointer<ModelParameter> moduleParameter, *view->getModuleParameters())
    {
        if (parameterHasReference(moduleParameter))
        {
            return true;
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
        if (referenceExistsInView(view))
        {
            QTreeWidgetItem* viewItem = createMiddleItem(view->getName(), topViewsItem);
            if (referenceExistsInParameters(view->getParameters()))
            {
                QTreeWidgetItem* viewParametersItem = createMiddleItem("Parameters", viewItem);
                colourItemGrey(viewParametersItem);

                createParameterReferences(view->getParameters(), viewParametersItem);
            }
            if (referenceExistsInModelParameters(view->getModuleParameters()))
            {
                QTreeWidgetItem* viewModuleParametersItem = createMiddleItem("Module Parameters", viewItem);
                colourItemGrey(viewModuleParametersItem);

                createReferencesForModelParameters(view->getModuleParameters(), viewModuleParametersItem);
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
    if (port->getLeftBoundExpression().contains(targetID_) ||
        port->getRightBoundExpression().contains(targetID_) || port->getDefaultValue().contains(targetID_) ||
        port->getArrayLeft().contains(targetID_) || port->getArrayRight().contains(targetID_))
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
    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        return true;
    }

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
// Function: ParameterReferenceTree::referenceExistsInAddressBlockValues()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInAddressBlockValues(QSharedPointer<AddressBlock> addressBlock)
{
    return addressBlock->getBaseAddress().contains(targetID_) || addressBlock->getRange().contains(targetID_) ||
        addressBlock->getWidthExpression().contains(targetID_);
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

    if (referenceExistsInRegisterFields(targetRegister))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInRegisterFields()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInRegisterFields(QSharedPointer<Register> targetRegister)
{
    foreach (QSharedPointer<Field> registerField, targetRegister->getFields())
    {
        if (registerFieldHasReference(registerField))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::registerFieldHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::registerFieldHasReference(QSharedPointer<Field> targetField)
{
    if (targetField->getBitOffsetExpression().contains(targetID_) ||
        targetField->getBitWidthExpression().contains(targetID_) ||
        targetField->getIsPresentExpression().contains(targetID_))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForAddressSpaces()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForAddressSpaces()
{
    QTreeWidgetItem* topAddressSpaceItem = createTopItem("Address Spaces");

    foreach (QSharedPointer<AddressSpace> addressSpace, component_->getAddressSpaces())
    {
        if (addressSpace->hasLocalMemoryMap() && referenceExistsInSingleMemoryMap(addressSpace->getLocalMemoryMap()))
        {
            QTreeWidgetItem* addressSpaceItem = createMiddleItem(addressSpace->getName(), topAddressSpaceItem);
            QSharedPointer <MemoryMap> localMemoryMap = addressSpace->getLocalMemoryMap();

            createReferencesForSingleMemoryMap(localMemoryMap, addressSpaceItem);
        }
    }
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

    if (referenceExistsInAddressBlockValues(addressBlock))
    {
        createItemsForAddressBlock(addressBlock, addressBlockItem);
    }

    foreach (QSharedPointer<RegisterModel> registerModel,
        addressBlock->getRegisterData())
    {
        QSharedPointer<Register> targetRegister = registerModel.dynamicCast<Register>();

        if (targetRegister)
        {
            if (registerHasReference(targetRegister))
            {
                createReferencesForSingleRegister(targetRegister, addressBlockItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForSingleRegister()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForSingleRegister(QSharedPointer<Register> targetRegister,
    QTreeWidgetItem* parentItem)
{
    QTreeWidgetItem* registerItem = createMiddleItem(targetRegister->getName(), parentItem);
    createItemsForRegister(targetRegister, registerItem);

    if (referenceExistsInRegisterFields(targetRegister))
    {
        QTreeWidgetItem* fieldsItem = createMiddleItem("Fields", registerItem);
        colourItemGrey(fieldsItem);

        foreach (QSharedPointer<Field> registerField, targetRegister->getFields())
        {
            if (registerFieldHasReference(registerField))
            {
                QTreeWidgetItem* singleFieldItem = createMiddleItem(registerField->getName(), fieldsItem);
                createItemsForField(registerField, singleFieldItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInBusInterfaces()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInBusInterfaces()
{
    foreach (QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
    {
        if (referenceExistsInSingleBusInterface(busInterface))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSingleBusInterface()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleBusInterface(QSharedPointer<BusInterface> busInterface)
{
    foreach (QSharedPointer<Parameter> parameter, *busInterface->getParameters())
    {
        if (parameterHasReference(parameter))
        {
            return true;
        }
    }

    if (busInterface->getMirroredSlave())
    {
        if (referenceExistsInMirroredSlave(busInterface->getMirroredSlave()))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInMirroredSlave()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInMirroredSlave(QSharedPointer<MirroredSlaveInterface> mirrorSlave)
{
    if (mirroredSlaveRemapAddressHasReference(mirrorSlave))
    {
        return true;
    }
    if (mirroredSlaveRangeHasReference(mirrorSlave))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::mirroredSlaveRemapAddressHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::mirroredSlaveRemapAddressHasReference(
    QSharedPointer<MirroredSlaveInterface> mirrorSlave)
{
    if (mirrorSlave->getRemapAddressID() == targetID_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::mirroredSlaveRangeHasReference()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::mirroredSlaveRangeHasReference(QSharedPointer<MirroredSlaveInterface> mirrorSlave)
{
    if (mirrorSlave->getRangeID() == targetID_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForBusInterfaces()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForBusInterfaces()
{
    QTreeWidgetItem* topBusInterfaceItem = createTopItem("Bus Interfaces");

    foreach (QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
    {
        if (referenceExistsInSingleBusInterface(busInterface))
        {
            QTreeWidgetItem* busInterfaceItem = createMiddleItem(busInterface->getName(), topBusInterfaceItem);

            QSharedPointer<MirroredSlaveInterface> mirrorSlave(busInterface->getMirroredSlave());
            if (mirrorSlave)
            {
                if (referenceExistsInMirroredSlave(mirrorSlave))
                {
                    QTreeWidgetItem* mirroredInterfaceItem =
                        createMiddleItem("Mirrored Slave Interface", busInterfaceItem);
                    colourItemGrey(mirroredInterfaceItem);

                    if (mirroredSlaveRemapAddressHasReference(mirrorSlave))
                    {
                        createItem("Remap Address", mirrorSlave->getRemapAddressID(), mirroredInterfaceItem);
                    }
                    if (mirroredSlaveRangeHasReference(mirrorSlave))
                    {
                        createItem("Range", mirrorSlave->getRangeID(), mirroredInterfaceItem);
                    }
                }
            }

            if (referenceExistsInParameters(busInterface->getParameters()))
            {
                createParameterReferences(busInterface->getParameters(), busInterfaceItem);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInRemapStates()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInRemapStates() const
{
    foreach(QSharedPointer<RemapState> remapState, *component_->getRemapStates())
    {
        if (referenceExistsInSingleRemapState(remapState))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSingleRemapState()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleRemapState(QSharedPointer<RemapState> remapState) const
{
    foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
    {
        if (referenceExistsInSingleRemapPort(remapPort))
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::referenceExistsInSingleRemapPort()
//-----------------------------------------------------------------------------
bool ParameterReferenceTree::referenceExistsInSingleRemapPort(QSharedPointer<RemapPort> remapPort) const
{
    if (remapPort->getValue().contains(targetID_))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createReferencesForRemapStates()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createReferencesForRemapStates()
{
    QTreeWidgetItem* topRemapStatesItem = createTopItem("Remap States");

    foreach (QSharedPointer<RemapState> remapState, *component_->getRemapStates())
    {
        if (referenceExistsInSingleRemapState(remapState))
        {
            QTreeWidgetItem* remapStateItem = createMiddleItem(remapState->getName(), topRemapStatesItem);

            QTreeWidgetItem* remapPortsItem = createMiddleItem("Remap Ports", remapStateItem);
            colourItemGrey(remapPortsItem);

            foreach (QSharedPointer<RemapPort> remapPort, *remapState->getRemapPorts())
            {
                if (referenceExistsInSingleRemapPort(remapPort))
                {
                    QString itemName = remapPort->getPortNameRef();

                    if (remapPort->getPortIndex() >= 0)
                    {
                        itemName.append("[" + QString::number(remapPort->getPortIndex()) + "]");
                    }
                    createItem(itemName, remapPort->getValue(), remapPortsItem);
                }
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
    if (parameter->getBitWidthLeft().contains(targetID_))
    {
        QString itemName = "Bit Width Left";
        QString expression = parameter->getBitWidthLeft();
        createItem(itemName, expression, parent);
    }
    if (parameter->getBitWidthRight().contains(targetID_))
    {
        QString itemName = "Bit Width Right";
        QString expression = parameter->getBitWidthRight();
        createItem(itemName, expression, parent);
    }
    if (parameter->getAttribute("kactus2:arrayLeft").contains(targetID_))
    {
        QString itemName = "Array Left";
        QString expression = parameter->getAttribute("kactus2:arrayLeft");
        createItem(itemName, expression, parent);
    }
    if (parameter->getAttribute("kactus2:arrayRight").contains(targetID_))
    {
        QString itemName = "Array Right";
        QString expression = parameter->getAttribute("kactus2:arrayRight");
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
    if (port->getArrayLeft().contains(targetID_))
    {
        QString itemName = "Array Left";
        QString expression = port->getArrayLeft();
        createItem(itemName, expression, parent);
    }
    if (port->getArrayRight().contains(targetID_))
    {
        QString itemName = "Array Right";
        QString expression = port->getArrayRight();
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
// Function: ParameterReferenceTree::createItemsForAddressBlock()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForAddressBlock(QSharedPointer<AddressBlock> targetAddressBlock,
    QTreeWidgetItem* parent)
{
    if (targetAddressBlock->getBaseAddress().contains(targetID_))
    {
        QString itemName = "Base Address";
        QString expression = targetAddressBlock->getBaseAddress();
        createItem(itemName, expression, parent);
    }
    if (targetAddressBlock->getRange().contains(targetID_))
    {
        QString itemName = "Range";
        QString expression = targetAddressBlock->getRange();
        createItem(itemName, expression, parent);
    }
    if (targetAddressBlock->getWidthExpression().contains(targetID_))
    {
        QString itemName = "Width";
        QString expression = targetAddressBlock->getWidthExpression();
        createItem(itemName, expression, parent);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterReferenceTree::createItemsForField()
//-----------------------------------------------------------------------------
void ParameterReferenceTree::createItemsForField(QSharedPointer<Field> targetField, QTreeWidgetItem* parent)
{
    if (targetField->getBitOffsetExpression().contains(targetID_))
    {
        QString itemName = "Offset";
        QString expression = targetField->getBitOffsetExpression();
        createItem(itemName, expression, parent);
    }
    if (targetField->getBitWidthExpression().contains(targetID_))
    {
        QString itemName = "Width";
        QString expression = targetField->getBitWidthExpression();
        createItem(itemName, expression, parent);
    }
    if (targetField->getIsPresentExpression().contains(targetID_))
    {
        QString itemName = "Is Present";
        QString expression = targetField->getIsPresentExpression();
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
