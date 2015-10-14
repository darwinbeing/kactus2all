//-----------------------------------------------------------------------------
// File: OtherClockDriver.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// Writer class for ipxact:otherClockDriver element.
//-----------------------------------------------------------------------------

#include "OtherClockDriver.h"

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::OtherClockDriver()
//-----------------------------------------------------------------------------
OtherClockDriver::OtherClockDriver(QString const& clockName /* = QString() */) :
clockName_(clockName),
clockSource_(),
clockPeriod_(new ClockUnit()),
clockPulseOffset_(new ClockUnit()),
clockPulseValue_(),
clockPulseDuration_(new ClockUnit())
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::OtherClockDriver()
//-----------------------------------------------------------------------------
OtherClockDriver::OtherClockDriver( const OtherClockDriver &other ):
clockName_(other.clockName_),
clockSource_(other.clockSource_),
clockPeriod_(new ClockUnit()),
clockPulseOffset_(new ClockUnit()),
clockPulseValue_(other.clockPulseValue_),
clockPulseDuration_(new ClockUnit())
{
    copyClockData(other);
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::operator=()
//-----------------------------------------------------------------------------
OtherClockDriver & OtherClockDriver::operator=( const OtherClockDriver &other )
{
	if (this != &other)
    {
		clockName_ = other.clockName_;
		clockSource_ = other.clockSource_;
        clockPulseValue_ = other.clockPulseValue_;

        copyClockData(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::()
//-----------------------------------------------------------------------------
OtherClockDriver::~OtherClockDriver()
{
    clockPeriod_.clear();
    clockPulseOffset_.clear();
    clockPulseDuration_.clear();
}
/*
bool OtherClockDriver::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (clockName_.isEmpty()) {
		errorList.append(QObject::tr("Name missing in other clock driver within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (!clockPeriod_) {
		errorList.append(QObject::tr("Clock period missing in other clock driver"
			" %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseOffset_) {
		errorList.append(QObject::tr("Clock pulse offset missing in other clock "
			"driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseValue_) {
		errorList.append(QObject::tr("Clock pulse value missing in other clock "
			"driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}
	else if (clockPulseValue_->value_ != 0 && clockPulseValue_->value_ != 1) {
		errorList.append(QObject::tr("Invalid value set for clock pulse value in"
			" other clock driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	if (!clockPulseDuration_) {
		errorList.append(QObject::tr("Clock pulse duration missing in other clock"
			" driver %1 within %2").arg(clockName_).arg(parentIdentifier));
		valid = false;
	}

	return valid;
}
*//*
bool OtherClockDriver::isValid() const {

	if (clockName_.isEmpty())
		return false;
	else if (!clockPeriod_)
		return false;
	else if (!clockPulseOffset_)
		return false;
	else if (!clockPulseValue_)
		return false;
	else if (clockPulseValue_->value_ != 0 && clockPulseValue_->value_ != 1)
		return false;
	else if (!clockPulseDuration_)
		return false;
	else 
		return true;
}*/

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockName()
//-----------------------------------------------------------------------------
QString OtherClockDriver::getClockName() const
{
	return clockName_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockName()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockName(QString const& clockName)
{
	clockName_ = clockName;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockSource()
//-----------------------------------------------------------------------------
QString OtherClockDriver::getClockSource() const
{
    return clockSource_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockSource()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockSource(QString const& clockSource)
{
    clockSource_ = clockSource;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockPeriod()
//-----------------------------------------------------------------------------
QSharedPointer<ClockUnit> OtherClockDriver::getClockPeriod() const
{
    return clockPeriod_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockPeriod()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockPeriod(QSharedPointer<ClockUnit> newClockPeriod)
{
    clockPeriod_ = newClockPeriod;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockPulseOffset()
//-----------------------------------------------------------------------------
QSharedPointer<ClockUnit> OtherClockDriver::getClockPulseOffset() const
{
    return clockPulseOffset_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockPulseOffset()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockPulseOffset(QSharedPointer<ClockUnit> newClockPulseOffset)
{
    clockPulseOffset_ = newClockPulseOffset;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockPulseValue()
//-----------------------------------------------------------------------------
QString OtherClockDriver::getClockPulseValue() const
{
    return clockPulseValue_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockPulseValue()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockPulseValue(QString const& newClockPulseValue)
{
    clockPulseValue_ = newClockPulseValue;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::getClockPulseDuration()
//-----------------------------------------------------------------------------
QSharedPointer<ClockUnit> OtherClockDriver::getClockPulseDuration() const
{
    return clockPulseDuration_;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::setClockPulseDuration()
//-----------------------------------------------------------------------------
void OtherClockDriver::setClockPulseDuration(QSharedPointer<ClockUnit> newClockPulseDuration)
{
    clockPulseDuration_ = newClockPulseDuration;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriver::copyClockData()
//-----------------------------------------------------------------------------
void OtherClockDriver::copyClockData(const OtherClockDriver &other)
{
    if (other.clockPeriod_)
    {
        clockPeriod_ = QSharedPointer<ClockUnit>(new ClockUnit(*other.clockPeriod_.data()));
    }

    if (other.clockPulseOffset_)
    {
        clockPulseOffset_ = QSharedPointer<ClockUnit>(new ClockUnit(*other.clockPulseOffset_.data()));
    }

    if (other.clockPulseDuration_)
    {
        clockPulseDuration_ = QSharedPointer<ClockUnit>(new ClockUnit(*other.clockPulseDuration_.data()));
    }
}