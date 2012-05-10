/* 
 *
 *  Created on: 16.7.2010
 *      Author: Antti Kamppi
 */

#include "component.h"
#include "librarycomponent.h"
#include "businterface.h"
#include "ComInterface.h"
#include "ApiInterface.h"
#include "parameter.h"
#include "addressspace.h"
#include "memorymap.h"
#include "remapstate.h"
#include "model.h"
#include "otherclockdriver.h"
#include "cpu.h"
#include "componentgenerator.h"
#include "fileset.h"
#include "channel.h"
#include "choice.h"
#include "file.h"

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QString>
#include <QDomNamedNodeMap>
#include <QList>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMap>
#include <QDir>
#include <QFileInfo>

#include <QDebug>

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(QDomDocument &doc)
    : LibraryComponent(doc),
      busInterfaces_(), 
      comInterfaces_(),
      apiInterfaces_(),
      channels_(), 
      remapStates_(), 
      addressSpaces_(),
      memoryMaps_(),
      model_(), 
      compGenerators_(), 
      choices_(), 
      fileSets_(),
      cpus_(),
      otherClockDrivers_(),
      parameters_(), 
      attributes_()
{
	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNodeList children = doc.childNodes().item(i).childNodes();

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	for (int i = 0; i < children.size(); ++i) {

		// dont parse comments
		if (children.at(i).isComment()) {
			continue;
		}

		// get busInterfaces
		if (children.at(i).nodeName() == QString("spirit:busInterfaces")) {
			// call constructors for all bus interfaces
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single choice as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!tempNode.isComment()) {

					// create the bus interface from the node.
					BusInterface* interface =
							new BusInterface(tempNode);

					// add the pointer to the map
					busInterfaces_.insert(interface->getName(),
							QSharedPointer<BusInterface>(interface));
				}
			}
		}

		// get channels
		else if (children.at(i).nodeName() == QString("spirit:channels")) {

			// go through all the channel elements
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode channelNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!channelNode.isComment()) {
					channels_.append(QSharedPointer<Channel>(
							new Channel(channelNode)));
				}
			}
		}

		// get remapStates
		else if (children.at(i).nodeName() == QString("spirit:remapStates")) {

			// go through all remap states
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode remapStateNode = children.at(i).childNodes().at(j);

				// dont parse comments
				if (!remapStateNode.isComment()) {
					remapStates_.append(QSharedPointer<RemapState>(
							new RemapState(remapStateNode)));
				}
			}
		}

		// get addressSpaces
		else if (children.at(i).nodeName() == QString("spirit:addressSpaces")) {

			// go through all the addressSpace elements
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode addrSpaceNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!addrSpaceNode.isComment()) {
					addressSpaces_.append(QSharedPointer<AddressSpace>(
							new AddressSpace(addrSpaceNode)));
				}
			}
		}

		// get memoryMaps
		else if (children.at(i).nodeName() == QString("spirit:memoryMaps")) {

			// go through all the memory maps
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode memoryMapNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!memoryMapNode.isComment()) {
					memoryMaps_.append(QSharedPointer<MemoryMap>(
							new MemoryMap(memoryMapNode)));
				}
			}
		}

		// get model
		else if (children.at(i).nodeName() == QString("spirit:model")) {
			QDomNode modelNode = children.at(i);
			model_ = QSharedPointer<Model>(new Model(modelNode));
		}

		// get componentGenerators
		else if (children.at(i).nodeName() ==
				QString("spirit:componentGenerators")) {

			// call constructor for all component generators
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single componentGenerator as parameter
				QDomNode generatorNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!generatorNode.isComment()) {
					compGenerators_.append(QSharedPointer<ComponentGenerator>(
							new ComponentGenerator(generatorNode)));
				}
			}
		}

		// get choices
		else if (children.at(i).nodeName() == QString("spirit:choices")) {

			// call constructors for all choices
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single choice as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!tempNode.isComment()) {
					choices_.append(QSharedPointer<Choice>(
							new Choice(tempNode)));
				}
			}
		}

		else if (children.at(i).nodeName() == QString("spirit:fileSets")) {

			// call constructors for all file sets
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				// call constructor and give the child node representing the
				// single file set as parameter
				QDomNode tempNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!tempNode.isComment()) {
					fileSets_.append(QSharedPointer<FileSet>(
							new FileSet(tempNode)));
				}
			}
		}

		// get cpus
		else if (children.at(i).nodeName() == QString("spirit:cpus")) {

			// go through all cpus
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode cpuNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!cpuNode.isComment()) {
					cpus_.append(QSharedPointer<Cpu>(
							new Cpu(cpuNode)));
				}
			}
		}

		// get otherClockDrivers
		else if (children.at(i).nodeName() ==
				QString("spirit:otherClockDrivers")) {

			// go through all otherClockDrivers
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				QDomNode clockNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!clockNode.isComment()) {
					otherClockDrivers_.append(QSharedPointer<OtherClockDriver>(
							new OtherClockDriver(clockNode)));
				}
			}
		}

		// get parameters
		else if (children.at(i).nodeName() == QString("spirit:parameters")) {
			// go through all parameters
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {

				QDomNode parameterNode = children.at(i).childNodes().at(j);

				// don't parse comments
				if (!parameterNode.isComment()) {
					parameters_.append(QSharedPointer<Parameter>(
							new Parameter(parameterNode)));
				}
			}
		}

		// get vendor extensions
		else if (children.at(i).nodeName() == QString("spirit:vendorExtensions"))
        {
			for (int j = 0; j < children.at(i).childNodes().count(); ++j) {
				
				QDomNode extensionNode = children.at(i).childNodes().at(j);

				// if node is for kactus2 extensions
				if (extensionNode.nodeName() == QString("kactus2:extensions")) {

					for (int k = 0; k < extensionNode.childNodes().count(); ++k) {
						
						QDomNode kactusExtension = extensionNode.childNodes().at(k);

						// if node is for kactus2 attributes
						if (kactusExtension.nodeName() == QString("kactus2:kts_attributes")) {
							parseKactus2Attributes(kactusExtension);
						}
                        else if (kactusExtension.nodeName() == "kactus2:comInterfaces")
                        {
                            parseComInterfaces(kactusExtension);
                        }
                        else if (kactusExtension.nodeName() == "kactus2:apiInterfaces")
                        {
                            parseApiInterfaces(kactusExtension);
                        }
					}
				}
			}
		}
	}

	if (!model_) {
		model_ = QSharedPointer<Model>(new Model());
	}

	return;
}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const VLNV &vlnv)
    : LibraryComponent(vlnv),
      busInterfaces_(),
      comInterfaces_(),
      apiInterfaces_(),
      channels_(),
      remapStates_(),
      addressSpaces_(),
      memoryMaps_(),
      model_(),
      compGenerators_(),
      choices_(),
      fileSets_(),
      cpus_(),
      otherClockDrivers_(),
      parameters_(), 
      attributes_()
{
	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);

	model_ = QSharedPointer<Model>(new Model());
}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component()
    : LibraryComponent(),
      busInterfaces_(), 
      comInterfaces_(),
      apiInterfaces_(),
      channels_(),
      remapStates_(),
      addressSpaces_(),
      memoryMaps_(),
      model_(),
      compGenerators_(), 
      choices_(),
      fileSets_(), 
      cpus_(), 
      otherClockDrivers_(), 
      parameters_(),
      attributes_()
{
	model_ = QSharedPointer<Model>(new Model());
}

//-----------------------------------------------------------------------------
// Function: Component::Component()
//-----------------------------------------------------------------------------
Component::Component(const Component &other)
    : LibraryComponent(other),
      busInterfaces_(),
      comInterfaces_(),
      apiInterfaces_(),
      channels_(),
      remapStates_(),
      addressSpaces_(),
      memoryMaps_(),
      model_(),
      compGenerators_(),
      choices_(),
      fileSets_(),
      cpus_(),
      otherClockDrivers_(),
      parameters_(),
      attributes_(other.attributes_)
{

	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i = other.busInterfaces_.begin();
		i != other.busInterfaces_.end(); ++i) {
			if (i.value()) {
				QSharedPointer<BusInterface> copy = QSharedPointer<BusInterface>(
					new BusInterface(*i.value().data()));
				busInterfaces_.insert(copy->getName(), copy);
			}
	}

    for (QMap<QString, QSharedPointer<ComInterface> >::const_iterator i = other.comInterfaces_.begin();
		 i != other.comInterfaces_.end(); ++i)
    {
			if (i.value())
            {
				QSharedPointer<ComInterface> copy =
                    QSharedPointer<ComInterface>(new ComInterface(*i.value().data()));

				comInterfaces_.insert(copy->getName(), copy);
			}
	}

    for (QMap<QString, QSharedPointer<ApiInterface> >::const_iterator i = other.apiInterfaces_.begin();
         i != other.apiInterfaces_.end(); ++i)
    {
        if (i.value())
        {
            QSharedPointer<ApiInterface> copy =
                QSharedPointer<ApiInterface>(new ApiInterface(*i.value().data()));

            apiInterfaces_.insert(copy->getName(), copy);
        }
    }

	foreach (QSharedPointer<Channel> channel, other.channels_) {
		if (channel) {
			QSharedPointer<Channel> copy = QSharedPointer<Channel>(
				new Channel(*channel.data()));
			channels_.append(copy);
		}
	}

	foreach (QSharedPointer<RemapState> remState, other.remapStates_) {
		if (remState) {
			QSharedPointer<RemapState> copy = QSharedPointer<RemapState>(
				new RemapState(*remState.data()));
			remapStates_.append(copy);
		}
	}

	foreach (QSharedPointer<AddressSpace> addrSpace, other.addressSpaces_) {
		if (addrSpace) {
			QSharedPointer<AddressSpace> copy = QSharedPointer<AddressSpace>(
				new AddressSpace(*addrSpace.data()));
			addressSpaces_.append(copy);
		}
	}

	foreach (QSharedPointer<MemoryMap> memMap, other.memoryMaps_) {
		if (memMap) {
			QSharedPointer<MemoryMap> copy = QSharedPointer<MemoryMap>(
				new MemoryMap(*memMap.data()));
			memoryMaps_.append(copy);
		}
	}

	if (other.model_) {
		model_ = QSharedPointer<Model>(new Model(*other.model_.data()));
	}
	else {
		model_ = QSharedPointer<Model>(new Model());
	}

	foreach (QSharedPointer<ComponentGenerator> compGen, other.compGenerators_) {
		if (compGen) {
			QSharedPointer<ComponentGenerator> copy = QSharedPointer<ComponentGenerator>(
				new ComponentGenerator(*compGen.data()));
			compGenerators_.append(copy);
		}
	}

	foreach (QSharedPointer<Choice> choice, other.choices_) {
		if (choice) {
			QSharedPointer<Choice> copy = QSharedPointer<Choice>(
				new Choice(*choice.data()));
			choices_.append(copy);
		}
	}

	foreach (QSharedPointer<FileSet> fileSet, other.fileSets_) {
		if (fileSet) {
			QSharedPointer<FileSet> copy = QSharedPointer<FileSet>(
				new FileSet(*fileSet.data()));
			fileSets_.append(copy);
		}
	}

	foreach (QSharedPointer<Cpu> cpu, other.cpus_) {
		if (cpu) {
			QSharedPointer<Cpu> copy = QSharedPointer<Cpu>(
				new Cpu(*cpu.data()));
			cpus_.append(copy);
		}
	}

	foreach (QSharedPointer<OtherClockDriver> othClock, other.otherClockDrivers_) {
		if (othClock) {
			QSharedPointer<OtherClockDriver> copy = QSharedPointer<OtherClockDriver>(
				new OtherClockDriver(*othClock.data()));
			otherClockDrivers_.append(copy);
		}
	}

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Component & Component::operator=( const Component &other ) {
	if (this != &other) {
		LibraryComponent::operator=(other);
        attributes_ = other.attributes_;

		busInterfaces_.clear();
		for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i = other.busInterfaces_.begin();
			i != other.busInterfaces_.end(); ++i) {
				if (i.value()) {
					QSharedPointer<BusInterface> copy = QSharedPointer<BusInterface>(
						new BusInterface(*i.value().data()));
					busInterfaces_.insert(copy->getName(), copy);
				}
		}

		channels_.clear();
		foreach (QSharedPointer<Channel> channel, other.channels_) {
			if (channel) {
				QSharedPointer<Channel> copy = QSharedPointer<Channel>(
					new Channel(*channel.data()));
				channels_.append(copy);
			}
		}

		remapStates_.clear();
		foreach (QSharedPointer<RemapState> remState, other.remapStates_) {
			if (remState) {
				QSharedPointer<RemapState> copy = QSharedPointer<RemapState>(
					new RemapState(*remState.data()));
				remapStates_.append(copy);
			}
		}

		addressSpaces_.clear();
		foreach (QSharedPointer<AddressSpace> addrSpace, other.addressSpaces_) {
			if (addrSpace) {
				QSharedPointer<AddressSpace> copy = QSharedPointer<AddressSpace>(
					new AddressSpace(*addrSpace.data()));
				addressSpaces_.append(copy);
			}
		}

		memoryMaps_.clear();
		foreach (QSharedPointer<MemoryMap> memMap, other.memoryMaps_) {
			if (memMap) {
				QSharedPointer<MemoryMap> copy = QSharedPointer<MemoryMap>(
					new MemoryMap(*memMap.data()));
				memoryMaps_.append(copy);
			}
		}

		if (other.model_) {
			model_ = QSharedPointer<Model>(new Model(*other.model_.data()));
		}
		else
			model_ = QSharedPointer<Model>(new Model());

		compGenerators_.clear();
		foreach (QSharedPointer<ComponentGenerator> compGen, other.compGenerators_) {
			if (compGen) {
				QSharedPointer<ComponentGenerator> copy = QSharedPointer<ComponentGenerator>(
					new ComponentGenerator(*compGen.data()));
				compGenerators_.append(copy);
			}
		}

		choices_.clear();
		foreach (QSharedPointer<Choice> choice, other.choices_) {
			if (choice) {
				QSharedPointer<Choice> copy = QSharedPointer<Choice>(
					new Choice(*choice.data()));
				choices_.append(copy);
			}
		}

		fileSets_.clear();
		foreach (QSharedPointer<FileSet> fileSet, other.fileSets_) {
			if (fileSet) {
				QSharedPointer<FileSet> copy = QSharedPointer<FileSet>(
					new FileSet(*fileSet.data()));
				fileSets_.append(copy);
			}
		}

		cpus_.clear();
		foreach (QSharedPointer<Cpu> cpu, other.cpus_) {
			if (cpu) {
				QSharedPointer<Cpu> copy = QSharedPointer<Cpu>(
					new Cpu(*cpu.data()));
				cpus_.append(copy);
			}
		}

		otherClockDrivers_.clear();
		foreach (QSharedPointer<OtherClockDriver> othClock, other.otherClockDrivers_) {
			if (othClock) {
				QSharedPointer<OtherClockDriver> copy = QSharedPointer<OtherClockDriver>(
					new OtherClockDriver(*othClock.data()));
				otherClockDrivers_.append(copy);
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
	}
	return *this;
}

// the destructor
Component::~Component() {
	busInterfaces_.clear();
	channels_.clear();
	remapStates_.clear();
	addressSpaces_.clear();
	memoryMaps_.clear();
	compGenerators_.clear();
	choices_.clear();
	fileSets_.clear();
	cpus_.clear();
	otherClockDrivers_.clear();
	parameters_.clear();
	model_.clear();
	return;
}

QSharedPointer<LibraryComponent> Component::clone() const {
	return QSharedPointer<LibraryComponent>(new Component(*this));
}

void Component::write(QFile& file) {
	// create a writer instance and set it to operate on the given file
	QXmlStreamWriter writer(&file);

	// writer automatically adds whitespaces to make document more readable
	writer.setAutoFormatting(true);
	// writer uses one tab as indentation
	writer.setAutoFormattingIndent(-1);

	// call the base class implementation to write the top comment and
	// vlvn info. It also starts the type element (i.e <spirit:busDefinition>)
	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:component element
	General::writeAttributes(writer, attributes_);

	// call base class to write the VLNV info
	LibraryComponent::writeVLNV(writer);

	// if description is specified in the base class
	if (!LibraryComponent::description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	if (busInterfaces_.size() != 0) {
		writer.writeStartElement("spirit:busInterfaces");

		// write each busInterface
		for (QMap<QString, QSharedPointer<BusInterface> >::iterator i =
				busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

			i.value()->write(writer);
		}

		writer.writeEndElement(); // spirit:busInterfaces
	}

	if (channels_.size() != 0) {
		writer.writeStartElement("spirit:channels");

		// write each channel
		for (int i = 0; i < channels_.size(); ++i) {
			channels_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:channels
	}

	if (remapStates_.size() != 0) {
		writer.writeStartElement("spirit:remapStates");

		// write each remapState
		for (int i = 0; i < remapStates_.size(); ++i) {
			remapStates_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:remapStates
	}

	if (addressSpaces_.size() != 0) {
		writer.writeStartElement("spirit:addressSpaces");

		// write each addressSpace
		for (int i = 0; i < addressSpaces_.size(); ++i) {
			addressSpaces_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:addressSpaces
	}

	if (memoryMaps_.size() != 0) {
		writer.writeStartElement("spirit:memoryMaps");

		// write each memoryMap
		for (int i = 0; i < memoryMaps_.size(); ++i) {
			// each memorymap root element must be written outside of
			// memoryMap::write() function
			writer.writeStartElement("spirit:memoryMap");
			memoryMaps_.at(i)->write(writer);
			writer.writeEndElement(); // spirit:memoryMap
		}

		writer.writeEndElement(); // spirit:memoryMaps
	}

	if (model_) {
		model_->write(writer);
	}

	if (compGenerators_.size() != 0) {
		writer.writeStartElement("spirit:componentGenerators");

		// write each component generator
		for (int i = 0; i < compGenerators_.size(); ++i) {
			compGenerators_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:componeneGenerators
	}

	if (choices_.size() != 0) {
		writer.writeStartElement("spirit:choices");

		// write each choice
		for (int i = 0; i < choices_.size(); ++i) {
			choices_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:choices
	}

	if (fileSets_.size() != 0) {
		writer.writeStartElement("spirit:fileSets");

		// write each file set
		for (int i = 0; i < fileSets_.size(); ++i) {
			fileSets_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:fileSets
	}

	if (cpus_.size() != 0) {
		writer.writeStartElement("spirit:cpus");

		// write each cpus
		for (int i = 0; i < cpus_.size(); ++i) {
			cpus_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:cpus
	}

	if (otherClockDrivers_.size() != 0) {
		writer.writeStartElement("spirit:otherClockDrivers");

		// write each otherClockDriver
		for (int i = 0; i < otherClockDrivers_.size(); ++i) {
			otherClockDrivers_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:otherClockDrivers
	}

	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		// write each parameter
		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}

		writer.writeEndElement(); // spirit:parameters
	}

	// if contains kactus2 attributes
	if (!kactus2Attributes_.isEmpty()) {
		writer.writeStartElement("spirit:vendorExtensions");
		writer.writeStartElement("kactus2:extensions");
		
		writeKactus2Attributes(writer);

        // Write COM interfaces if found.
        if (!comInterfaces_.empty())
        {
            writer.writeStartElement("kactus2:comInterfaces");

            foreach (QSharedPointer<ComInterface> comIf, comInterfaces_)
            {
                comIf->write(writer);
            }

            writer.writeEndElement(); // kactus2:comInterfaces
        }

        // Write API interfaces if found.
        if (!apiInterfaces_.empty())
        {
            writer.writeStartElement("kactus2:apiInterfaces");

            foreach (QSharedPointer<ApiInterface> apiIf, apiInterfaces_)
            {
                apiIf->write(writer);
            }

            writer.writeEndElement(); // kactus2:apiInterfaces
        }

		writer.writeEndElement(); // kactus2:extensions
		writer.writeEndElement(); // spirit:vendorExtensions
	}

	writer.writeEndElement(); // spirit:component
	writer.writeEndDocument();
	return;

}

bool Component::isValid( QStringList& errorList ) const {
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing component"));

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the component."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}
	else {
		thisIdentifier = QObject::tr("component %1").arg(vlnv_->toString());
	}

	// get the names of all file sets
	QStringList fileSetNames;
	foreach (QSharedPointer<FileSet> fileset, fileSets_) {

		if (fileSetNames.contains(fileset->getName())) {
			errorList.append(QObject::tr("%1 contains several file sets with"
				" name %2").arg(thisIdentifier).arg(fileset->getName()));
			valid = false;
		}
		else {
			fileSetNames.append(fileset->getName());
		}

		if (!fileset->isValid(errorList, thisIdentifier, true)) {
			valid = false;
		}
	}

	// the list to contains the bounds and names of the physical ports
	QList<General::PortBounds> physPorts;
	QStringList portNames;
	
	if (model_) {
		physPorts = model_->getPortBounds();
		portNames = model_->getPortNames();

		if (!model_->isValid(fileSetNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList busifNames;
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {
		
		if (busifNames.contains(busif->getName())) {
			errorList.append(QObject::tr("%1 contains several bus interfaces"
			" with name %2").arg(thisIdentifier).arg(busif->getName()));
			valid = false;
		}
		else {
			busifNames.append(busif->getName());
		}

		if (!busif->isValid(physPorts, errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList comIfNames;
    foreach (QSharedPointer<ComInterface> comIf, comInterfaces_) {

        if (comIfNames.contains(comIf->getName())) {
            errorList.append(QObject::tr("%1 contains several COM interfaces"
                " with name %2").arg(thisIdentifier).arg(comIf->getName()));
            valid = false;
        }
        else {
            comIfNames.append(comIf->getName());
        }

        if (!comIf->isValid(errorList, thisIdentifier)) {
            valid = false;
        }
    }

    QStringList apiIfNames;
    foreach (QSharedPointer<ApiInterface> apiIf, apiInterfaces_) {

        if (apiIfNames.contains(apiIf->getName())) {
            errorList.append(QObject::tr("%1 contains several API interfaces"
                " with name %2").arg(thisIdentifier).arg(apiIf->getName()));
            valid = false;
        }
        else {
            apiIfNames.append(apiIf->getName());
        }

        if (!apiIf->isValid(errorList, thisIdentifier)) {
            valid = false;
        }
    }

	QStringList channelNames;
	foreach (QSharedPointer<Channel> channel, channels_) {

		if (channelNames.contains(channel->getName())) {
			errorList.append(QObject::tr("%1 contains several channels with "
				"name %2").arg(thisIdentifier).arg(channel->getName()));
			valid = false;
		}

		if (!channel->isValid(busifNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList remapNames;
	foreach (QSharedPointer<RemapState> remState, remapStates_) {

		if (remapNames.contains(remState->getName())) {
			errorList.append(QObject::tr("%1 contains several remap states with"
				" name %2").arg(thisIdentifier).arg(remState->getName()));
			valid = false;
		}
		else {
			remapNames.append(remState->getName());
		}

		if (!remState->isValid(portNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList addSpaceNames;
	foreach (QSharedPointer<AddressSpace> addrSpace, addressSpaces_) {

		if (addSpaceNames.contains(addrSpace->getName())) {
			errorList.append(QObject::tr("%1 contains several address spaces"
				" with name %2").arg(thisIdentifier).arg(addrSpace->getName()));
			valid = false;
		}
		else {
			addSpaceNames.append(addrSpace->getName());
		}

		if (!addrSpace->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList memoryMapNames;
	foreach (QSharedPointer<MemoryMap> memMap, memoryMaps_) {

		if (memoryMapNames.contains(memMap->getName())) {
			errorList.append(QObject::tr("%1 contains several memory maps with"
				" name %2").arg(thisIdentifier).arg(memMap->getName()));
			valid = false;
		}
		else {
			memoryMapNames.append(memMap->getName());
		}

		if (!memMap->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList compGenNames;
	foreach (QSharedPointer<ComponentGenerator> compGen, compGenerators_) {

		if (compGenNames.contains(compGen->getName())) {
			errorList.append(QObject::tr("%1 contains several component generators"
				" with name %2").arg(thisIdentifier).arg(compGen->getName()));
			valid = false;
		}
		else {
			compGenNames.append(compGen->getName());
		}

		if (!compGen->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList choiceNames;
	foreach (QSharedPointer<Choice> choice, choices_) {

		if (choiceNames.contains(choice->getName())) {
			errorList.append(QObject::tr("%1 contains several choices with name %2").arg(
				thisIdentifier).arg(choice->getName()));
			valid = false;
		}
		else {
			choiceNames.append(choice->getName());
		}

		if (!choice->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList cpuNames;
	foreach (QSharedPointer<Cpu> cpu, cpus_) {

		if (cpuNames.contains(cpu->getName())) {
			errorList.append(QObject::tr("%1 contains several cpus with name %2").arg(
				thisIdentifier).arg(cpu->getName()));
			valid = false;
		}
		else {
			cpuNames.append(cpu->getName());
		}

		if (!cpu->isValid(addSpaceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList clockNames;
	foreach (QSharedPointer<OtherClockDriver> otherClock, otherClockDrivers_) {

		if (clockNames.contains(otherClock->getClockName())) {
			errorList.append(QObject::tr("%1 contains several other clock drivers"
				" with name %2").arg(thisIdentifier).arg(otherClock->getClockName()));
			valid = false;
		}
		else {
			clockNames.append(otherClock->getClockName());
		}

		if (!otherClock->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList paramNames;
	foreach (QSharedPointer<Parameter> param, parameters_) {

		if (paramNames.contains(param->getName())) {
			errorList.append(QObject::tr("%1 contains several parameters with"
				" name %2").arg(thisIdentifier).arg(param->getName()));
			valid = false;
		}
		else {
			paramNames.append(param->getName());
		}

		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool Component::isValid() const {
	if (!vlnv_) {
		return false;
	}
	else if (!vlnv_->isValid()) {
		return false;
	}

	// get the names of all file sets
	QStringList fileSetNames;
	foreach (QSharedPointer<FileSet> fileset, fileSets_) {

		if (fileSetNames.contains(fileset->getName())) {
			return false;
		}
		else {
			fileSetNames.append(fileset->getName());
		}

		if (!fileset->isValid(true)) {
			return false;
		}
	}

	// the list to contains the bounds and names of the physical ports
	QList<General::PortBounds> physPorts;
	QStringList portNames;

	if (model_) {
		physPorts = model_->getPortBounds();
		portNames = model_->getPortNames();

		if (!model_->isValid(fileSetNames)) {
			return false;
		}
	}

	QStringList busifNames;
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {

		if (busifNames.contains(busif->getName())) {
			return false;
		}
		else {
			busifNames.append(busif->getName());
		}

		if (!busif->isValid(physPorts)) {
			return false;
		}
	}

    QStringList comIfNames;
    foreach (QSharedPointer<ComInterface> comIf, comInterfaces_) {

        if (comIfNames.contains(comIf->getName())) {
            return false;
        }
        else {
            comIfNames.append(comIf->getName());
        }

        if (!comIf->isValid()) {
            return false;
        }
    }

    QStringList apiIfNames;
    foreach (QSharedPointer<ApiInterface> apiIf, apiInterfaces_) {

        if (apiIfNames.contains(apiIf->getName())) {
            return false;
        }
        else {
            apiIfNames.append(apiIf->getName());
        }

        if (!apiIf->isValid()) {
            return false;
        }
    }


	QStringList channelNames;
	foreach (QSharedPointer<Channel> channel, channels_) {

		if (channelNames.contains(channel->getName())) {
			return false;
		}

		if (!channel->isValid(busifNames)) {
			return false;
		}
	}

	QStringList remapNames;
	foreach (QSharedPointer<RemapState> remState, remapStates_) {

		if (remapNames.contains(remState->getName())) {
			return false;
		}
		else {
			remapNames.append(remState->getName());
		}

		if (!remState->isValid(portNames)) {
			return false;
		}
	}

	QStringList addSpaceNames;
	foreach (QSharedPointer<AddressSpace> addrSpace, addressSpaces_) {

		if (addSpaceNames.contains(addrSpace->getName())) {
			return false;
		}
		else {
			addSpaceNames.append(addrSpace->getName());
		}

		if (!addrSpace->isValid()) {
			return false;
		}
	}

	QStringList memoryMapNames;
	foreach (QSharedPointer<MemoryMap> memMap, memoryMaps_) {

		if (memoryMapNames.contains(memMap->getName())) {
			return false;
		}
		else {
			memoryMapNames.append(memMap->getName());
		}

		if (!memMap->isValid()) {
			return false;
		}
	}

	QStringList compGenNames;
	foreach (QSharedPointer<ComponentGenerator> compGen, compGenerators_) {

		if (compGenNames.contains(compGen->getName())) {
			return false;
		}
		else {
			compGenNames.append(compGen->getName());
		}

		if (!compGen->isValid()) {
			return false;
		}
	}

	QStringList choiceNames;
	foreach (QSharedPointer<Choice> choice, choices_) {

		if (choiceNames.contains(choice->getName())) {
			return false;
		}
		else {
			choiceNames.append(choice->getName());
		}

		if (!choice->isValid()) {
			return false;
		}
	}

	QStringList cpuNames;
	foreach (QSharedPointer<Cpu> cpu, cpus_) {

		if (cpuNames.contains(cpu->getName())) {
			return false;
		}
		else {
			cpuNames.append(cpu->getName());
		}

		if (!cpu->isValid(addSpaceNames)) {
			return false;
		}
	}

	QStringList clockNames;
	foreach (QSharedPointer<OtherClockDriver> otherClock, otherClockDrivers_) {

		if (clockNames.contains(otherClock->getClockName())) {
			return false;
		}
		else {
			clockNames.append(otherClock->getClockName());
		}

		if (!otherClock->isValid()) {
			return false;
		}
	}

	QStringList paramNames;
	foreach (QSharedPointer<Parameter> param, parameters_) {

		if (paramNames.contains(param->getName())) {
			return false;
		}
		else {
			paramNames.append(param->getName());
		}

		if (!param->isValid()) {
			return false;
		}
	}

	return true;
}

// get the attributes
const QMap<QString, QString>& Component::getAttributes() const {
	return attributes_;
}

// set the attributes
void Component::setAttributes(const QMap<QString, QString>& attributes) {
	attributes_ = attributes;
}

Model *Component::getModel() {
	
	// if model is defined for this component
	if (model_)
		return model_.data();
	
	// if model is not defined then create an empty model and return pointer to
	// it
	else {
		model_ = QSharedPointer<Model>(new Model());
		return model_.data();
	}
}

const QList<QSharedPointer<ComponentGenerator> >&
Component::getCompGenerators() const {
	return compGenerators_;
}

QList<QSharedPointer<Parameter> >& Component::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<MemoryMap> >& Component::getMemoryMaps() const {
	return memoryMaps_;
}

QList<QSharedPointer<Cpu> >& Component::getCpus() {
	return cpus_;
}

void Component::setOtherClockDrivers(const
QList<QSharedPointer<OtherClockDriver> > &otherClockDrivers) {
	// first remove old clock drivers and free the memory
	otherClockDrivers_.clear();

	// save new otherclockdrivers
	otherClockDrivers_ = otherClockDrivers;
}

void Component::setModel(Model *model) {
	// if an old model exists, delete it
	if (!model_) {
		model_.clear();
	}
	model_ = QSharedPointer<Model>(model);
}

const QList<QSharedPointer<FileSet> >& Component::getFileSets() const {
	return fileSets_;
}

QList<QSharedPointer<FileSet> >& Component::getFileSets() {
	return fileSets_;
}

void Component::setParameters(const QList<QSharedPointer<Parameter> >
&parameters) {
	// remove old parameters first
	parameters_.clear();
	parameters_ = parameters;
}

void Component::setCpus(const QList<QSharedPointer<Cpu> > &cpus) {
	// delete old cpus
	cpus_.clear();

	// save new cpus
	cpus_ = cpus;
}

void Component::setMemoryMaps(const QList<QSharedPointer<MemoryMap> >
&memoryMaps) {
	// remove old memory maps first
	memoryMaps_.clear();

	// save new memory maps
	memoryMaps_ = memoryMaps;
}

void Component::setChannels(const QList<QSharedPointer<Channel> > &channels) {
	// delete old channels
	channels_.clear();

	// save new channels
	channels_ = channels;
}

void Component::setChoices(const QList<QSharedPointer<Choice> > &choices) {
	// delete old choices
	choices_.clear();

	// save new choices
	choices_ = choices;
}

void Component::setBusInterfaces(const
QMap<QString, QSharedPointer<BusInterface> > &busInterfaces) {
	// delete old bus interfaces
	busInterfaces_.clear();

	// save new bus interfaces
	busInterfaces_ = busInterfaces;
}

void Component::setFileSets(const QList<QSharedPointer<FileSet> > &fileSets) {
	// delete old file sets
	fileSets_.clear();

	// save new file sets
	fileSets_ = fileSets;
}

const QList<QSharedPointer<Channel> >& Component::getChannels() const {
	return channels_;
}

void Component::setCompGenerators(const
QList<QSharedPointer<ComponentGenerator> > &compGenerators) {
	// delete old component generators
	compGenerators_.clear();

	// save new component generators
	compGenerators_ = compGenerators;
}

void Component::setAddressSpaces(const
QList<QSharedPointer<AddressSpace> > &addressSpaces) {
	// delete old address spaces
	addressSpaces_.clear();

	// save new address spaces
	addressSpaces_ = addressSpaces;
}

void Component::setRemapStates(const
QList<QSharedPointer<RemapState> > &remapStates) {
	// delete old remapstates
	remapStates_.clear();

	// save new remap states
	remapStates_ = remapStates;
}

const QMap<QString, QSharedPointer<BusInterface> >&
Component::getBusInterfaces() const {
	return busInterfaces_;
}

QList<QSharedPointer<AddressSpace> >& Component::getAddressSpaces() {
	return addressSpaces_;
}

const QList<QSharedPointer<AddressSpace> >& Component::getAddressSpaces() const {
	return addressSpaces_;
}

const QList<QSharedPointer<RemapState> >& Component::getRemapStates() const {
	return remapStates_;
}

const QList<QSharedPointer<Choice> >& Component::getChoices() const {
	return choices_;
}

QList<QSharedPointer<OtherClockDriver> >& Component::getOtherClockDrivers() {
	return otherClockDrivers_;
}

const QList<QSharedPointer<OtherClockDriver> >& Component::getOtherClockDrivers() const {
	return otherClockDrivers_;
}

const QList<VLNV> Component::getDependentVLNVs() const {
	QList<VLNV> vlnvList;

	// search all bus interfaces
	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i =
			busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

		// if abstraction type is defined
		if (i.value()->getAbstractionType().isValid()) {
			vlnvList.append(i.value()->getAbstractionType());
		}
		// if bus type is defined
		if (i.value()->getBusType().isValid()) {
			vlnvList.append(i.value()->getBusType());
		}
	}

	// ask model for all hierarchyrefs
	if (model_)
		vlnvList += model_->getHierarchyRefs();

	return vlnvList;
}

const QStringList Component::getDependentFiles() const {
	QStringList files;

	// find the file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		files += fileSets_.at(i)->getFilePaths();
	}

	// find the component generators
	for (int i = 0; i < compGenerators_.size(); ++i) {
		files += compGenerators_.at(i)->getGeneratorExe();
	}
	return files;
}

BusInterface* Component::getBusInterface(const QString& name) {

	// if bus interface is found
	if (busInterfaces_.contains(name)) {
		return busInterfaces_.value(name).data();
	}

	// if named bus interface couldn't be found
	return 0;
}

QList<General::LibraryFilePair> Component::getVhdlLibraries() const {
	QList<General::LibraryFilePair> libraries;

	// take libraries from all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		libraries += fileSets_.at(i)->getVhdlLibraries();
	}

	return libraries;
}

QStringList Component::getVhdlLibraries( const QString& viewName ) const {

	QStringList fileSetNames;

	// if view was specified
	if (!viewName.isEmpty()) {

		// if theres no views
		if (!model_) {
			return QStringList();
		}

		// get the names of the file sets that are used.
		fileSetNames = model_->getFileSetRefs(viewName);
	}
	// view was not specified so take all file sets
	else {
		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			fileSetNames.append(fileSet->getName());
		}
	}

	QStringList libraries;
	// each file set referenced by the view
	foreach (QString fileSetName, fileSetNames) {

		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			
			// if file set is the searched one
			if (fileSet->getName() == fileSetName) {
				libraries += fileSet->getVhdlLibraryNames();
			}
		}
	}
	libraries.removeDuplicates();
	return libraries;
}

QList<General::LibraryFilePair> Component::getLibraries( const QString& viewName ) const {

	QList<General::LibraryFilePair> libraries;
	// if no model then there are no views.
	if (!model_)
		return libraries;

	// ask the model for references to file sets.
	QStringList fileSets = model_->getFileSetRefs(viewName);
	foreach (QString fileSetName, fileSets) {

		// find the matching file set
		foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
			if (fileSet->getName() == fileSetName) {
				libraries += fileSet->getVerilogLibraries();
				libraries += fileSet->getVhdlLibraries();
			}
		}
	}
	return libraries;
}

QList<General::LibraryFilePair> Component::getVerilogLibraries() const {
	QList<General::LibraryFilePair> libraries;

	// take libraries from all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {
		libraries += fileSets_.at(i)->getVerilogLibraries();
	}

	return libraries;
}

bool Component::isCpu() const {
	if (cpus_.size() != 0) {
		return true;
	}
	return false;
}


bool Component::isBus() const {

	// if there is channels this is bus component
	if (channels_.size() != 0 ) {
		return true;
	}

	// if bridge is found in one of the bus interfaces this is bus component
	for (QMap<QString, QSharedPointer<BusInterface> >::const_iterator i =
			busInterfaces_.begin(); i != busInterfaces_.end(); ++i) {

		// if one of the bus interfaces has a bridge, this is bus component
		if (i.value()->hasBridge()) {
			return true;
		}
	}

	// if no channel or bridge was found
	return false;
}

bool Component::isChannel() const {
	return !channels_.isEmpty();
}

bool Component::isBridge() const {
	
	// search all bus interfaces.
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {

		// if one bridge is found
		if (busif->hasBridge())
			return true;
	}
	// not a single bridge was found
	return false;
}


bool Component::isHierarchical() const {

	// if no model element is defined then there is no hierarchical view
	if (!model_)
		return false;

	// if the model-element has atleast one hierachical view defined
	return model_->hasHierView();
}

Port* Component::getPort(const QString& name) const {
	if (model_)
		// ask model for the port
		return model_->getPort(name);

	return 0;
}

int Component::getPortWidth( const QString& port ) const {
	if (model_)
		return model_->getPortWidth(port);

	return -1;
}

bool Component::hasPort( const QString& name ) const {
	return model_->getPort(name);
}

FileSet* Component::getFileSet(const QString& name) const {
	// search all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {

		// if the file set was found
		if (fileSets_.at(i)->getName() == name) {
			return fileSets_.at(i).data();
		}
	}

	// file set was not found
	return 0;
}

void Component::addFileSet(FileSet* fileSet) {
	fileSets_.append(QSharedPointer<FileSet>(fileSet));
}

QStringList Component::getFiles(const QString fileSetName) {

	QStringList files;

	// search all file sets
	for (int i = 0; i < fileSets_.size(); ++i) {

		// if the file set has the specified name
		if (fileSets_.at(i)->getName() == fileSetName) {
			files += fileSets_.at(i)->getFileNames();
		}
	}
	return files;
}

QStringList Component::getFiles() const {

	QStringList files;

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		files += fileSet->getFileNames();
	}
	return files;
}

void Component::removeFileSet( const QString& fileSetName ) {
	for (int i = 0; i <fileSets_.size(); ++i) {
		
		// if the file set is the specified one
		if (fileSets_.at(i)->getName() == fileSetName) {
			fileSets_.value(i).clear();
			fileSets_.removeAt(i);
			return;
		}
	}
}

FileSet* Component::createFileSet() {
	// create an empty file set
	fileSets_.append(QSharedPointer<FileSet>(new FileSet()));
	// and return pointer to it
	return fileSets_.last().data();
}

bool Component::addPort( QSharedPointer<Port> port ) {
	
	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->addPort(port);
}

void Component::removePort( const QString& portName ) {
	
	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	model_->removePort(portName);
}

bool Component::renamePort( const QString& oldName, const QString& newName ) {

	// if no model has been specified for the component
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->renamePort(oldName, newName);
}

bool Component::addBusInterface(QSharedPointer<BusInterface> busInterface ) {

	if (busInterfaces_.contains(busInterface->getName()))
		return false;

	busInterfaces_.insert(busInterface->getName(), busInterface);
	return true;
}

void Component::removeBusInterface( const QString& busifName ) {

	busInterfaces_.remove(busifName);
}

void Component::removeBusInterface( const BusInterface* busInterface ) {
	// get all bus interface pointers
	QList<QSharedPointer<BusInterface> > list = busInterfaces_.values();
	// and check if any matches with given bus interface
	foreach (QSharedPointer<BusInterface> busif, list) {

		// if they match
		if (busif.data() == busInterface) {

			// remove the old item in the map
			busInterfaces_.remove(busif->getName());
			continue;
		}
	}
}

Channel* Component::createChannel() {

	QSharedPointer<Channel> chan = QSharedPointer<Channel>(new Channel());
	channels_.append(chan);
	return chan.data();
}

QStringList Component::getBusInterfaceNames() const {

	QStringList list;
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {
		list.append(busif->getName());
	}
	return list;
}

void Component::updateBusInterface(BusInterface* busInterface ) {

	// get all bus interface pointers
	QList<QSharedPointer<BusInterface> > list = busInterfaces_.values();
	// and check if any matches with given bus interface
	foreach (QSharedPointer<BusInterface> busif, list) {

		// if they match
		if (busif.data() == busInterface) {

			// remove the old item in the map
			busInterfaces_.take(busInterfaces_.key(busif)/*busif->getName()*/);

			// update the name
			busif->setName(busInterface->getName());

			// add new item to the map 
			busInterfaces_.insert(busif->getName(), busif);

			return;
		}
	}

	// if no match was found, add the bus interface to the map
	busInterfaces_.insert(busInterface->getName(), 
		QSharedPointer<BusInterface>(busInterface));
}

BusInterface* Component::createBusInterface() {

	BusInterface* busif = new BusInterface();

	// there might another bus interface with no name so must use insertMulti
	busInterfaces_.insertMulti(busif->getName(), QSharedPointer<BusInterface>(busif));
	return busif;
}

const QStringList Component::getAddressSpaceNames() const {

	// the list to store the names
	QStringList list;

	// get the names of all address spaces.
	foreach (QSharedPointer<AddressSpace> addrSpace, addressSpaces_) {
		list.append(addrSpace->getName());
	}
	return list;
}

QStringList Component::getPortNames() const {

	// if model exists then ask for it's ports
	if (model_)
		return model_->getPortNames();

	// if there is no model there is no ports either.
	else
		return QStringList();
}

QStringList Component::getFileSetNames() const {

	QStringList list;

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		list.append(fileSet->getName());
	}
	return list;
}

bool Component::hasFileSet( const QString& fileSetName ) const {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		
		// if the file set is found.
		if (fileSet->getName() == fileSetName)
			return true;
	}

	// all file sets were searched but none found
	return false;
}

QList<QSharedPointer<File> > Component::getRTLFiles() const {

	QList<QSharedPointer<File> > list;
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		list += fileSet->getRTLFiles();
	}
	return list;
}

QMap<QString, QSharedPointer<ModelParameter> >& Component::getModelParameters() {
	if (model_) {
		return model_->getModelParameters();
	}
	else {
		model_ = QSharedPointer<Model>(new Model());
		return model_->getModelParameters();
	}
}

General::Direction Component::getPortDirection( const QString& portName ) const {

	if (!model_)
		return General::DIRECTION_INVALID;

	return model_->getPortDirection(portName);
}

void Component::removeChannel( const QString& channelName ) {

	for (int i = 0; i < channels_.size(); ++i) {
		if (channels_.at(i)->getName() == channelName) {
			channels_.removeAt(i);
			return;
		}
	}
}

void Component::updateFiles( const Component& other,
							const QString& sourcePath, 
							const QString& targetPath ) {
				
	Q_ASSERT_X(!sourcePath.isEmpty(), "Component::updateFiles",
		"Empty source path given as parameter");
	Q_ASSERT_X(!targetPath.isEmpty(), "Component::updateFiles",
		"Empty targetPath given as parameter");

	// can't operate on the same component
	if (&other == this)
		return;

	// get the files from the other component
	QStringList files = other.getFiles();
	
	// take each file
	foreach (QString file, files) {
	
		// get the absolute path to the file
		QDir source(sourcePath);
		QString absoluteSource = source.absoluteFilePath(file);

		// if file is located under the source directory
		if (!file.contains(QString("../"))) {

			QDir target(targetPath);
			QString absoluteTarget = target.absoluteFilePath(file);
			
			QFileInfo targetInfo(absoluteTarget);

			target.mkpath(targetInfo.absolutePath());
			QFile::copy(absoluteSource, absoluteTarget);
			 
		}
		// if file is higher in directory hierarchy than the source directory
		else {
			// update the file name
			changeFileName(file, absoluteSource);
		}
	}
}

void Component::changeFileName( const QString& from, const QString& to ) {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {

		if (fileSet->contains(from)) {
			fileSet->changeFileName(from, to);
			return;
		}
	}
}

File* Component::getQuartusPinmap() const {

	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		File* file = fileSet->getQuartusPinMap();
		if (file)
			return file;
	}

	// no pin map was found
	return 0;
}

void Component::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::COMPONENT);
}

KactusAttribute::ProductHierarchy Component::getComponentHierarchy() const {

	KactusAttribute::ProductHierarchy hierarchy = KactusAttribute::KTS_GLOBAL;

	// if attribute is not found
	if (!kactus2Attributes_.contains(QString("kts_productHier")))
		return hierarchy;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_productHier")),
		hierarchy);

	return hierarchy;
}

KactusAttribute::Firmness Component::getComponentFirmness() const {
	KactusAttribute::Firmness firmness = KactusAttribute::KTS_MUTABLE;

	if (!kactus2Attributes_.contains(QString("kts_firmness")))
		return firmness;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_firmness")),
		firmness);
	return firmness;
}

KactusAttribute::Implementation Component::getComponentImplementation() const {
	KactusAttribute::Implementation implementation = KactusAttribute::KTS_HW;

	if (!kactus2Attributes_.contains(QString("kts_implementation")))
		return implementation;
	else
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_implementation")),
		implementation);
	return implementation;
}

//-----------------------------------------------------------------------------
// Function: getComponentSWType()
//-----------------------------------------------------------------------------
KactusAttribute::SWType Component::getComponentSWType() const
{
	KactusAttribute::SWType type = KactusAttribute::KTS_SW_MAPPING;

	if (kactus2Attributes_.contains(QString("kts_sw_type"))) {
		KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_sw_type")), type);
	}

	return type;
}

//-----------------------------------------------------------------------------
// Function: setComponentHierarchy()
//-----------------------------------------------------------------------------
void Component::setComponentHierarchy(KactusAttribute::ProductHierarchy prodHier) {
    kactus2Attributes_.insert("kts_productHier", KactusAttribute::valueToString(prodHier));
}

void Component::setComponentImplementation( KactusAttribute::Implementation implementation ) {
	kactus2Attributes_.insert("kts_implementation", KactusAttribute::valueToString(implementation));
}

//-----------------------------------------------------------------------------
// Function: setComponentFirmness()
//-----------------------------------------------------------------------------
void Component::setComponentFirmness(KactusAttribute::Firmness firmness) {
    kactus2Attributes_.insert("kts_firmness", KactusAttribute::valueToString(firmness));
}

//-----------------------------------------------------------------------------
// Function: setComponentSWType()
//-----------------------------------------------------------------------------
void Component::setComponentSWType(KactusAttribute::SWType type) {
    kactus2Attributes_.insert("kts_sw_type", KactusAttribute::valueToString(type));
	
	// also set component type as software
	kactus2Attributes_.insert("kts_implementation", "SW");
}

bool Component::hasModelParameters() const {
	if (!model_)
		return false;
	else
		return model_->hasModelParameters();
}

bool Component::hasParameters() const {
	return !parameters_.isEmpty();
}

bool Component::hasPorts() const {
	if (!model_)
		return false;
	else
		return model_->hasPorts();
}

bool Component::isPhysicalPort( const QString& portName ) const {
	if (!model_) {
		return false;
	}
	else {
		return model_->isPhysicalPort(portName);
	}
}

bool Component::hasInterfaces() const {
	return !busInterfaces_.isEmpty();
}

bool Component::hasInterface( const QString& interfaceName ) const {
	return busInterfaces_.contains(interfaceName);
}

bool Component::hasFileSets() const {
	return !fileSets_.isEmpty();
}

QMap<QString, QSharedPointer<Port> >& Component::getPorts() {
	if (model_) {
		return model_->getPorts();
	}
	else {
		model_ = QSharedPointer<Model>(new Model());
		return model_->getPorts();
	}
}

QMap<QString, QSharedPointer<Port> > Component::getPorts() const {
	if (model_) {
		return model_->getPorts();
	}
	else {
		return QMap<QString, QSharedPointer<Port> >();
	}
}

const QMap<QString, QSharedPointer<Port> > Component::getPorts( const QString& interfaceName ) const {

	QMap<QString, QSharedPointer<Port> > ports;
	
	// if interface is not found
	if (!busInterfaces_.contains(interfaceName)) {
		return ports;
	}

	// get list of ports in the interface
	QStringList portNames = busInterfaces_.value(interfaceName)->getPhysicalPortNames();

	// get all the ports on the component
	ports = getPorts();

	QMap<QString, QSharedPointer<Port> > portsToReturn;

	// check each port
	for (QMap<QString, QSharedPointer<Port> >::iterator i = ports.begin();
		i != ports.end(); ++i) {

			// if the port is on this interface
			if (portNames.contains(i.key())) {
				
				// copy the port to the list to be returned
				portsToReturn.insert(i.key(), i.value());
			}
	}
	return portsToReturn;
}

bool Component::hasFile( const QString& fileName ) const {
	foreach (QSharedPointer<FileSet> fileSet, fileSets_) {
		if (fileSet->contains(fileName)) {
			return true;
		}
	}
	return false;
}

int Component::getPortLeftBound( const QString& port ) const {
	if (!hasPort(port)) {
		return -1;
	}
	else {
		return getPort(port)->getLeftBound();
	}
}

int Component::getPortRightBound( const QString& port ) const {
	if (!hasPort(port)) {
		return -1;
	}
	else {
		return getPort(port)->getRightBound();
	}
}








VLNV Component::getHierRef(const QString viewName) const {

	// if no model is specified
	if (!model_)
		return VLNV();

	// ask the model for a hierarchyRef with a given name
	return model_->getHierRef(viewName);
}


QList<VLNV> Component::getHierRefs() const {
	if (!model_)
		return QList<VLNV>();

	return model_->getHierarchyRefs();
}


void Component::setHierRef(const VLNV& vlnv, const QString& viewName /*= QString()*/ ) {
	if (!model_)
		return;
	model_->setHierRef(vlnv, viewName);
}

View* Component::findView(const QString name) const {
	if (!model_) {
		return NULL;
	}
	return model_->findView(name);
}

void Component::addView(View* newView) {
	model_->addView(newView);
}

const QList<QSharedPointer<View> > Component::getViews() const {

	// if model exists then ask it for views
	if (model_)
		return model_->getViews();

	// if not then return empty list
	else
		return QList<QSharedPointer<View> >();
}

View* Component::createView() {

	// if model does not exist then create one
	if (!model_)
		model_ = QSharedPointer<Model>(new Model());

	return model_->createView();
}

QStringList Component::getViewNames() const {
	if (model_)
		return model_->getViewNames();
	else
		return QStringList();
}

int Component::viewCount() const {
	if (model_)
		return model_->viewCount();

	// if no model is defined then no views are either
	else
		return 0;
}

QStringList Component::getHierViews() const {

	if (model_)
		return model_->getHierViews();
	else
		return QStringList();
}

QString Component::getEntityName( const QString& viewName ) const {
	QString entityName;
	if (model_) {
		entityName = model_->getEntityName(viewName);

		// if view contained an entity name
		if (!entityName.isEmpty()) {
			return entityName;
		}
	
	}
	// if there are no views then the VLNV name is used
	entityName = vlnv_->getName();
	entityName = entityName.remove(QString(".comp"), Qt::CaseInsensitive);
	return entityName;
}

QString Component::getArchitectureName( const QString& viewName ) const {

	if (model_) {
		return model_->getArchitectureName(viewName);
	}
	// if there is no views
	return QString();
}

void Component::removeView( const QString& viewName ) {
	if (model_)
		model_->removeView(viewName);
}

bool Component::hasViews() const {
	if (!model_)
		return false;
	else
		return model_->hasViews();
}

bool Component::hasView( const QString& viewName ) const {
	if (!model_) {
		return false;
	}
	return model_->findView(viewName);
}

QString Component::getViewDescription( const QString& viewName ) const {
	View* view = findView(viewName);
	if (!view)
		return QString();

	return view->getDescription();
}

QMap<QString, QString> Component::getPortDefaultValues() const {
	if (model_) {
		return model_->getPortDefaultValues();
	}
	else {
		return QMap<QString, QString>();
	}
}

QStringList Component::getPortTypeDefinitions() const {
	if (model_) {
		return model_->getPortTypeDefinitions();
	}
	else {
		return QStringList();
	}
}

QMultiMap<QString, VLNV> Component::getInterfaceAbsDefForPort( const QString& physicalPortName ) const {
	QMultiMap<QString, VLNV> absDefList;
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {
		// if the interface contains the physical port
		if (busif->hasPhysicalPort(physicalPortName)) {
			
			QString logicalPort = busif->getLogicalPortName(physicalPortName);

			if (!logicalPort.isEmpty()) {
				absDefList.insert(logicalPort, busif->getAbstractionType());
			}
		}
	}
	return absDefList;
}

QString Component::getInterfaceNameForPort( const QString& portName ) const {
	bool found = false;
	QString interfaceName = "none";

	// search all ports
	foreach (QSharedPointer<BusInterface> busif, busInterfaces_) {
		
		// if the interface contains the port
		if (busif->hasPhysicalPort(portName)) {

			// if the port was found before then there are many interfaces for this port
			if (found) {
				interfaceName = "several";
			}
			// mark port as found and set the interface name
			else {
				found = true;
				interfaceName = busif->getName();
			}
		}
	}
	return interfaceName;
}

QString Component::getInterfaceDescription( const QString& interfaceName ) const {
	if (busInterfaces_.contains(interfaceName)) {
		return busInterfaces_.value(interfaceName)->getDescription();
	}
	else {
		return QString();
	}
}

void Component::createEmptyFlatView() {
	// create new view
	View* newView = new View();

	// depending on the hierarchy level select the name for the view
	KactusAttribute::ProductHierarchy hier = getComponentHierarchy();
	switch (hier) 
	{
	case KactusAttribute::KTS_IP:
	case KactusAttribute::KTS_SOC: {
		// set the name
		newView->setName("rtl");
		break;
								   }
	default: {
		newView->setName("flat");
		break;
			 }
	}

	newView->addEnvIdentifier(QString("::"));

	// if model has not yet been created
	if (!model_) {
		model_ = QSharedPointer<Model>(new Model());
	}
	model_->addView(newView);
}

void Component::createHierarchicalView( const VLNV& hierRef ) {
	// create new view
	View* newView = new View();

	// depending on the hierarchy level select the name for the view
	KactusAttribute::ProductHierarchy hier = getComponentHierarchy();
	switch (hier) 
	{
	case KactusAttribute::KTS_IP:
	case KactusAttribute::KTS_SOC: {
		// set the name
		newView->setName("structural");
		break;
								   }
	default: {
		newView->setName("hierarchical");
		break;
			 }
	}

	newView->addEnvIdentifier(QString("::"));
	newView->setHierarchyRef(hierRef);

	// if model has not yet been created
	if (!model_) {
		model_ = QSharedPointer<Model>(new Model());
	}

	model_->addView(newView);
}

bool Component::hasPortTypes() const {
	if (model_) {
		return model_->hasPortTypes();
	}
	// if there is no model there is no ports
	else {
		return false;
	}
}

QList<General::PortBounds> Component::getPortBounds() const {
	if (model_) {
		return model_->getPortBounds();
	}

	return QList<General::PortBounds>();
}

void Component::removeAddressSpace( const QString& addrSpaceName ) {
	for (int i = 0; i < addressSpaces_.size(); ++i) {
		
		// if the specified address space is found
		if (addressSpaces_.at(i)->getName() == addrSpaceName) {
			addressSpaces_.removeAt(i);
			return;
		}
	}
}

AddressSpace* Component::createAddressSpace() {
	QSharedPointer<AddressSpace> addrSpace(new AddressSpace());
	addressSpaces_.append(addrSpace);
	return addrSpace.data();
}

bool Component::hasAddressSpace( const QString& addrSpaceName ) const {
	foreach (QSharedPointer<AddressSpace> addrSpace, addressSpaces_) {
		if (addrSpace->getName() == addrSpaceName) {
			return true;
		}
	}
	return false;
}

void Component::removeCpu( const QString& cpuName ) {
	for (int i = 0; i < cpus_.size(); ++i) {
		if (cpus_.at(i)->getName() == cpuName) {
			cpus_.removeAt(i);
			return;
		}
	}
}

Cpu* Component::createCpu() {
	QSharedPointer<Cpu> cpu(new Cpu());
	cpus_.append(cpu);
	return cpu.data();
}

QList<QSharedPointer<MemoryMap> >* Component::getMemoryMapsPointer() {
	return &memoryMaps_;
}
//-----------------------------------------------------------------------------
// Function: Component::parseComInterfaces()
//-----------------------------------------------------------------------------
void Component::parseComInterfaces(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode comNode = node.childNodes().at(i);

        if (comNode.nodeName() == "kactus2:comInterface")
        {
            QSharedPointer<ComInterface> comIf(new ComInterface(comNode));
            comInterfaces_.insert(comIf->getName(), comIf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::parseApiInterfaces()
//-----------------------------------------------------------------------------
void Component::parseApiInterfaces(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode apiNode = node.childNodes().at(i);

        if (apiNode.nodeName() == "kactus2:apiInterface")
        {
            QSharedPointer<ApiInterface> apiIf(new ApiInterface(apiNode));
            apiInterfaces_.insert(apiIf->getName(), apiIf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterfaces()
//-----------------------------------------------------------------------------
QMap< QString, QSharedPointer<ComInterface> > const& Component::getComInterfaces() const
{
    return comInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterfaceNames()
//-----------------------------------------------------------------------------
QStringList Component::getComInterfaceNames() const
{
    return comInterfaces_.keys();
}

//-----------------------------------------------------------------------------
// Function: Component::getComInterface()
//-----------------------------------------------------------------------------
ComInterface* Component::getComInterface(QString const& name)
{
    if (comInterfaces_.contains(name))
    {
        return comInterfaces_.value(name).data();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: Component::createComInterface()
//-----------------------------------------------------------------------------
ComInterface* Component::createComInterface()
{
    ComInterface* comIf = new ComInterface();
    comInterfaces_.insertMulti(comIf->getName(), QSharedPointer<ComInterface>(comIf));
    return comIf;
}

//-----------------------------------------------------------------------------
// Function: Component::addComInterface()
//-----------------------------------------------------------------------------
bool Component::addComInterface(QSharedPointer<ComInterface> comInterface)
{
    if (comInterfaces_.contains(comInterface->getName()))
    {
        return false;
    }

    comInterfaces_.insert(comInterface->getName(), comInterface);
    return true;
}

//-----------------------------------------------------------------------------
// Function: Component::updateComInterface()
//-----------------------------------------------------------------------------
void Component::updateComInterface(ComInterface* comInterface)
{
    // get all com interface pointers
    QList<QSharedPointer<ComInterface> > list = comInterfaces_.values();
    // and check if any matches with given com interface
    foreach (QSharedPointer<ComInterface> comIf, list) {

        // if they match
        if (comIf.data() == comInterface) {

            // remove the old item in the map
            comInterfaces_.take(comInterfaces_.key(comIf)/*comIf->getName()*/);

            // update the name
            comIf->setName(comInterface->getName());

            // add new item to the map 
            comInterfaces_.insert(comIf->getName(), comIf);

            return;
        }
    }

    // if no match was found, add the com interface to the map
    comInterfaces_.insert(comInterface->getName(), 
        QSharedPointer<ComInterface>(comInterface));
}

//-----------------------------------------------------------------------------
// Function: Component::removeComInterface()
//-----------------------------------------------------------------------------
void Component::removeComInterface(QString const& name)
{
    comInterfaces_.remove(name);
}

//-----------------------------------------------------------------------------
// Function: Component::removeComInterface()
//-----------------------------------------------------------------------------
void Component::removeComInterface(ComInterface* comInterface)
{
    // Retrieve all COM interface pointers.
    QList<QSharedPointer<ComInterface> > list = comInterfaces_.values();

    // Check for a match with the one that is being removed.
    foreach (QSharedPointer<ComInterface> comIf, list)
    {
        if (comIf.data() == comInterface)
        {
            // remove the old item in the map
            comInterfaces_.remove(comIf->getName());
            continue;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterfaces()
//-----------------------------------------------------------------------------
QMap< QString, QSharedPointer<ApiInterface> > const& Component::getApiInterfaces() const
{
    return apiInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterfaceNames()
//-----------------------------------------------------------------------------
QStringList Component::getApiInterfaceNames() const
{
    return apiInterfaces_.keys();
}

//-----------------------------------------------------------------------------
// Function: Component::getApiInterface()
//-----------------------------------------------------------------------------
ApiInterface* Component::getApiInterface(QString const& name)
{
    if (apiInterfaces_.contains(name))
    {
        return apiInterfaces_.value(name).data();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: Component::createApiInterface()
//-----------------------------------------------------------------------------
ApiInterface* Component::createApiInterface()
{
    ApiInterface* apiIf = new ApiInterface();
    apiInterfaces_.insertMulti(apiIf->getName(), QSharedPointer<ApiInterface>(apiIf));
    return apiIf;
}

//-----------------------------------------------------------------------------
// Function: Component::addApiInterface()
//-----------------------------------------------------------------------------
bool Component::addApiInterface(QSharedPointer<ApiInterface> apiInterface)
{
    if (apiInterfaces_.contains(apiInterface->getName()))
    {
        return false;
    }

    apiInterfaces_.insert(apiInterface->getName(), apiInterface);
    return true;
}

//-----------------------------------------------------------------------------
// Function: Component::updateApiInteface()
//-----------------------------------------------------------------------------
void Component::updateApiInteface(ApiInterface* apiInterface)
{
    // get all api interface pointers
    QList<QSharedPointer<ApiInterface> > list = apiInterfaces_.values();
    // and check if any matches with given api interface
    foreach (QSharedPointer<ApiInterface> apiIf, list) {

        // if they match
        if (apiIf.data() == apiInterface) {

            // remove the old item in the map
            apiInterfaces_.take(apiInterfaces_.key(apiIf)/*apiIf->getName()*/);

            // update the name
            apiIf->setName(apiInterface->getName());

            // add new item to the map 
            apiInterfaces_.insert(apiIf->getName(), apiIf);

            return;
        }
    }

    // if no match was found, add the api interface to the map
    apiInterfaces_.insert(apiInterface->getName(), 
        QSharedPointer<ApiInterface>(apiInterface));
}

//-----------------------------------------------------------------------------
// Function: Component::removeApiInterface()
//-----------------------------------------------------------------------------
void Component::removeApiInterface(QString const& name)
{
    comInterfaces_.remove(name);
}

//-----------------------------------------------------------------------------
// Function: Component::removeApiInterface()
//-----------------------------------------------------------------------------
void Component::removeApiInterface(ApiInterface* apiInterface)
{
    // Retrieve all COM interface pointers.
    QList<QSharedPointer<ApiInterface> > list = apiInterfaces_.values();

    // Check for a match with the one that is being removed.
    foreach (QSharedPointer<ApiInterface> apiIf, list)
    {
        if (apiIf.data() == apiInterface)
        {
            // remove the old item in the map
            apiInterfaces_.remove(apiIf->getName());
            continue;
        }
    }
}
