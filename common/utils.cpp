/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: utils.cpp
 * 		
 * 		Description: This file contains definitions for general purpose 
 * 		functions that can be used in the whole software.
 */

#include "utils.h"

#include <qmath.h>
#include <QRegExp>

quint64 Utils::str2Uint( const QString& str ) {

	if (str.isEmpty()) {
		return 0;
	}

	// used to detect if the conversion was successful
	bool success = true;
	quint64 number = 0;

	// if starts with "0x" then it is hexadecimal digit
	if (str.startsWith("0x", Qt::CaseInsensitive)) {
		
		number = str.toULongLong(&success, 16);
		if (success) {
			return number;
		}
	}

	// needed because the last letter is chopped if one is found
	QString strNumber = str;

	// the multiple is the last letter if one exists
	const QChar multiple = strNumber.at(strNumber.size()-1);
	quint64 multiplier = 1;

	// get the correct multiplier and remove the letter from the string
	if (multiple == 'k' || multiple == 'K') {
		multiplier = qPow(2, 10);
		strNumber.chop(1);
	}
	else if (multiple == 'M') {
		multiplier = qPow(2, 20);
		strNumber.chop(1);
	}
	else if (multiple == 'G') {
		multiplier = qPow(2, 30);
		strNumber.chop(1);
	}
	else if (multiple == 'T') {
		multiplier = qPow(2, 40);
		strNumber.chop(1);
	}
	else if (multiple == 'P') {
		multiplier = qPow(2, 50);
		strNumber.chop(1);
	}

	// try to convert the number 
	number = strNumber.toULongLong(&success);

	// if the conversion failed
	if (!success) {
		return 0;
	}
	// otherwise return the correct int-format
	else {
		return number * multiplier;
	}
}

qint64 Utils::str2Int( const QString& str ) {
	if (str.isEmpty()) {
		return 0;
	}

	bool negative = false;
	QString absValueStr = str;

	// if the number is negative
	if (str.startsWith("-", Qt::CaseInsensitive)) {
		negative = true;
		// remove the negative sign from the start
		absValueStr = str.mid(1);
	}

	// get the absolute value of the number
	quint64 absValue = Utils::str2Uint(absValueStr);

	if (negative) {
		return -static_cast<qint64>(absValue);
	}
	else {
		return absValue;
	}
}

bool Utils::isNumber( const QString& str ) {
	if (str.isEmpty()) {
		return false;
	}

	QString absValueStr = str;
	bool success = true;

	// if the number is negative then remove minus sign
	if (str.startsWith("-", Qt::CaseInsensitive)) {
		absValueStr = str.mid(1);
	}

	// if the number is hexadecimal form
	if (absValueStr.startsWith("0x", Qt::CaseInsensitive)) {
		
		// if the hexadecimal number could be converted
		/*quint64 number = */absValueStr.toULongLong(&success, 16);
		return success;
	}

	// check if the string contains a letter for multiples

	// the multiple is the last letter if one exists
	const QChar multiple = absValueStr.at(absValueStr.size()-1);

	// get the correct multiplier and remove the letter from the string
	if (multiple == 'k' || 
		multiple == 'K' ||
		multiple == 'M' ||
		multiple == 'G' ||
		multiple == 'T' ||
		multiple == 'P') {
		absValueStr.chop(1);
	}

	// try to convert the number 
	/*quint64 number = */absValueStr.toULongLong(&success);

	return success;
}

//-----------------------------------------------------------------------------
// Function: Utils::replaceMagicWord()
// Replaces all 'magicWords' in 'text' with (first letters) of 'value'
//-----------------------------------------------------------------------------
void Utils::replaceMagicWord(QString& text, QString const& magicWord, QString const& value)
{
	// User can set how e.g. instance names are formed, e.g. first 3 letters of 
	// name and then a running number. magicWord is e.g. 'ComponentName'

	// Simple case first: replace all occurrences of '$magicWord$' with 'value'
    text.replace(QRegExp(QString("\\$") + magicWord + QString("\\$")), value);


    // Check if only the first d letters of are taken, format '_d'
    QRegExp exp(QString("\\$") + magicWord + QString("_(\\d+)\\$"));

    int index = exp.indexIn(text);
    while (index != -1)
    {
        // Match found, take the #cap leftmost lettes from 'value'
        QString cap = exp.cap(1);
        text.replace(QRegExp(QString("\\$") + magicWord + "_" + cap + "\\$"), value.left(cap.toUInt()));
        index = exp.indexIn(text);
    }
}

Utils::Mapping::Mapping(): portName_(QString("---")), left_(0), right_(0){
}

Utils::Mapping::Mapping( const QString& name ): 
portName_(name), left_(0), right_(0) {
}

Utils::Mapping::Mapping( const QString& name, int left, int right ):
portName_(name), left_(left), right_(right) {
}

bool Utils::Mapping::operator==( const Mapping& other ) const {
	return (portName_ == other.portName_);
}

bool Utils::Mapping::operator!=( const Mapping& other ) const {
	return (portName_ != other.portName_);
}

bool Utils::Mapping::operator<( const Mapping& other ) const {
	return portName_ < other.portName_;
}

bool Utils::Mapping::operator>( const Mapping& other ) const {
	return portName_ > other.portName_;
}

Utils::ImplementationOptions::ImplementationOptions():
hw_(true),
sw_(true),
system_(true) {
}

Utils::HierarchyOptions::HierarchyOptions():
flat_(true),
product_(true),
board_(true),
chip_(true),
soc_(true),
ip_(true) {
}

Utils::FirmnessOptions::FirmnessOptions():
templates_(true),
mutable_(true),
fixed_(true)
{
}

Utils::TypeOptions::TypeOptions():
components_(true),
buses_(true),
advanced_(false) {
}

//-----------------------------------------------------------------------------
// Function: Utils::getCurrentUser()
//-----------------------------------------------------------------------------
KACTUS2_API QString Utils::getCurrentUser()
{
#ifdef Q_OS_WIN
#pragma warning (disable: 4996)
    return getenv("USERNAME");
#pragma warning (default: 4996)
#else
    return getenv("USER");
#endif
}
