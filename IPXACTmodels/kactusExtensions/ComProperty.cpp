//-----------------------------------------------------------------------------
// File: ComProperty.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 2.4.2012
//
// Description:
// Property class for communication definitions.
//-----------------------------------------------------------------------------

#include "ComProperty.h"

#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty() :
name_(),
required_(true),
type_("string"),
defaultValue_(""),
desc_("")
{

}

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty(ComProperty const& rhs) :
name_(rhs.name_),
required_(rhs.required_),
type_(rhs.type_),
defaultValue_(rhs.defaultValue_),
desc_(rhs.desc_)
{

}

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty(QDomNode& node) :
name_(),
required_(true),
type_("string"),
defaultValue_(""),
desc_("")
{
    name_ = node.attributes().namedItem("kactus2:name").nodeValue();
    required_ = General::str2Bool(node.attributes().namedItem("kactus2:required").nodeValue(), false);
    type_ = node.attributes().namedItem("kactus2:propertyType").nodeValue();
    defaultValue_ = node.attributes().namedItem("kactus2:defaultValue").nodeValue();
    desc_ = node.attributes().namedItem("kactus2:description").nodeValue();
}

//-----------------------------------------------------------------------------
// Function: ComProperty::~ComProperty()
//-----------------------------------------------------------------------------
ComProperty::~ComProperty()
{

}

//-----------------------------------------------------------------------------
// Function: ComProperty::clone()
//-----------------------------------------------------------------------------
ComProperty* ComProperty::clone() const
{
    return new ComProperty(*this);
}

//-----------------------------------------------------------------------------
// Function: ComProperty::type()
//-----------------------------------------------------------------------------
QString ComProperty::type() const
{
    return QString("kactus2:property");
}

//-----------------------------------------------------------------------------
// Function: ComProperty::write()
//-----------------------------------------------------------------------------
void ComProperty::write(QXmlStreamWriter& writer) const
{
    writer.writeEmptyElement("kactus2:property");
    writer.writeAttribute("name", name_);
    writer.writeAttribute("required", General::bool2Str(required_));
    writer.writeAttribute("propertyType", type_);
    writer.writeAttribute("defaultValue", defaultValue_);
    writer.writeAttribute("description", desc_);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::isValid()
//-----------------------------------------------------------------------------
bool ComProperty::isValid(QStringList& errorList, QString const& parentId) const
{
    bool valid = true;
    QString const thisId = QObject::tr("COM property '%1'").arg(name_);

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for a COM property in %1").arg(parentId));
        valid = false;
    }

    if (type_.isEmpty())
    {
        errorList.append(QObject::tr("No type specified for %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::isValid()
//-----------------------------------------------------------------------------
bool ComProperty::isValid() const
{
    return (!name_.isEmpty() && !type_.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setName()
//-----------------------------------------------------------------------------
void ComProperty::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setRequired()
//-----------------------------------------------------------------------------
void ComProperty::setRequired(bool required)
{
    required_ = required;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setType()
//-----------------------------------------------------------------------------
void ComProperty::setType(QString const& type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setDefaultValue()
//-----------------------------------------------------------------------------
void ComProperty::setDefaultValue(QString const& defaultValue)
{
    defaultValue_ = defaultValue;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setDescription()
//-----------------------------------------------------------------------------
void ComProperty::setDescription(QString const& description)
{
    desc_ = description;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::name()
//-----------------------------------------------------------------------------
QString const& ComProperty::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::isRequired()
//-----------------------------------------------------------------------------
bool ComProperty::isRequired() const
{
    return required_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getType()
//-----------------------------------------------------------------------------
QString const& ComProperty::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getDefaultValue()
//-----------------------------------------------------------------------------
QString const& ComProperty::getDefaultValue() const
{
    return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getDescription()
//-----------------------------------------------------------------------------
QString const& ComProperty::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::operator=()
//-----------------------------------------------------------------------------
ComProperty& ComProperty::operator=(ComProperty const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        required_ = rhs.required_;
        type_ = rhs.type_;
        defaultValue_ = rhs.defaultValue_;
        desc_ = rhs.desc_;
    }

    return *this;
}