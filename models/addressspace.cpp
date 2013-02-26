/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#include "addressspace.h"
#include "memorymap.h"
#include "parameter.h"
#include <common/utils.h>

#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QString>
#include <QObject>
#include <QList>
#include <QXmlStreamWriter>

const int DEFAULT_ADDRESS_UNIT_BITS = 8;

// the constructor
AddressSpace::AddressSpace(QDomNode &addrSpaceNode):
nameGroup_(addrSpaceNode),
range_(QString()),
rangeAttributes_(),
width_(-1), 
widthAttributes_(),
segments_(),
addressUnitBits_(DEFAULT_ADDRESS_UNIT_BITS), 
localMemoryMap_(0),
parameters_() {
	for (int i = 0; i < addrSpaceNode.childNodes().count(); ++i) {
		QDomNode tempNode = addrSpaceNode.childNodes().at(i);

		// get range
		if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			// get the attributes
			General::parseAttributes(tempNode, rangeAttributes_);
		}

		// get width
		else if (tempNode.nodeName() == QString("spirit:width")) {
			width_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes
			General::parseAttributes(tempNode, widthAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:segments")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode segmentNode = tempNode.childNodes().at(j);
				segments_.append(QSharedPointer<Segment>(
					new Segment(segmentNode)));
			}
		}

		// get addressUnitBits
		else if (tempNode.nodeName() == QString("spirit:addressUnitBits")) {
			addressUnitBits_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}

		else if (tempNode.nodeName() == QString("spirit:localMemoryMap")) {
			localMemoryMap_ = QSharedPointer<MemoryMap>(
					new MemoryMap(tempNode));
		}

		else if (tempNode.nodeName() == QString("spirit:parameters")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
	}
	return;
}

AddressSpace::AddressSpace( const AddressSpace &other ):
nameGroup_(other.nameGroup_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
width_(other.width_),
widthAttributes_(other.widthAttributes_),
segments_(),
addressUnitBits_(other.addressUnitBits_),
localMemoryMap_(),
parameters_() {

	if (other.localMemoryMap_) {
		localMemoryMap_ = QSharedPointer<MemoryMap>(
			new MemoryMap(*other.localMemoryMap_.data()));
	}

	parameters_.clear();
	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}

	segments_.clear();
	foreach (QSharedPointer<Segment> segment, other.segments_) {
		if (segment) {
			QSharedPointer<Segment> copy = QSharedPointer<Segment>(
				new Segment(*segment.data()));
			segments_.append(copy);
		}
	}
}

AddressSpace::AddressSpace():
nameGroup_(),
range_(),
rangeAttributes_(),
width_(-1), 
widthAttributes_(),
segments_(),
addressUnitBits_(DEFAULT_ADDRESS_UNIT_BITS), 
localMemoryMap_(0),
parameters_() {
}

AddressSpace & AddressSpace::operator=( const AddressSpace &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
		width_ = other.width_;
		widthAttributes_ = other.widthAttributes_;
		addressUnitBits_ = other.addressUnitBits_;

		if (other.localMemoryMap_) {
			localMemoryMap_ = QSharedPointer<MemoryMap>(
				new MemoryMap(*other.localMemoryMap_.data()));
		}
		else
			localMemoryMap_ = QSharedPointer<MemoryMap>();

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}

		segments_.clear();
		foreach (QSharedPointer<Segment> segment, other.segments_) {
			if (segment) {
				QSharedPointer<Segment> copy = QSharedPointer<Segment>(
					new Segment(*segment.data()));
				segments_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
AddressSpace::~AddressSpace() {
	rangeAttributes_.clear();
	widthAttributes_.clear();
	segments_.clear();
}

void AddressSpace::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:addressSpace");

	writer.writeTextElement("spirit:name", nameGroup_.name_);

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

    // start the spirit:range tag
    writer.writeStartElement("spirit:range");

    // write the attributes for the element
    General::writeAttributes(writer, rangeAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(range_);
    writer.writeEndElement(); // spirit:range

    // start the spirit:width element
    writer.writeStartElement("spirit:width");

    // write the attributes for the element
    General::writeAttributes(writer, widthAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(QString::number(width_));
    writer.writeEndElement(); // spirit:width

	if (!segments_.isEmpty()) {

		writer.writeStartElement("spirit:segments");
		foreach (QSharedPointer<Segment> segment, segments_) {
			segment->write(writer);
		}
		writer.writeEndElement(); // spirit:segments
	}

	writer.writeTextElement("spirit:addressUnitBits", QString::number(addressUnitBits_));

	if (localMemoryMap_ && !localMemoryMap_->isEmpty()) {
		writer.writeStartElement("spirit:localMemoryMap");
		localMemoryMap_->write(writer);
		writer.writeEndElement(); // spirit:localMemoryMap
	}

	// if any parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:addressSpace
}

bool AddressSpace::isValid( QStringList& errorList, 
						   const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("address space %1").arg(nameGroup_.name_));

	if (nameGroup_.name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_.isEmpty()) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (width_ < 0) {
		errorList.append(QObject::tr("No width set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<Segment> segment, segments_) {
		if (!segment->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	if (localMemoryMap_) {
		
		// if the local memory map contains definitions but is not valid
		if (!localMemoryMap_->isEmpty() && !localMemoryMap_->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool AddressSpace::isValid() const {
	
	if (nameGroup_.name_.isEmpty()) {
		return false;
	}

	if (range_.isEmpty()) {
		return false;
	}

	if (width_ < 0) {
		return false;
	}

	foreach (QSharedPointer<Segment> segment, segments_) {
		if (!segment->isValid()) {
			return false;
		}
	}

	if (localMemoryMap_) {

		// if the memory map contains definitions but is not valid
		if (!localMemoryMap_->isEmpty() && !localMemoryMap_->isValid()) {
			return false;
		}
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid()) {
			return false;
		}
	}
	return true;
}

void AddressSpace::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

unsigned int AddressSpace::getAddressUnitBits() const {
	return addressUnitBits_;
}

void AddressSpace::setRange(const QString &range) {
	range_ = range;
}

QString AddressSpace::getRange() const {
	return range_;
}

const QMap<QString, QString>& AddressSpace::getRangeAttributes() {
	return rangeAttributes_;
}

void AddressSpace::setRangeAttributes(
		const QMap<QString, QString> &rangeAttributes) {
	// delete old range attributes
	rangeAttributes_.clear();

	// save new range attributes
	rangeAttributes_ = rangeAttributes;
}

void AddressSpace::setName(const QString &name) {
	nameGroup_.name_ = name;
}

int AddressSpace::getWidth() const {
	return width_;
}

const QMap<QString, QString>& AddressSpace::getWidthAttributes() {
	return widthAttributes_;
}

void AddressSpace::setWidthAttributes(
		const QMap<QString, QString> &widthAttributes) {
	// delete old width attributes
	widthAttributes_.clear();

	// save new width attributes
	widthAttributes_ = widthAttributes;
}

QString AddressSpace::getName() const {
	return nameGroup_.name_;
}

void AddressSpace::setWidth(int width) {
	width_ = width;
}

const QSharedPointer<MemoryMap> AddressSpace::getLocalMemoryMap() const {
	return localMemoryMap_;
}

QSharedPointer<MemoryMap> AddressSpace::getLocalMemoryMap() {
	if (!localMemoryMap_) {
		localMemoryMap_ = QSharedPointer<MemoryMap>(new MemoryMap());
	}
	return localMemoryMap_;
}

void AddressSpace::setLocalMemoryMap( QSharedPointer<MemoryMap> localMemoryMap ) {
	if (localMemoryMap_) {
		localMemoryMap_.clear();
	}
	localMemoryMap_ = localMemoryMap;
}

QList<QSharedPointer<Segment> >& AddressSpace::getSegments() {
	return segments_;
}

const QList<QSharedPointer<Segment> >& AddressSpace::getSegments() const {
	return segments_;
}

void AddressSpace::setSegments( const QList<QSharedPointer<Segment> >& segments ) {
	segments_ = segments;
}

void AddressSpace::setDisplayName( const QString& dispName ) {
	nameGroup_.displayName_ = dispName;
}

QString AddressSpace::getDisplayName() const {
	return nameGroup_.displayName_;
}

void AddressSpace::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

QString AddressSpace::getDescription() const {
	return nameGroup_.description_;
}

QList<QSharedPointer<Parameter> >& AddressSpace::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& AddressSpace::getParameters() const {
	return parameters_;
}

General::NameGroup& AddressSpace::getNameGroup() {
	return nameGroup_;
}

const General::NameGroup& AddressSpace::getNameGroup() const {
	return nameGroup_;
}

quint64 AddressSpace::getLastSegmentedAddress() const {
	quint64 lastAddress = 0;
	
	// find the last address defined in a segment
	foreach (QSharedPointer<Segment> segment, segments_) {
		lastAddress = qMax(lastAddress, segment->getLastAddress());
	}
	return lastAddress;
}

quint64 AddressSpace::getLastAddress() const {
	quint64 range = Utils::str2Uint(range_);
	
	// if range is undefined
	if (range <= 0) {
		return 0;
	}
	return range - 1;
}

bool AddressSpace::hasLocalMemoryMap() const {
	return localMemoryMap_;
}
