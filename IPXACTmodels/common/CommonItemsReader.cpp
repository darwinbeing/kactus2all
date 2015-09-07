//-----------------------------------------------------------------------------
// File: CommonItemsReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.09.2015
//
// Description:
// Reader class for common IP-XACT elements: VLNV, parameters, assertions, vendor extensions and presence.
//-----------------------------------------------------------------------------

#include "CommonItemsReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::CommonItemsReader(QObject* parent /* = 0 */) :
QObject(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::~CommonItemsReader()
//-----------------------------------------------------------------------------
CommonItemsReader::~CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::createVLNVFrom()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::createVLNVFrom(QDomNode const& vlnvNode, VLNV::IPXactType type) const
{
    QString vendor = vlnvNode.firstChildElement("ipxact:vendor").firstChild().nodeValue();
    QString library = vlnvNode.firstChildElement("ipxact:library").firstChild().nodeValue();
    QString name = vlnvNode.firstChildElement("ipxact:name").firstChild().nodeValue();
    QString version = vlnvNode.firstChildElement("ipxact:version").firstChild().nodeValue();

    VLNV itemVLNV;
    itemVLNV.setType(type);
    itemVLNV.setVendor(vendor);
    itemVLNV.setLibrary(library);
    itemVLNV.setName(name);
    itemVLNV.setVersion(version);

    return itemVLNV;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVLNVAttributes()
//-----------------------------------------------------------------------------
VLNV CommonItemsReader::parseVLNVAttributes(QDomNode const& vlnvNode, VLNV::IPXactType vlnvType) const
{
    QDomNamedNodeMap attributes = vlnvNode.attributes();

    VLNV attributedVLNV;
    attributedVLNV.setType(vlnvType);
    attributedVLNV.setVendor(attributes.namedItem("vendor").nodeValue());
    attributedVLNV.setLibrary(attributes.namedItem("library").nodeValue());
    attributedVLNV.setName(attributes.namedItem("name").nodeValue());
    attributedVLNV.setVersion(attributes.namedItem("version").nodeValue());

    return attributedVLNV;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAndCreateParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > CommonItemsReader::parseAndCreateParameters(
    QDomNode const& itemNode) const
{
    QDomNodeList parameterNodeList = itemNode.firstChildElement("ipxact:parameters").childNodes();
    
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters (new QList<QSharedPointer<Parameter> > ());
    if (!parameterNodeList.isEmpty())
    {
        ParameterReader parameterReader;

        int parameterCount = parameterNodeList.count();
        for (int parameterIndex = 0;parameterIndex < parameterCount; parameterIndex++)
        {
            newParameters->append(parameterReader.createParameterFrom(parameterNodeList.at(parameterIndex)));
        }
    }
    return newParameters;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseAndCreateAssertions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Assertion> > > CommonItemsReader::parseAndCreateAssertions(
    QDomNode const& itemNode) const
{
    QDomNodeList assertionNodeList = itemNode.firstChildElement("ipxact:assertions").childNodes();
    
    QSharedPointer<QList<QSharedPointer<Assertion> > > newAssertions(new QList<QSharedPointer<Assertion> > ()); 

    if (!assertionNodeList.isEmpty())
    {
        int assertionCount = assertionNodeList.count();
        for (int i = 0; i < assertionCount; i++)
        {
            QDomNode assertionNode = assertionNodeList.at(i);

            QSharedPointer<Assertion> assertion(new Assertion());
            assertion->setName(assertionNode.firstChildElement("ipxact:name").firstChild().nodeValue());
            assertion->setDisplayName(assertionNode.firstChildElement("ipxact:displayName").firstChild().nodeValue());
            assertion->setDescription(assertionNode.firstChildElement("ipxact:description").firstChild().nodeValue());
            assertion->setAssert(assertionNode.firstChildElement("ipxact:assert").firstChild().nodeValue());

            newAssertions->append(assertion);
        }
    }
    return newAssertions;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseVendorExtensions()
//-----------------------------------------------------------------------------
void CommonItemsReader::parseVendorExtensions(QDomNode const& itemNode, QSharedPointer<Extendable> element)
    const
{
    QDomNodeList extensionNodes = itemNode.firstChildElement("ipxact:vendorExtensions").childNodes();

    int extensionCount = extensionNodes.count();
    for (int i = 0; i < extensionCount; i++)
    {
        QDomNode extensionNode = extensionNodes.at(i);

        if (!extensionNode.nodeName().startsWith("kactus2:"))
        {
            QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
            element->getVendorExtensions()->append(extension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableVLNVReference()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> CommonItemsReader::parseConfigurableVLNVReference(
    QDomNode const& configurableVLNVNode, VLNV::IPXactType type) const
{
    QDomNamedNodeMap attributeMap = configurableVLNVNode.attributes();

    QString vendor = attributeMap.namedItem("vendor").nodeValue();
    QString library = attributeMap.namedItem("library").nodeValue();
    QString name = attributeMap.namedItem("name").nodeValue();
    QString version = attributeMap.namedItem("version").nodeValue();

    QSharedPointer<ConfigurableVLNVReference> vlnvReference(
        new ConfigurableVLNVReference(type, vendor, library, name, version));

    QDomNode configurableElementsNode = configurableVLNVNode.firstChildElement("ipxact:configurableElementValues");

    QDomNodeList configurableElementNodeList = configurableElementsNode.childNodes();
    for (int i = 0; i < configurableElementNodeList.size(); ++i)
    {
        QSharedPointer<ConfigurableElementValue> newConfigurableElementValue =
            parseConfigurableElementValue(configurableElementNodeList.at(i));

        vlnvReference->getConfigurableElementValues()->append(newConfigurableElementValue);
    }

    return vlnvReference;
}

//-----------------------------------------------------------------------------
// Function: CommonItemsReader::parseConfigurableElementValue()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableElementValue> CommonItemsReader::parseConfigurableElementValue(
    QDomNode const& configurableElementNode) const
{
    QSharedPointer<ConfigurableElementValue> newConfigurableElementValue (new ConfigurableElementValue());

    newConfigurableElementValue->setConfigurableValue(configurableElementNode.firstChild().nodeValue());

    QDomNamedNodeMap attributeMap = configurableElementNode.attributes();

    for (int i = 0; i < attributeMap.size(); ++i)
    {
        QDomNode attributeItem = attributeMap.item(i);
        newConfigurableElementValue->insertAttribute(
            attributeItem.nodeName(), attributeItem.firstChild().nodeValue());
    }

    return newConfigurableElementValue;
}