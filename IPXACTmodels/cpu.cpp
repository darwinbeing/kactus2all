/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "cpu.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include "GenericVendorExtension.h"

#include <IPXACTmodels/common/ParameterReader.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu():
NameGroup(),
addressSpaceRefs_(),
parameters_(),
vendorExtensions_()
{
}

Cpu::Cpu(QDomNode &cpuNode): 
NameGroup(), 
addressSpaceRefs_(),
parameters_(),
vendorExtensions_()
 {

	for (int i = 0; i < cpuNode.childNodes().count(); ++i) {
		QDomNode tempNode = cpuNode.childNodes().at(i);

		// don't try to parse comments
		if (tempNode.isComment()) {
			continue;
		}

		if (tempNode.nodeName() == QString("spirit:addressSpaceRef")) {

			// get the spirit:addressSpaceRef attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString str = attributeMap.namedItem(QString(
					"spirit:addressSpaceRef")).childNodes().at(0).nodeValue();

			// all was fine and attribute can be added
			addressSpaceRefs_.append(str);
		}
		else if (tempNode.nodeName() == QString("spirit:parameters"))
        {
            ParameterReader reader;
			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				QDomNode parameterNode = tempNode.childNodes().at(j);

				// dont parse comments
				if (!parameterNode.isComment()) {
					parameters_.append(QSharedPointer<Parameter>(reader.createParameterFrom(parameterNode)));
				}
			}
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);

                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

Cpu::Cpu( const Cpu &other ):
NameGroup(other),
addressSpaceRefs_(other.addressSpaceRefs_),
parameters_(),
vendorExtensions_(other.vendorExtensions_)
{
	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy(new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Cpu & Cpu::operator=( const Cpu &other ) {
	if (this != &other) {
		NameGroup::operator=(other);
		addressSpaceRefs_ = other.addressSpaceRefs_;
        vendorExtensions_ = other.vendorExtensions_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy(new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

Cpu::~Cpu() {
	parameters_.clear();
}

void Cpu::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:cpu");

	writer.writeTextElement("spirit:name", name());

	if (!displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName());
	}

	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

    for (int i = 0; i < addressSpaceRefs_.size(); ++i) {

        // the IP-Xact specification defines this to be empty element with
        // mandatory attribute of same name. Maybe this will change in
        // future.
        writer.writeEmptyElement("spirit:addressSpaceRef");
        writer.writeAttribute("spirit:addressSpaceRef",
            addressSpaceRefs_.at(i));
    }

    if (parameters_.size() != 0)
    {
        writer.writeStartElement("ipxact:parameters");

        ParameterWriter parameterWriter;
        // write each parameter
        for (int i = 0; i < parameters_.size(); ++i)
        {
            parameterWriter.writeParameter(writer, parameters_.at(i));
        }

        writer.writeEndElement(); // ipxact:parameters
    }

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:cpu
	return;
}

bool Cpu::isValid(const QStringList& addrSpaceNames,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
				  QStringList& errorList, 
				  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("cpu %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for a cpu within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		errorList.append(QObject::tr("No address space reference defined for"
			" cpu %1 within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}
	// if there are references then check that they are valid
	else {
		foreach (QString addrRef, addressSpaceRefs_) {
			if (!addrSpaceNames.contains(addrRef)) {
				errorList.append(QObject::tr("Cpu %1 contained reference to "
					"address space %2 which is not found within %3").arg(
					name()).arg(addrRef).arg(parentIdentifier));
				valid = false;
			}
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
// 	}

	return valid;
}

bool Cpu::isValid(const QStringList& addrSpaceNames, 
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const 
{
	if (name().isEmpty()) 
    {
		return false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		return false;
	}
	// if there are references then check that they are valid.
	else {
		foreach (QString addrRef, addressSpaceRefs_) {
			if (!addrSpaceNames.contains(addrRef)) {
				return false;
			}
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }

	return true;
}

QList<QSharedPointer<Parameter> >& Cpu::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& Cpu::getParameters() const {
	return parameters_;
}

void Cpu::setParameters(QList<QSharedPointer<Parameter> > &parameters) {
	// delete the old parameters
	parameters_.clear();

	// save the new parameteres
	parameters_ = parameters;
}

const QStringList& Cpu::getAddressSpaceRefs() {
	return addressSpaceRefs_;
}

void Cpu::setAddressSpaceRefs(const QStringList& addressSpaceRefs) {
	// remove old addressSpaceRefs
	addressSpaceRefs_.clear();
	addressSpaceRefs_ = addressSpaceRefs;
}
