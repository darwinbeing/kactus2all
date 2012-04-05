//-----------------------------------------------------------------------------
// File: ComInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 5.4.2012
//
// Description:
// COM interface class for defining properties of a specific COM interface
// in a SW component.
//-----------------------------------------------------------------------------

#include "ComInterface.h"

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface() : name_(), displayName_(), desc_(),
                               comType_(), dataType_(), dir_(General::INOUT),
                               propertyValues_()
{
}

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface(ComInterface const& rhs) : name_(rhs.name_),
                                                      displayName_(rhs.displayName_),
                                                      desc_(rhs.desc_),
                                                      comType_(rhs.comType_),
                                                      dataType_(rhs.dataType_),
                                                      dir_(rhs.dir_),
                                                      propertyValues_(rhs.propertyValues_)
{
}

//-----------------------------------------------------------------------------
// Function: ComInterface::ComInterface()
//-----------------------------------------------------------------------------
ComInterface::ComInterface(QDomNode& node) : name_(), displayName_(), desc_(), comType_(),
                                             dataType_(), dir_(General::INOUT),
                                             propertyValues_()
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode childNode = node.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:name")
        {
            name_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:displayName")
        {
            displayName_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:description")
        {
            desc_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:comType")
        {
            comType_ = General::createVLNV(childNode, VLNV::COMDEFINITION);
        }
        else if (childNode.nodeName() == "kactus2:dataType")
        {
            dataType_ = childNode.nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:direction")
        {
            dir_ = General::str2Direction(childNode.nodeValue(), General::INOUT);
        }
        else if (childNode.nodeName() == "kactus2:propertyValues")
        {
            parsePropertyValues(childNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterface::~ComInterface()
//-----------------------------------------------------------------------------
ComInterface::~ComInterface()
{

}

//-----------------------------------------------------------------------------
// Function: ComInterface::write()
//-----------------------------------------------------------------------------
void ComInterface::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:comInterface");

    // Write basic name info.
    writer.writeTextElement("kactus2:name", name_);
    writer.writeTextElement("kactus2:displayName", displayName_);
    writer.writeTextElement("kactus2:description", desc_);

    // Write communication type, data type and communication direction.
    writer.writeEmptyElement("kactus2:comType");
    General::writeVLNVAttributes(writer, &comType_);

    writer.writeTextElement("kactus2:dataType", dataType_);
    writer.writeTextElement("kactus2:comDirection", General::direction2Str(dir_));

    writer.writeStartElement("kactus2:propertyValues");

    // Write property values.
    QMapIterator<QString, QString> iter(propertyValues_);

    while (iter.hasNext())
    {
        iter.next();

        writer.writeEmptyElement("kactus2:propertyValue");
        writer.writeAttribute("kactus2:name", iter.key());
        writer.writeAttribute("kactus2:value", iter.value());
    }

    writer.writeEndElement(); // kactus2:propertyValues

    writer.writeEndElement(); // kactus2:comInterface
}

//-----------------------------------------------------------------------------
// Function: ComInterface::isValid()
//-----------------------------------------------------------------------------
bool ComInterface::isValid(QStringList& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("COM interface '%1'").arg(name_);
    bool valid = true;

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for a COM interface in %1").arg(parentId));
        valid = false;
    }

    if (!comType_.isEmpty() && !comType_.isValid())
    {
        errorList.append(QObject::tr("Invalid COM type set in %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::isValid()
//-----------------------------------------------------------------------------
bool ComInterface::isValid() const
{
    return (!name_.isEmpty() && (comType_.isEmpty() || comType_.isValid()));
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setName()
//-----------------------------------------------------------------------------
void ComInterface::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setDisplayName()
//-----------------------------------------------------------------------------
void ComInterface::setDisplayName(QString const& displayName)
{
    displayName_ = displayName;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setDescription()
//-----------------------------------------------------------------------------
void ComInterface::setDescription(QString const& desc)
{
    desc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setComType()
//-----------------------------------------------------------------------------
void ComInterface::setComType(VLNV const& vlnv)
{
    comType_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::setDirection()
//-----------------------------------------------------------------------------
void ComInterface::setDirection(General::Direction dir)
{
    dir_ = dir;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getName()
//-----------------------------------------------------------------------------
QString const& ComInterface::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDisplayName()
//-----------------------------------------------------------------------------
QString const& ComInterface::getDisplayName() const
{
    return displayName_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDescription()
//-----------------------------------------------------------------------------
QString const& ComInterface::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getComType()
//-----------------------------------------------------------------------------
VLNV const& ComInterface::getComType() const
{
    return comType_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::getDirection()
//-----------------------------------------------------------------------------
General::Direction ComInterface::getDirection() const
{
    return dir_;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::operator=()
//-----------------------------------------------------------------------------
ComInterface& ComInterface::operator=(ComInterface const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        displayName_ = rhs.displayName_;
        desc_ = rhs.desc_;
        comType_ = rhs.comType_;
        dir_ = rhs.dir_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: ComInterface::parsePropertyValues()
//-----------------------------------------------------------------------------
void ComInterface::parsePropertyValues(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:propertyValue")
        {
            QString name = propNode.attributes().namedItem("kactus2:name").nodeValue();
            QString value = propNode.attributes().namedItem("kactus:value").nodeValue();

            propertyValues_.insert(name, value);
        }
    }
}
