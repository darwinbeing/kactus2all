/* 
 *
 *  Created on: 18.10.2010
 *      Author: Antti Kamppi
 */

#include "vector.h"
#include "XmlUtils.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QString>
#include <QMap>

Vector::Vector(QDomNode &vectorNode): left_(-1), leftAttributes_(),
right_(-1), rightAttributes_() {

	// go through all child nodes
	for (int i = 0; i < vectorNode.childNodes().count(); ++i) {
		QDomNode tempNode = vectorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:left")) {

			left_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes for the left-element
			leftAttributes_ = XmlUtils::parseAttributes(tempNode);
		}

		else if (tempNode.nodeName() == QString("spirit:right")) {

			right_ = tempNode.childNodes().at(0).nodeValue().toInt();

			// get the attributes for the right-element
			rightAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
	}

	// if left bound was not specified
// 	if (left_ < 0) {
// 		throw Parse_error(QString("Mandatory element spirit:left missing in"
// 				" spirit:vector"));
// 	}
// 
// 	// if right was not specified
// 	if (right_ < 0) {
// 		throw Parse_error(QString("Mandatory element spirit:right missing"
// 				" in spirit:vector"));
// 	}
	return;
}

//-----------------------------------------------------------------------------
// Function: Vector()
//-----------------------------------------------------------------------------
Vector::Vector(Vector const& other) : 
left_(other.left_), 
leftAttributes_(other.leftAttributes_),
right_(other.right_), 
rightAttributes_(other.rightAttributes_) {
}

Vector::Vector( int left, int right ): left_(left), leftAttributes_(),
right_(right), rightAttributes_() {

}

Vector::Vector(): left_(-1), leftAttributes_(),
right_(-1), rightAttributes_() {

}

Vector& Vector::operator=( const Vector& other ) {
	if (this != &other) {
		left_ = other.left_;
		leftAttributes_ = other.leftAttributes_;
		right_ = other.right_;
		rightAttributes_ = other.rightAttributes_;
	}
	return *this;
}

Vector::~Vector() {

}

void Vector::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:vector");

	
    // start the spirit:left tag
    writer.writeStartElement("spirit:left");

    // write the attributes for the element
    XmlUtils::writeAttributes(writer, leftAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(QString::number(left_));
    writer.writeEndElement(); // spirit:left

	// start the spirit:right tag
    writer.writeStartElement("spirit:right");

    // write the attributes for the element
    XmlUtils::writeAttributes(writer, rightAttributes_);

    // write the value of the element and close the tag
    writer.writeCharacters(QString::number(right_));
    writer.writeEndElement(); // spirit:right

	writer.writeEndElement(); // spirit:vector
}

bool Vector::isValid() const {

	if (left_ < 0 || right_ < 0)
		return false;

	return true;
}

bool Vector::isValid( QStringList& errorList, const QString& parentIdentifier ) const {

	bool valid = true;

	if (left_ < 0) {
		errorList.append(QObject::tr("Invalid left value set for vector within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (right_ < 0) {
		errorList.append(QObject::tr("Invalid right value set for vector within %1").arg(
			parentIdentifier));
		valid = false;
	}

	return valid;
}

int Vector::getLeft() const {
	return left_;
}

int Vector::getRight() const {
	return right_;
}

const QMap<QString, QString>& Vector::getLeftAttributes() {
	return leftAttributes_;
}

const QMap<QString, QString>& Vector::getRightAttributes() {
	return rightAttributes_;
}

void Vector::setLeft( int left ) {
	left_ = left;
}

void Vector::setRight( int right ) {
	right_ = right;
}

QString Vector::toString() const {
	if (left_ < 0 && right_ < 0)
		return QString();

	QString str("[");
	str += QString::number(left_);
	str += "..";
	str += QString::number(right_);
	str += "]";
	return str;
}

int Vector::getSize() const {
	if (left_ < 0 && right_ < 0)
		return 1;

	return left_ - right_ + 1;
}


