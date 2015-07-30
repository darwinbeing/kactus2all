/* 
 *
 *  Created on: 25.10.2010
 *      Author: Antti Kamppi
 */

#include "field.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "enumeratedvalue.h"
#include "GenericVendorExtension.h"

#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QString>
#include <QXmlStreamWriter>
#include <QMap>
#include <QList>
#include <QDomNode>
#include <QSharedPointer>
#include <QDomNamedNodeMap>

#include <QDebug>
#include "XmlUtils.h"

Field::Field(QDomNode& fieldNode): 
id_(), 
nameGroup_(fieldNode),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(false),
access_(General::ACCESS_COUNT),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_(),
offsetExpression_(),
isPresentExpression_()
{

	// parse the spirit:id attribute
	QDomNamedNodeMap attributeMap = fieldNode.attributes();
	id_ = attributeMap.namedItem("spirit:id").nodeValue();
	id_ = XmlUtils::removeWhiteSpace(id_);

	// go through all nodes and parse them
	for (int i = 0; i < fieldNode.childNodes().count(); ++i) {
		QDomNode tempNode = fieldNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:bitOffset")) {
			bitOffset_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}
		else if (tempNode.nodeName() == QString("spirit:typeIdentifier")) {
			typeIdentifier_ = tempNode.childNodes().at(0).nodeValue();
			typeIdentifier_ = XmlUtils::removeWhiteSpace(typeIdentifier_);
		}
		else if (tempNode.nodeName() == QString("spirit:bitWidth")) {
			bitWidth_ = tempNode.childNodes().at(0).nodeValue().toInt();
			bitWidthAttributes_ = XmlUtils::parseAttributes(tempNode);
		}
		else if (tempNode.nodeName() == QString("spirit:enumeratedValues")) {

			// parse each enumerated value
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode enumeratedNode = tempNode.childNodes().at(j);

				enumeratedValues_.append(QSharedPointer<EnumeratedValue>(
						new EnumeratedValue(enumeratedNode)));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// parse each parameter
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode parameterNode = tempNode.childNodes().at(j);

				parameters_.append(QSharedPointer<Parameter>(
						new Parameter(parameterNode)));
			}
		}
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			volatile_ = General::str2Bool(tempNode.childNodes().at(0).nodeValue(), false);
		}
		else if (tempNode.nodeName() == QString("spirit:access")) {
			access_ = General::str2Access(tempNode.childNodes().at(0).nodeValue(), General::ACCESS_COUNT);
		}
		else if (tempNode.nodeName() == QString("spirit:modifiedWriteValue")) {
			modifiedWrite_ = General::str2ModifiedWrite(tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:writeValueConstraint")) {
			writeConstraint_ = QSharedPointer<WriteValueConstraint>(
				new WriteValueConstraint(tempNode));
		}
		else if (tempNode.nodeName() == QString("spirit:readAction")) {
			readAction_ = General::str2ReadAction(tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == QString("spirit:testable")) {
			testable_ = General::str2Bool(tempNode.childNodes().at(0).nodeValue(), true);
			QDomNamedNodeMap testAttributes = tempNode.attributes();
			QString constraint = testAttributes.namedItem("spirit:testConstraint").nodeValue();
			testConstraint_ = General::str2TestConstraint(constraint);
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            parseVendorExtensions(tempNode);
        }
	}
}

Field::Field():
id_(), 
nameGroup_(),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(false),
access_(General::ACCESS_COUNT),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_(),
offsetExpression_(),
isPresentExpression_()
{

}

Field::Field( General::BooleanValue volatileValue, General::Access access ):
id_(), 
nameGroup_(),
bitOffset_(0), 
typeIdentifier_(),
bitWidth_(0),
bitWidthAttributes_(),
enumeratedValues_(), 
parameters_(),
volatile_(General::BooleanValue2Bool(volatileValue, false)),
access_(access),
modifiedWrite_(General::MODIFIED_WRITE_COUNT),
readAction_(General::READ_ACTION_COUNT),
testable_(true),
testConstraint_(General::TEST_UNCONSTRAINED),
writeConstraint_(),
vendorExtensions_(),
offsetExpression_(),
isPresentExpression_()
{

}

Field::Field( const Field& other ):
id_(other.id_),
nameGroup_(other.nameGroup_),
bitOffset_(other.bitOffset_),
typeIdentifier_(other.typeIdentifier_),
bitWidth_(other.bitWidth_),
bitWidthAttributes_(other.bitWidthAttributes_),
enumeratedValues_(),
parameters_(),
volatile_(other.volatile_),
access_(other.access_),
modifiedWrite_(other.modifiedWrite_),
readAction_(other.readAction_),
testable_(other.testable_),
testConstraint_(other.testConstraint_),
writeConstraint_(),
vendorExtensions_(),
offsetExpression_(),
isPresentExpression_()
{

	foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
		if (enumValue) {
			QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
			new EnumeratedValue(*enumValue.data()));
			enumeratedValues_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}

	if (other.writeConstraint_) {
		writeConstraint_ = QSharedPointer<WriteValueConstraint>(new WriteValueConstraint(
			*other.writeConstraint_.data()));
	}

    copyVendorExtensions(other);
}

Field& Field::operator=( const Field& other ) {
	if (this != &other) {
		id_ = other.id_;
		nameGroup_ = other.nameGroup_;
		bitOffset_ = other.bitOffset_;
		typeIdentifier_ = other.typeIdentifier_;
		bitWidth_ = other.bitWidth_;
		bitWidthAttributes_ = other.bitWidthAttributes_;
		volatile_ = other.volatile_;
		access_ = other.access_;
		modifiedWrite_ = other.modifiedWrite_;
		readAction_ = other.readAction_;
		testable_ = other.testable_;
		testConstraint_ = other.testConstraint_;
        vendorExtensions_ = other.vendorExtensions_;

		enumeratedValues_.clear();
		foreach (QSharedPointer<EnumeratedValue> enumValue, other.enumeratedValues_) {
			if (enumValue) {
				QSharedPointer<EnumeratedValue> copy = QSharedPointer<EnumeratedValue>(
					new EnumeratedValue(*enumValue.data()));
				enumeratedValues_.append(copy);
			}
		}

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}

		writeConstraint_ = QSharedPointer<WriteValueConstraint>(
			new WriteValueConstraint(*other.writeConstraint_.data()));

        copyVendorExtensions(other);
	}
	return *this;
}

Field::~Field() {
	enumeratedValues_.clear();
	parameters_.clear();
	bitWidthAttributes_.clear();
	writeConstraint_.clear();
}

void Field::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:field");

	// if spirit:id attribute is defined
	if (!id_.isEmpty()) {
		writer.writeAttribute("spirit:id", id_);
	}

	writer.writeTextElement("spirit:name", nameGroup_.name());

	// if optional displayName is defined
	if (!nameGroup_.displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName());
	}

	// if optional description is defined
	if (!nameGroup_.description().isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description());
	}

	writer.writeTextElement("spirit:bitOffset", QString::number(bitOffset_));

	// if optional typeIdentifier is defined
	if (!typeIdentifier_.isEmpty()) {
		writer.writeTextElement("spirit:typeIdentifier", typeIdentifier_);
	}

    writer.writeStartElement("spirit:bitWidth");

    XmlUtils::writeAttributes(writer, bitWidthAttributes_);
    writer.writeCharacters(QString::number(bitWidth_));

    writer.writeEndElement(); // spirit:bitWidth

	writer.writeTextElement("spirit:volatile", General::bool2Str(volatile_));

	if (access_ != General::ACCESS_COUNT) {
		writer.writeTextElement("spirit:access", General::access2Str(access_));
	}

	if (modifiedWrite_ != General::MODIFIED_WRITE_COUNT) {
		writer.writeTextElement("spirit:modifiedWriteValue", General::modifiedWrite2Str(modifiedWrite_));
	}

	if (writeConstraint_) {
		writeConstraint_->write(writer);
	}

	if (readAction_ != General::READ_ACTION_COUNT) {
		writer.writeTextElement("spirit:readAction", General::readAction2Str(readAction_));
	}


	// start the spirit:testable tag
	writer.writeStartElement("spirit:testable");
	
	// if the testable is true then there might a constrain attached to it
	if (testable_) {
		writer.writeAttribute("spirit:testConstraint", General::testConstraint2Str(testConstraint_));
	}
	// write the value of the element and close the tag
	writer.writeCharacters(General::bool2Str(testable_));
	writer.writeEndElement(); // spirit:testable

	// if optional enumeratedValues exist
	if (enumeratedValues_.size() != 0) {
		writer.writeStartElement("spirit:enumeratedValues");

		for (int i = 0; i < enumeratedValues_.size(); ++i) {
			enumeratedValues_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:enumeratedValues
	}

	// if optional parameters exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:field
}

bool Field::isValid(unsigned int registerSize, QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	if (nameGroup_.name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for a field within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (bitOffset_ < 0) {
		errorList.append(QObject::tr("No bit offset set for field %1 within %2").arg(
			nameGroup_.name()).arg(parentIdentifier));
		valid = false;
	}

	if (bitWidth_ <= 0) {
		errorList.append(QObject::tr("No bit width set for field %1 within %2").arg(
			nameGroup_.name()).arg(parentIdentifier));
		valid = false;
	}

	if ((bitOffset_ + bitWidth_) > registerSize) {
		errorList.append(QObject::tr("The register contains %1 bits but field's MSB bit is %2").arg(
			registerSize).arg(bitOffset_ + bitWidth_ - 1));
		valid = false;
	}

	foreach (QSharedPointer<EnumeratedValue> enumValue, enumeratedValues_)
    {
		if (!enumValue->isValid(errorList, QObject::tr("field %1").arg(nameGroup_.name())))
        {
			valid = false;
		}
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        errorList.append(validator.findErrorsIn(param.data(), QObject::tr("field %1").arg(nameGroup_.name()),
            componentChoices));
        if (!validator.validate(param.data(), componentChoices)) 
        {
            valid = false;
        }
    }

	return valid;
}

bool Field::isValid(unsigned int registerSize, 
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const 
{
    if (nameGroup_.name().isEmpty()) 
    {
        return false;
    }

    if (bitOffset_ < 0)
    {
        return false;
    }

    if (bitWidth_ <= 0)
    {
        return false;
    }

    if ((bitOffset_ + bitWidth_) > registerSize)
    {
        return false;
    }

    foreach (QSharedPointer<EnumeratedValue> enumValue, enumeratedValues_) {
        if (!enumValue->isValid()) {
            return false;
        }
    }

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!validator.validate(param.data(), componentChoices)) 
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: field::getBitOffset()
//-----------------------------------------------------------------------------
int Field::getBitOffset() const
{
    return bitOffset_;
}

//-----------------------------------------------------------------------------
// Function: field::setBitOffsetExpression()
//-----------------------------------------------------------------------------
void Field::setBitOffsetExpression(QString const& expression)
{
    if (offsetExpression_.isNull())
    {
        createOffsetExpressionExtension(expression);
    }

    else
    {
        offsetExpression_->setValue(expression);
    }
}

//-----------------------------------------------------------------------------
// Function: field::getBitOffsetExpression()
//-----------------------------------------------------------------------------
QString Field::getBitOffsetExpression()
{
    if (hasBitOffsetExpression())
    {
        return offsetExpression_->value();
    }
    else
    {
        return QString::number(getBitOffset());
    }
}

//-----------------------------------------------------------------------------
// Function: field::hasBitOffsetExpression()
//-----------------------------------------------------------------------------
bool Field::hasBitOffsetExpression()
{
    if (offsetExpression_.isNull())
    {
        return false;
    }

    QString bitOffsetValue = offsetExpression_->value();
    if (bitOffsetValue.isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: field::removeBitOffsetExpression()
//-----------------------------------------------------------------------------
void Field::removeBitOffsetExpression()
{
    vendorExtensions_.removeAll(offsetExpression_);
    offsetExpression_.clear();
}

//-----------------------------------------------------------------------------
// Function: field::setIsPresentExpression()
//-----------------------------------------------------------------------------
void Field::setIsPresentExpression(QString const& expression)
{
    if (hasIsPresentExpression())
    {
        isPresentExpression_->setValue(expression);
    }
    else if (!expression.isEmpty())
    {
        createIsPresentExpressionExtension(expression);
    }
}

//-----------------------------------------------------------------------------
// Function: field::getIsPresentExpression()
//-----------------------------------------------------------------------------
QString Field::getIsPresentExpression()
{
    if (hasIsPresentExpression())
    {
        return isPresentExpression_->value();
    }

    return QString("1");
}

//-----------------------------------------------------------------------------
// Function: field::removeIsPresentExpression()
//-----------------------------------------------------------------------------
void Field::removeIsPresentExpression()
{
    vendorExtensions_.removeAll(isPresentExpression_);
    isPresentExpression_.clear();
}

//-----------------------------------------------------------------------------
// Function: field::hasIsPresentExpression()
//-----------------------------------------------------------------------------
bool Field::hasIsPresentExpression()
{
    return !isPresentExpression_.isNull();
}

//-----------------------------------------------------------------------------
// Function: field::getBitWidth()
//-----------------------------------------------------------------------------
unsigned int Field::getBitWidth() const
{
    return bitWidth_;
}

//-----------------------------------------------------------------------------
// Function: field::setBitWidthExpression()
//-----------------------------------------------------------------------------
void Field::setBitWidthExpression(QString const& expression)
{
    if (!expression.isEmpty())
    {
        bitWidthAttributes_.insert("expression", expression);
    }
}

//-----------------------------------------------------------------------------
// Function: field::getBitWidthExpression()
//-----------------------------------------------------------------------------
QString Field::getBitWidthExpression()
{
    if (hasBitWidthExpression())
    {
        return bitWidthAttributes_.value("expression");
    }
    else
    {
        return QString::number(getBitWidth());
    }
}

//-----------------------------------------------------------------------------
// Function: field::removeBitWidthExpression()
//-----------------------------------------------------------------------------
void Field::removeBitWidthExpression()
{
    bitWidthAttributes_.remove("expression");
}

//-----------------------------------------------------------------------------
// Function: field::hasBitWidthExpression()
//-----------------------------------------------------------------------------
bool Field::hasBitWidthExpression()
{
    return bitWidthAttributes_.contains("expression");
}

const QMap<QString,QString>& Field::getBitWidthAttributes() const {
    return bitWidthAttributes_;
}

QString Field::getDescription() const {
    return nameGroup_.description();
}

QString Field::getDisplayName() const {
    return nameGroup_.displayName();
}

const QList<QSharedPointer<EnumeratedValue> >&
Field::getEnumeratedValues() const {
    return enumeratedValues_;
}

QList<QSharedPointer<EnumeratedValue> >& Field::getEnumeratedValues() {
	return enumeratedValues_;
}

QString Field::getName() const {
    return nameGroup_.name();
}

const QList<QSharedPointer<Parameter> >& Field::getParameters() const {
    return parameters_;
}

QString Field::getTypeIdentifier() const {
    return typeIdentifier_;
}

void Field::setBitOffset(int bitOffset) {
    bitOffset_ = bitOffset;
}

void Field::setBitWidth(unsigned int bitWidth) {
    bitWidth_ = bitWidth;
}

void Field::setBitWidthAttributes(
		const QMap<QString,QString>& bitWidthAttributes) {
    bitWidthAttributes_ = bitWidthAttributes;
}

void Field::setDescription(const QString& description) {
    nameGroup_.setDescription(description);
}

void Field::setDisplayName(const QString& displayName) {
    nameGroup_.setDisplayName(displayName);
}

void Field::setEnumeratedValues(
		const QList<QSharedPointer<EnumeratedValue> >& enumeratedValues) {
    enumeratedValues_ = enumeratedValues;
}

void Field::setName(const QString& name) {
    nameGroup_.setName(name);
}

void Field::setParameters(const QList<QSharedPointer<Parameter> >& parameters) {
	parameters_.clear();
    parameters_ = parameters;
}

void Field::setTypeIdentifier(const QString& typeIdentifier) {
    typeIdentifier_ = typeIdentifier;
}

QString Field::getId() const {
    return id_;
}

void Field::setId(const QString& id) {
    id_ = id;
}

bool Field::getVolatile() const {
	return volatile_;
}

void Field::setVolatile( bool volatileValue ) {
	volatile_ = volatileValue;
}

General::Access Field::getAccess() const {
	return access_;
}

void Field::setAccess( General::Access access ) {
	access_ = access;
}

General::ModifiedWrite Field::getModifiedWrite() const {
	return modifiedWrite_;
}

void Field::setModifiedWrite( const General::ModifiedWrite modWriteValue ) {
	modifiedWrite_ = modWriteValue;
}

General::ReadAction Field::getReadAction() const {
	return readAction_;
}

void Field::setReadAction( const General::ReadAction readAction ) {
	readAction_ = readAction;
}

bool Field::getTestable() const {
	return testable_;
}

void Field::setTestable( bool testable ) {
	testable_ = testable;
}

General::TestConstraint Field::getTestConstraint() const {
	return testConstraint_;
}

void Field::setTestConstraint( const General::TestConstraint testContraint ) {
	testConstraint_ = testContraint;
}

const QSharedPointer<WriteValueConstraint> Field::getWriteConstraint() const {
	return writeConstraint_;
}

QSharedPointer<WriteValueConstraint> Field::getWriteConstraint() {
	if (!writeConstraint_) {
		writeConstraint_ = QSharedPointer<WriteValueConstraint>(new WriteValueConstraint());
	}
	return writeConstraint_;
}

//-----------------------------------------------------------------------------
// Function: field::getNameGroup()
//-----------------------------------------------------------------------------
NameGroup& Field::getNameGroup()
{
    return nameGroup_;
}

//-----------------------------------------------------------------------------
// Function: field::setResetValue()
//-----------------------------------------------------------------------------
void Field::setResetValue(QString const& newResetValue)
{
    QSharedPointer<VendorExtension> resetValueExtension = getVendorExtension("kactus2:resetValue");

    if (newResetValue.isEmpty())
    {
        vendorExtensions_.removeAll(resetValueExtension);
    }
    else if (resetValueExtension)
    {
        QSharedPointer<Kactus2Value> resetValue = resetValueExtension.dynamicCast<Kactus2Value>();
        resetValue->setValue(newResetValue);
    }
    else
    {
        createResetValueExtension(newResetValue);
    }
}

//-----------------------------------------------------------------------------
// Function: field::getResetValue()
//-----------------------------------------------------------------------------
QString Field::getResetValue() const
{
    QSharedPointer<VendorExtension> resetValueExtension = getVendorExtension("kactus2:resetValue");

    if (resetValueExtension)
    {
        QSharedPointer<Kactus2Value> resetValue = resetValueExtension.dynamicCast<Kactus2Value>();
        return resetValue->value();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: field::setResetMask()
//-----------------------------------------------------------------------------
void Field::setResetMask(QString const& newResetMask)
{
    QSharedPointer<VendorExtension> resetMaskExtension = getVendorExtension("kactus2:resetMask");
    
    if (newResetMask.isEmpty())
    {
        vendorExtensions_.removeAll(resetMaskExtension);
    }

    else if (resetMaskExtension)
    {
        QSharedPointer<Kactus2Value> resetMask = resetMaskExtension.dynamicCast<Kactus2Value>();
        resetMask->setValue(newResetMask);
    }
    else
    {
        createResetMaskExtension(newResetMask);
    }
}

//-----------------------------------------------------------------------------
// Function: field::getResetMask()
//-----------------------------------------------------------------------------
QString Field::getResetMask() const
{
    QSharedPointer<VendorExtension> resetMaskExtension = getVendorExtension("kactus2:resetMask");

    if (resetMaskExtension)
    {
        QSharedPointer<Kactus2Value> resetMask = resetMaskExtension.dynamicCast<Kactus2Value>();
        return resetMask->value();
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: field::getVendorExtension()
//-----------------------------------------------------------------------------
QSharedPointer<VendorExtension> Field::getVendorExtension(QString const& extensionType) const
{
    foreach (QSharedPointer<VendorExtension> kactus2Extension, vendorExtensions_)
    {
        if (kactus2Extension->type() == extensionType)
        {
            return kactus2Extension;
        }
    }

    return QSharedPointer<VendorExtension>();
}

//-----------------------------------------------------------------------------
// Function: field::parseVendorExtensions()
//-----------------------------------------------------------------------------
void Field::parseVendorExtensions(QDomNode const& fieldNode)
{
    int extensionCount = fieldNode.childNodes().count();
    for (int i = 0; i < extensionCount; ++i)
    {
        QDomNode extensionNode = fieldNode.childNodes().at(i);

        if (extensionNode.nodeName() == QString("kactus2:offsetExpression"))
        {
            createOffsetExpressionExtension(extensionNode.childNodes().at(i).nodeValue());
        }
        else if (extensionNode.nodeName() == QString("kactus2:isPresent"))
        {
            createIsPresentExpressionExtension(extensionNode.childNodes().at(i).nodeValue());
        }
        else if (extensionNode.nodeName() == QString("kactus2:resetValue"))
        {
            createResetValueExtension(extensionNode.childNodes().at(0).nodeValue());
        }
        else if (extensionNode.nodeName() == QString("kactus2:resetMask"))
        {
            createResetMaskExtension(extensionNode.childNodes().at(0).nodeValue());
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: field::createOffsetExpressionExtension()
//-----------------------------------------------------------------------------
void Field::createOffsetExpressionExtension(QString const& expression)
{
    if (offsetExpression_.isNull())
    {
        offsetExpression_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:offsetExpression", expression));
        vendorExtensions_.append(offsetExpression_);
    }
}

//-----------------------------------------------------------------------------
// Function: field::createIsPresentExpressionExtension()
//-----------------------------------------------------------------------------
void Field::createIsPresentExpressionExtension(QString const& expression)
{
    if (isPresentExpression_.isNull())
    {
        isPresentExpression_ = QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:isPresent", expression));
        vendorExtensions_.append(isPresentExpression_);
    }
}

//-----------------------------------------------------------------------------
// Function: field::createResetValueExtension()
//-----------------------------------------------------------------------------
void Field::createResetValueExtension(QString const& newResetValue)
{
    QSharedPointer<VendorExtension> resetValueExtension (new Kactus2Value("kactus2:resetValue", newResetValue));
    vendorExtensions_.append(resetValueExtension);
}

//-----------------------------------------------------------------------------
// Function: field::createResetMaskExtension()
//-----------------------------------------------------------------------------
void Field::createResetMaskExtension(QString const& newResetMask)
{
    QSharedPointer<VendorExtension> resetMaskExtension (new Kactus2Value("kactus2:resetMask", newResetMask));
    vendorExtensions_.append(resetMaskExtension);
}

//-----------------------------------------------------------------------------
// Function: field::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Field::copyVendorExtensions(const Field & other)
{
    foreach (QSharedPointer<VendorExtension> extension, other.vendorExtensions_)
    {
        if (extension->type() == "kactus2:offsetExpression")
        {
            offsetExpression_ = QSharedPointer<Kactus2Value>(other.offsetExpression_->clone());
            vendorExtensions_.append(offsetExpression_);
        }
        else if (extension->type() == "kactus2:isPresent")
        {
            isPresentExpression_ = QSharedPointer<Kactus2Value>(other.isPresentExpression_->clone());
            vendorExtensions_.append(isPresentExpression_);
        }
        else
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
        }
    }
}

