//-----------------------------------------------------------------------------
// File: DocumentReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.08.2015
//
// Description:
// Base class for XML readers.
//-----------------------------------------------------------------------------

#include "DocumentReader.h"
#include "ParameterReader.h"

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: DocumentReader::DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::DocumentReader(QObject* parent):
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::~DocumentReader()
//-----------------------------------------------------------------------------
DocumentReader::~DocumentReader()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseTopComments()
//-----------------------------------------------------------------------------
void DocumentReader::parseTopComments(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QStringList comments;

    QDomNodeList nodeList = documentNode.childNodes();
    QDomNode singleDocumentNode = documentNode.firstChildElement();

    for (int i = 0; i < nodeList.size() && nodeList.at(i) != singleDocumentNode; ++i)
    {
        if (nodeList.at(i).isComment())
        {
            comments.append(nodeList.at(i).nodeValue());
        }
    }

    document->setTopComments(comments);
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseVLNV()
//-----------------------------------------------------------------------------
void DocumentReader::parseVLNVElements(QDomNode const& documentNode, QSharedPointer<Document> document,
    VLNV::IPXactType type) const
{
    document->setVlnv(createVLNVFrom(documentNode, type));
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseDescription()
//-----------------------------------------------------------------------------
void DocumentReader::parseDescription(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    document->setDescription(documentNode.firstChildElement("ipxact:description").firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseParameters()
//-----------------------------------------------------------------------------
void DocumentReader::parseParameters(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters = parseAndCreateParameters(documentNode);

    foreach (QSharedPointer<Parameter> parameter, *newParameters)
    {
        document->getParameters()->append(parameter);
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseAssertions()
//-----------------------------------------------------------------------------
void DocumentReader::parseAssertions(QDomNode const& documentNode, QSharedPointer<Document> document) const
{
    QDomNodeList assertionNodeList = documentNode.firstChildElement("ipxact:assertions").childNodes();

    if (!assertionNodeList.isEmpty())
    {
        int assertionCount = assertionNodeList.count();
        for (int i = 0; i < assertionCount; ++i)
        {
            QDomNode assertionNode = assertionNodeList.at(i);

            QSharedPointer<Assertion> newAssertion (new Assertion());
            newAssertion->setName(assertionNode.firstChildElement("ipxact:name").firstChild().nodeValue());
            newAssertion->setDisplayName(assertionNode.firstChildElement("ipxact:displayName").firstChild().
                nodeValue());
            newAssertion->setDescription(assertionNode.firstChildElement("ipxact:description").firstChild().
                nodeValue());
            newAssertion->setAssert(assertionNode.firstChildElement("ipxact:assert").firstChild().nodeValue());

            document->getAssertions()->append(newAssertion);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentReader::parseKactusAndVendorExtensions()
//-----------------------------------------------------------------------------
void DocumentReader::parseKactusAndVendorExtensions(QDomNode const& documentNode, 
    QSharedPointer<Document> document) const
{
    QDomElement extensionNodes = documentNode.firstChildElement("ipxact:vendorExtensions");

    QDomNode versionNode = extensionNodes.firstChildElement("kactus2:version");
    if (!versionNode.isNull())
    {
        document->setVersion(versionNode.firstChild().nodeValue());
    }

    parseVendorExtensions(documentNode, document);
}
