//-----------------------------------------------------------------------------
// File: NameValuePair.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Describes the ipxact:nameValuePair element.
//-----------------------------------------------------------------------------

#include "NameValuePair.h"

//-----------------------------------------------------------------------------
// Function: NameValuePair::NameValuePair()
//-----------------------------------------------------------------------------
NameValuePair::NameValuePair(QString const& name /* = QString() */, QString const& value /* = QString() */) :
NameGroup(name),
Extendable(),
value_(value)
{

}

//-----------------------------------------------------------------------------
// Function: NameValuePair::NameValuePair()
//-----------------------------------------------------------------------------
NameValuePair::NameValuePair(const NameValuePair& other):
NameGroup(other),
Extendable(other),
value_(other.value_)
{

}

//-----------------------------------------------------------------------------
// Function: NameValuePair::operator=()
//-----------------------------------------------------------------------------
NameValuePair& NameValuePair::operator=(const NameValuePair& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        value_ = other.value_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: NameValuePair::~NameValuePair()
//-----------------------------------------------------------------------------
NameValuePair::~NameValuePair()
{

}

/*
bool File::Define::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	if (nameGroup_.name().isEmpty()) {
		errorList.append(QObject::tr(
			"Mandatory name missing for define within %1").arg(parentIdentifier));
		valid = false;
	}
	if (value_.isEmpty()) {
		errorList.append(QObject::tr("Mandatory value for define missing in %1").arg(
			parentIdentifier));
		valid = false;
	}
	return valid;
}

bool File::Define::isValid() const {
	if (nameGroup_.name().isEmpty()) {
		return false;
	}
	if (value_.isEmpty()) {
		return false;
	}
	return true;
}*/

//-----------------------------------------------------------------------------
// Function: NameValuePair::getValue()
//-----------------------------------------------------------------------------
QString NameValuePair::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: NameValuePair::setValue()
//-----------------------------------------------------------------------------
void NameValuePair::setValue(QString const& newValue)
{
    value_ = newValue;
}