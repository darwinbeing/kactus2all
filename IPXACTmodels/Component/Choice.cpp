//-----------------------------------------------------------------------------
// File: choice.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.10.2010
//
// Description:
// Equals the spirit:choice element in IP-Xact specification.
//-----------------------------------------------------------------------------

#include "choice.h"

#include <IPXACTmodels/common/Enumeration.h>

#include <QList>
#include <QString>

//-----------------------------------------------------------------------------
// Function: Choice::Choice()
//-----------------------------------------------------------------------------
Choice::Choice() : choiceName_(), enumerations_(new QList<QSharedPointer<Enumeration> >())
{
    
}

//-----------------------------------------------------------------------------
// Function: Choice::Choice()
//-----------------------------------------------------------------------------
Choice::Choice( const Choice& other ):
choiceName_(other.choiceName_),
enumerations_(new QList<QSharedPointer<Enumeration> >())
{
    foreach (QSharedPointer<Enumeration> enumeration, *other.enumerations_)
    {
        QSharedPointer<Enumeration> copy(new Enumeration(*enumeration));
        enumerations_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: Choice::operator=()
//-----------------------------------------------------------------------------
Choice& Choice::operator=( const Choice& other )
{
	if (this != &other)
    {
		choiceName_ = other.choiceName_;
		
        enumerations_->clear();
        foreach (QSharedPointer<Enumeration> enumeration, *other.enumerations_)
        {
            QSharedPointer<Enumeration> copy(new Enumeration(*enumeration));
            enumerations_->append(copy);
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Choice::~Choice()
//-----------------------------------------------------------------------------
Choice::~Choice()
{

}

//-----------------------------------------------------------------------------
// Function: Choice::isValid()
//-----------------------------------------------------------------------------
bool Choice::isValid( QStringList& errorList, const QString& parentIdentifier ) const
{
	bool valid = true;
	
	if (choiceName_.isEmpty())
    {
		errorList.append(QObject::tr("No name specified for choice within %1").arg(parentIdentifier));
		valid = false;
	}

	if (enumerations_->isEmpty())
    {
		errorList.append(QObject::tr("At least one enumeration is required in choice %1"
            " within %2").arg(choiceName_).arg(parentIdentifier));
		valid = false;
	}

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue().isEmpty())
        {
            errorList.append(QObject::tr("No value specified for enumeration in choice %1"
                " within %2").arg(choiceName_).arg(parentIdentifier));
            valid = false;
        }
    }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: Choice::isValid()
//-----------------------------------------------------------------------------
bool Choice::isValid() const
{
    if (choiceName_.isEmpty())
    {
        return false;
    }

    if (enumerations_->isEmpty())
    {
        return false;
    }

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue().isEmpty())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: Choice::name()
//-----------------------------------------------------------------------------
QString Choice::name() const
{
	return choiceName_;
}

//-----------------------------------------------------------------------------
// Function: Choice::setName()
//-----------------------------------------------------------------------------
void Choice::setName(const QString& name)
{
	choiceName_ = name;
}

//-----------------------------------------------------------------------------
// Function: Choice::hasEnumeration()
//-----------------------------------------------------------------------------
bool Choice::hasEnumeration(QString const& enumerationValue) const
{
    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (enumeration->getValue() == enumerationValue)
        {
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: Choice::enumerationCount()
//-----------------------------------------------------------------------------
QStringList Choice::getEnumerationValues() const
{
    QStringList enumerationValues;

    foreach (QSharedPointer<Enumeration> enumeration, *enumerations_)
    {
        if (!enumeration->getText().isEmpty())
        {
            enumerationValues.append(enumeration->getText());
        }
        else
        {
            enumerationValues.append(enumeration->getValue());
        }
    }

    return enumerationValues;
}

//-----------------------------------------------------------------------------
// Function: Choice::enumerations()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Enumeration> > > Choice::enumerations() const
{
    return enumerations_;
}