/* 
 *
 *  Created on: 2.8.2010
 *      Author: Antti Kamppi
 */

#include "port.h"
#include "parameter.h"
#include "transactional.h"
#include "wire.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QDebug>

// the constructor
Port::Port(QDomNode &portNode): 
nameGroup_(portNode), 
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(), 
portAccessType_() {

	for (int i = 0; i < portNode.childNodes().count(); ++i) {
		QDomNode tempNode = portNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:wire")) {
                        portType_ = General::WIRE;
			wire_ = QSharedPointer<Wire>(new Wire(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:transactional")) {
                        portType_ = General::TRANSACTIONAL;
			transactional_ = QSharedPointer<Transactional>(
					new Transactional(tempNode));
		}

		// get the portAccessHandle and portAccessType elements
		else if (tempNode.nodeName() == QString("spirit:access")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:portAccessHandle")) {
					portAccessHandle_ = tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
				}

				else if (tempNode.childNodes().at(j).nodeName() ==
						QString("spirit:portAccessType")) {
					portAccessType_ = tempNode.childNodes().at(j).childNodes().
							at(0).nodeValue();
				}
			}
		}
	}

	if (nameGroup_.name_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element name missing in "
				"spirit:port"));
	}

	if (!transactional_ && !wire_) {
		throw Parse_error(QObject::tr("No wire or transactional elements "
				"found in spirit:port"));
	}
	return;
}

//-----------------------------------------------------------------------------
// Function: Port()
//-----------------------------------------------------------------------------
Port::Port(QString const& name, Port const& other): 
nameGroup_(), portType_(other.portType_),
wire_(), 
transactional_(other.transactional_),
portAccessHandle_(other.portAccessHandle_),
portAccessType_(other.portAccessType_) {
	
	nameGroup_.name_ = name;

    if (other.wire_ != 0) {
        wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
    }
}

//-----------------------------------------------------------------------------
// Function: Port()
//-----------------------------------------------------------------------------
Port::Port(const Port &other): 
nameGroup_(other.nameGroup_), 
portType_(other.portType_),
wire_(),
transactional_(),
portAccessHandle_(other.portAccessHandle_),
portAccessType_(other.portAccessType_) {
	
	if (other.wire_) {
		wire_ = QSharedPointer<Wire>(new Wire(*other.wire_));
	}

	if (other.transactional_) {
		transactional_ = QSharedPointer<Transactional>(
			new Transactional(*other.transactional_.data()));
	}
}


Port & Port::operator=( const Port &other ) {
	if (this != &other) {
		nameGroup_ = other.nameGroup_;
		portType_ = other.portType_;
		portAccessHandle_ = other.portAccessHandle_;
		portAccessType_ = other.portAccessType_;

		if (other.wire_) {
			wire_ = QSharedPointer<Wire>(
				new Wire(*other.wire_.data()));
		}
		else
			wire_ = QSharedPointer<Wire>();

		if (other.transactional_) {
			transactional_ = QSharedPointer<Transactional>(
				new Transactional(*other.transactional_.data()));
		}
		else
			transactional_ = QSharedPointer<Transactional>();
	}
	return *this;
}


Port::Port():
nameGroup_(), 
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(),
portAccessType_()  {

	wire_ = QSharedPointer<Wire>(new Wire());
}

Port::Port( const QString& name, 
		   General::Direction direction, 
		   int leftBound, 
		   int rightBound, 
		   const QString& defaultValue, 
		   bool allLogicalDirections ):
nameGroup_(), 
portType_(General::WIRE),
wire_(), 
transactional_(),
portAccessHandle_(),
portAccessType_() {

	nameGroup_.name_ = name;

	wire_ = QSharedPointer<Wire>(new Wire(direction, leftBound, 
		rightBound, defaultValue, allLogicalDirections));
}

Port::Port( const QString& name,
		   General::Direction direction, 
		   int leftBound,
		   int rightBound,
		   const QString& typeName, 
		   const QString& typeDefinition,
		   const QString& defaultValue,
		   const QString& description ):
nameGroup_(),
portType_(General::WIRE),
wire_(),
transactional_(),
portAccessHandle_(),
portAccessType_() {

	nameGroup_.name_ = name;
	nameGroup_.description_ = description;

	wire_ = QSharedPointer<Wire>(new Wire(direction, leftBound, rightBound, defaultValue,
		false));
	wire_->setTypeName(typeName);
	wire_->setTypeDefinition(typeName, typeDefinition);
}

Port::~Port() {
}

void Port::write(QXmlStreamWriter& writer, const QStringList& viewNames) {
	writer.writeStartElement("spirit:port");

	// if mandatory name is missing
	if (nameGroup_.name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element name missing in "
				"spirit:port"));
	}
	else {
		writer.writeTextElement("spirit:name", nameGroup_.name_);
	}

	if (!nameGroup_.displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName_);
	}

	if (!nameGroup_.description_.isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description_);
	}

	// write the port type element (spirit:wire or spirit:transactional)
	switch (portType_) {
	case General::WIRE: {
		if (wire_) {
			wire_->write(writer, viewNames);
		}
		else {
			throw Write_error(QObject::tr("Port type defined as wire but not"
					" instantiated in spirit:port"));
		}
		break;
	}
	case General::TRANSACTIONAL: {
		if (transactional_) {
			transactional_->write(writer);
		}
		else {
			throw Write_error(QObject::tr("Port type defined as transactional"
					" but not instantiated in spirit:port"));
		}
		break;
	}
	default: {
		throw Write_error(QObject::tr("Invalid port type in spirit:port"));
		break;
	}
	}

	// if either of the optional elements is defined
	if (!portAccessHandle_.isEmpty() || !portAccessType_.isEmpty()) {
		writer.writeStartElement("spirit:access");

		if (!portAccessType_.isEmpty()) {
			writer.writeTextElement("spirit:portAccessType", portAccessType_);
		}

		if (!portAccessHandle_.isEmpty()) {
			writer.writeTextElement("spirit:portAccessHandle",
					portAccessHandle_);
		}

		writer.writeEndElement(); // spirit:access
	}

	writer.writeEndElement(); // spirit:port
}

void Port::setTransactional(Transactional *transactional) {
	// delete the old transactional if one exists
	if (wire_) {
		wire_.clear();
	}

	// delete the old wire if one exists
	if (transactional_) {
		transactional_.clear();
	}
	// change the port type
        portType_ = General::TRANSACTIONAL;

	transactional_ = QSharedPointer<Transactional>(transactional);
}

Transactional *Port::getTransactional() const {
	return transactional_.data();
}

void Port::setName(const QString &name) {
	nameGroup_.name_ = name;
}

QString Port::getName() const {
	return nameGroup_.name_;
}

QString Port::getDisplayName() const {
	return nameGroup_.displayName_;
}

void Port::setDisplayName( const QString& displayName ) {
	nameGroup_.displayName_ = displayName;
}

QString Port::getDescription() const {
	return nameGroup_.description_;
}

void Port::setDescription( const QString& description ) {
	nameGroup_.description_ = description;
}

General::PortType Port::getPortType() const {
	return portType_;
}

Wire *Port::getWire() const {
	return wire_.data();
}

void Port::setWire(Wire *wire) {
	// delete the old transactional if one exists
	if (wire_) {
		wire_.clear();
	}

	// delete the old wire if one exists
	if (transactional_) {
		transactional_.clear();
	}
	// change the port type
        portType_ = General::WIRE;

	wire_ = QSharedPointer<Wire>(wire);
}

QString Port::getPortAccessType() const {
	return portAccessType_;
}

void Port::setPortAccessType(const QString &portAccessType) {
	portAccessType_ = portAccessType;
}

int Port::getLeftBound() const {

	// if the port is not wire-type
	if (!wire_) {
		return 0;
	}

	// if the wire has a vector defined
	if (wire_->getVector()) {
		return wire_->getVector()->getLeft();
	}

	// no vector so bound is 0
	return 0;
}

int Port::getRightBound() const {

	// if the port is not wire-type
	if (!wire_) {
		return 0;
	}

	// if the wire has a vector defined
	if (wire_->getVector()) {
		return wire_->getVector()->getRight();
	}

	// no vector so bound is 0
	return 0;
}

int Port::getPortSize() const {
	return (getLeftBound() - getRightBound() +1);
}

QString Port::getDefaultValue() const {
	if (!wire_) {
		return QString();
	}
	return wire_->getDefaultDriverValue();
}

void Port::setDefaultValue( const QString& defaultValue ) {

	if (wire_)
		wire_->setDefaultDriverValue(defaultValue);

	// transactional does not have default value
	else if (transactional_)
		return;

	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setDefaultDriverValue(defaultValue);
		portType_ = General::WIRE;
	}
}

General::Direction Port::getDirection() const {
	if (wire_) {
		return wire_->getDirection();
	}
	return General::DIRECTION_INVALID;
}

bool Port::allLogicalDirectionsAllowed() const {
	
	if (wire_)
		return wire_->getAllLogicalDirectionsAllowed();

	return false;
}

bool Port::isValid() const {

	if (nameGroup_.name_.isEmpty())
		return false;

	// if port is type wire but the element is not defined.
	if (portType_ == General::WIRE && !wire_)
		return false;

	// if port is type transactional but element is not defined.
	if (portType_ == General::TRANSACTIONAL && !transactional_)
		return false;

	if (wire_)
		return wire_->isValid();

	return true;

}

void Port::setDirection( General::Direction direction ) {

	// if wire has been specified
	if (wire_)
		wire_->setDirection(direction);

	// if neither is specified then create a new wire element
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setDirection(direction);
	}
}

void Port::setLeftBound( int leftBound ) {

	if (wire_)
		wire_->setLeftBound(leftBound);

	// if neither exists then create wire 
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setLeftBound(leftBound);
		portType_ = General::WIRE;
	}
}

void Port::setRightBound( int rightBound ) {
	if (wire_)
		wire_->setRightBound(rightBound);

	// if neither exists then create wire 
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setRightBound(rightBound);
		portType_ = General::WIRE;
	}
}

void Port::setPortSize( int size ) {
	setLeftBound(size-1);
	setRightBound(0);
}

void Port::setAllLogicalDirectionsAllowed( bool allowed ) {

	if (wire_)
		wire_->setAllLogicalDirectionsAllowed(allowed);

	else if (transactional_)
		transactional_->setAllLogicalInitiativesAllowed(allowed);

	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		wire_->setAllLogicalDirectionsAllowed(allowed);
		portType_ = General::WIRE;
	}
}

QString Port::getTypeName( const QString& viewName /*= QString("")*/ ) const {
	if (wire_) {
		return wire_->getTypeName(viewName);
	}
	else {
		return QString("");
	}
}

void Port::setTypeName( const QString& typeName, const QString& viewName /*= QString("")*/ ) {
	if (wire_) {
		wire_->setTypeName(typeName, viewName);
	}
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setTypeName(typeName, viewName);
	}
}

QString Port::getTypeDefinition( const QString& typeName ) {
	if (wire_) {
		return wire_->getTypeDefinition(typeName);
	}
	else {
		return QString("");
	}
}

QStringList Port::getTypeDefinitions() const {
	if (wire_) {
		return wire_->getTypeDefinitions();
	}
	else {
		return QStringList();
	}
}

void Port::setTypeDefinition( const QString& typeName, const QString& typeDefinition ) {
	if (wire_) {
		wire_->setTypeDefinition(typeName, typeDefinition);
	}
	else {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
		wire_->setTypeDefinition(typeName, typeDefinition);
	}
}

bool Port::hasType( const QString& viewName /*= QString("")*/ ) const {
	if (wire_) {
		return wire_->hasType(viewName);
	}
	// if no wire then theres no type
	return false;
}

void Port::useDefaultVhdlTypes() {
	// if theres no wire definition
	if (!wire_) {
		wire_ = QSharedPointer<Wire>(new Wire());
		portType_ = General::WIRE;
	}

	int size = getPortSize();
	QString typeName;

	// use std_logic for scalar ports
	if (size <= 1) {
		typeName = QString("std_logic");
	}
	// use std_logic_vector for vectored ports
	else {
		typeName = QString("std_logic_vector");
	}

	wire_->setTypeName(typeName);

	// set the default type definition
	wire_->setTypeDefinition(typeName, QString("IEEE.std_logic_1164.all"));

}
