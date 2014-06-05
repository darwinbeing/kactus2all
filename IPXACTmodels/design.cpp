//-----------------------------------------------------------------------------
// File: design.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date: 
//
// Description:
// Describes the spirit:design element in an IP-XACT document
//-----------------------------------------------------------------------------

#include "design.h"

#include "AdHocConnection.h"
#include "XmlUtils.h"

#include <common/validators/vhdlNameValidator/vhdlnamevalidator.h>
#include <common/KactusAttribute.h>
#include <library/LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Position.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>


//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(QDomDocument& doc)
    : LibraryComponent(doc), 
      componentInstances_(),
      swInstances_(),
      interconnections_(), 
      hierConnections_(),
      adHocConnections_(),
      portAdHocVisibilities_(),
      attributes_(),
      apiDependencies_(),
      hierApiDependencies_(),
      comConnections_(),
      hierComConnections_()
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);

    QDomNodeList nodes = doc.childNodes();

    int i;
	for (i = 0; i < nodes.size(); i++) {

		// if the node is for a header comment
		if (nodes.at(i).isComment()) {
			topComments_.append(nodes.at(i).nodeValue());
		}
		else if (nodes.at(i).nodeName() == "spirit:design") {
			break;
		}
	}
    nodes = nodes.at(i).childNodes();

    for (int i = 0; i < nodes.size(); i++) {
        QDomNode node = nodes.at(i);

        if (node.nodeName() == "spirit:componentInstances")
        {
            QDomNodeList instanceNodes = node.childNodes();
            for (int i = 0; i < instanceNodes.size(); i++)
            {
                QDomNode instanceNode = instanceNodes.at(i);
                componentInstances_.append(ComponentInstance(instanceNode));
            }
        }
        else if (node.nodeName() == "spirit:interconnections")
        {
            QDomNodeList interconnectionNodes = node.childNodes();

            for (int i = 0; i < interconnectionNodes.size(); i++)
            {
                QDomNode interconnectionNode = interconnectionNodes.at(i);

                if (interconnectionNode.nodeName() == "spirit:interconnection")
                {
                    interconnections_.append(Interconnection(interconnectionNode));
                }
                else
                {
                    // TODO spirit::monitorInterconnection
                }
            }
        }
        else if (node.nodeName() == "spirit:adHocConnections")
        {
            QDomNodeList adHocNodes = node.childNodes();

            for (int i = 0; i < adHocNodes.size(); i++)
            {
                QDomNode adHocNode = adHocNodes.at(i);
                adHocConnections_.append(AdHocConnection(adHocNode));
            }
        }
        else if (node.nodeName() == "spirit:hierConnections")
        {
            QDomNodeList hierConnectionNodes = node.childNodes();

            for (int i = 0; i < hierConnectionNodes.size(); i++)
            {
                QDomNode hierConnectionNode = hierConnectionNodes.at(i);
                hierConnections_.append(HierConnection(hierConnectionNode));
            }
        }
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            parseVendorExtensions(node);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(const VLNV &vlnv)
    : LibraryComponent(vlnv), 
      componentInstances_(),
      swInstances_(),
      interconnections_(), 
      hierConnections_(),
      adHocConnections_(),
      portAdHocVisibilities_(),
      attributes_(),
      apiDependencies_(),
      hierApiDependencies_(),
      comConnections_(),
      hierComConnections_()
{
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);
}

//-----------------------------------------------------------------------------
// Function: Design::Design()
//-----------------------------------------------------------------------------
Design::Design(Design const& other)
    : LibraryComponent(other),
      columns_(other.columns_),
      componentInstances_(other.componentInstances_),
      swInstances_(other.swInstances_),
      interconnections_(other.interconnections_),
      hierConnections_(other.hierConnections_),
      adHocConnections_(other.adHocConnections_),
      portAdHocVisibilities_(other.portAdHocVisibilities_),
      adHocPortPositions_(other.adHocPortPositions_),
      attributes_(other.attributes_),
      apiDependencies_(other.apiDependencies_),
      hierApiDependencies_(other.hierApiDependencies_),
      comConnections_(other.comConnections_),
      hierComConnections_(other.hierComConnections_)
{
}

//-----------------------------------------------------------------------------
// Function: Design::operator=()
//-----------------------------------------------------------------------------
Design& Design::operator=( const Design& other )
{
	if (this != &other) {
		LibraryComponent::operator=(other);
		
		columns_ = other.columns_;
		componentInstances_ = other.componentInstances_;
        swInstances_ = other.swInstances_;
		interconnections_ = other.interconnections_;
		hierConnections_ = other.hierConnections_;
		adHocConnections_ = other.adHocConnections_;
        portAdHocVisibilities_ = other.portAdHocVisibilities_;
        adHocPortPositions_ = other.adHocPortPositions_;
		attributes_ = other.attributes_;
        apiDependencies_ = other.apiDependencies_;
        hierApiDependencies_ = other.hierApiDependencies_;
        comConnections_ = other.comConnections_;
        hierComConnections_ = other.hierComConnections_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Design::~Design()
//-----------------------------------------------------------------------------
Design::~Design()
{
}

//-----------------------------------------------------------------------------
// Function: Design::close()
//-----------------------------------------------------------------------------
QSharedPointer<LibraryComponent> Design::clone() const {
	return QSharedPointer<LibraryComponent>(new Design(*this));
}

//-----------------------------------------------------------------------------
// Function: Design::write()
//-----------------------------------------------------------------------------
void Design::write(QFile& file)
{
	QXmlStreamWriter writer(&file);

	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);

	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:designConfiguration element
	General::writeAttributes(writer, attributes_);

	LibraryComponent::writeVLNV(writer);

	if (!componentInstances_.isEmpty()) {
		writer.writeStartElement("spirit:componentInstances");

		foreach (ComponentInstance const& inst, componentInstances_)
        {
            inst.write(writer);
		}

		writer.writeEndElement();
	}

	if (!interconnections_.isEmpty()) {
		writer.writeStartElement("spirit:interconnections");

		foreach (Interconnection inter, interconnections_) {
            inter.write(writer);
		}

		writer.writeEndElement();
	}

	if (!hierConnections_.isEmpty()) {
		writer.writeStartElement("spirit:hierConnections");

		foreach (HierConnection hier, hierConnections_) {
			hier.write(writer);
		}

		writer.writeEndElement();
	}

	if (!adHocConnections_.isEmpty()) {
		writer.writeStartElement("spirit:adHocConnections");

		foreach (AdHocConnection adhoc, adHocConnections_)
        {
            adhoc.write(writer);
		}

		writer.writeEndElement();
	}

	writer.writeStartElement("spirit:vendorExtensions");    
    writer.writeStartElement("kactus2:extensions");  

  // if contains kactus2 attributes
    if (!kactus2Attributes_.isEmpty()) {           
        writeKactus2Attributes(writer);
    }

    // Write SW instances if any.
    if (!swInstances_.empty())
    {
        writer.writeStartElement("kactus2:swInstances");

        foreach (SWInstance const& instance, swInstances_)
        {
            instance.write(writer);
        }
        
        writer.writeEndElement(); // kactus2:swInstances
    }

    // Write API dependencies if any.
    if (!apiDependencies_.empty())
    {
        writer.writeStartElement("kactus2:apiDependencies");

        foreach (ApiDependency const& dependency, apiDependencies_)
        {
            dependency.write(writer);
        }

        writer.writeEndElement(); // kactus2:apiDependencies
    }

    // Write hierarchical API dependencies if any.
    if (!hierApiDependencies_.empty())
    {
        writer.writeStartElement("kactus2:hierApiDependencies");

        foreach (HierApiDependency const& dependency, hierApiDependencies_)
        {
            dependency.write(writer);
        }

        writer.writeEndElement(); // kactus2:hierApiDependencies
    }

    // Write COM connections if any.
    if (!comConnections_.empty())
    {
        writer.writeStartElement("kactus2:comConnections");

        foreach (ComConnection const& conn, comConnections_)
        {
            conn.write(writer);
        }

        writer.writeEndElement(); // kactus2:comConnections
    }

    // Write hierarchical COM connections if any.
    if (!hierComConnections_.empty())
    {
        writer.writeStartElement("kactus2:hierComConnections");

        foreach (HierComConnection const& conn, hierComConnections_)
        {
            conn.write(writer);
        }

        writer.writeEndElement(); // kactus2:hierComConnections
    }

	if (!columns_.isEmpty())
	{
		writer.writeStartElement("kactus2:columnLayout");

		foreach (ColumnDesc const& columnDesc, columns_)
		{
            columnDesc.write(writer);
		}

		writer.writeEndElement(); // kactus2:columnLayout
	}

    writer.writeStartElement("kactus2:routes");

    foreach (Interconnection const& conn, interconnections_)
    {
        if (!conn.route.empty())
        {
            writer.writeStartElement("kactus2:route");
            writer.writeAttribute("kactus2:connRef", conn.name);

            if (conn.offPage)
            {
                writer.writeAttribute("kactus2:offPage", "true");
            }
            else
            {
                writer.writeAttribute("kactus2:offPage", "false");
            }

            foreach (QPointF const& point, conn.route)
            {
                XmlUtils::writePosition(writer, point);
            }

            writer.writeEndElement();
        }
    }

    foreach (AdHocConnection const& conn, adHocConnections_)
    {
        if (!conn.getRoute().empty())
        {
            writer.writeStartElement("kactus2:route");
            writer.writeAttribute("kactus2:connRef", conn.name());

            if (conn.isOffPage())
            {
                writer.writeAttribute("kactus2:offPage", "true");
            }
            else
            {
                writer.writeAttribute("kactus2:offPage", "false");
            }

            foreach (QPointF const& point, conn.getRoute())
            {
                XmlUtils::writePosition(writer, point);
            }

            writer.writeEndElement();
        }
    }

    writer.writeEndElement(); // kactus2:routes

    writer.writeEndElement(); // kactus2:extensions

    // Write the top-level component's port ad-hoc visibilities.
    XmlUtils::writeAdHocVisibilities(writer, portAdHocVisibilities_, adHocPortPositions_);

    writeVendorExtensions(writer);

	writer.writeEndElement(); // kactus2:vendorExtensions

	writer.writeEndElement();
	writer.writeEndDocument();
}

bool Design::isValid( QStringList& errorList ) const {
	bool valid = true;
	QString thisIdentifier(QObject::tr("containing design"));

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the design."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, thisIdentifier)) {
		valid = false;
	}
	else {
		thisIdentifier = QObject::tr("design %1").arg(vlnv_->toString());
	}

	QStringList instanceNames;
    QStringList hwUUIDs;

	foreach (ComponentInstance const& instance, componentInstances_)
    {		
        hwUUIDs.append(instance.getUuid());

		// if there are several instances with same name
		if (instanceNames.contains(instance.getInstanceName())) {
			errorList.append(QObject::tr("Design contains several instances with"
				" name %1").arg(instance.getInstanceName()));
			valid = false;
		}
		else {
			instanceNames.append(instance.getInstanceName());
		}

		if (!instance.isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList swInstanceNames;

    foreach (SWInstance const& instance, swInstances_)
    {
        if (swInstanceNames.contains(instance.getInstanceName()))
        {
            errorList.append(QObject::tr("Design contains several SW instances "
                                         "with name %1").arg(instance.getInstanceName()));
            valid = false;
        }
        else
        {
            swInstanceNames.append(instance.getInstanceName());
        }

        if (!instance.isValid(errorList, hwUUIDs, thisIdentifier))
        {
            valid = false;
        }
    }

	QStringList interconnectionNames;
	foreach (Design::Interconnection interconnection, interconnections_) {
		
		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name)) {
			errorList.append(QObject::tr("Design contains several interconnections"
				" with name '%1'").arg(interconnection.name));
			valid = false;
		}
		else {
			interconnectionNames.append(interconnection.name);
		}

		if (!interconnection.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

    QStringList comConnectionNames;
    QStringList allInstanceNames = instanceNames + swInstanceNames;

    foreach (ComConnection const& conn, comConnections_)
    {
        if (comConnectionNames.contains(conn.getName()))
        {
            errorList.append(QObject::tr("Design contains several COM connections "
                                         "with name '%1'").arg(conn.getName()));
            valid = false;
        }
        else
        {
            comConnectionNames.append(conn.getName());
        }

        if (!conn.isValid(errorList, allInstanceNames, thisIdentifier))
        {
            valid = false;
        }
    }

	QStringList hierComNames;
	foreach (const HierComConnection& hierComm, hierComConnections_) {
		if (hierComNames.contains(hierComm.getName())) {
			errorList.append(QObject::tr("Design contains several hierarchical COM "
				"connections with name %1").arg(hierComm.getName()));
			valid = false;
		}
		else {
			hierComNames.append(hierComm.getName());
		}

		if (!hierComm.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList apiDepNames;
	foreach (const ApiDependency& apiDep, apiDependencies_) {
		if (apiDepNames.contains(apiDep.getName())) {
			errorList.append(QObject::tr("Design contains several API dependencies"
				" with name %1").arg(apiDep.getName()));
			valid = false;
		}
		else {
			apiDepNames.append(apiDep.getName());
		}

		if (!apiDep.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList hierAPINames;
	foreach (const HierApiDependency& hierAPI, hierApiDependencies_) {
		if (hierAPINames.contains(hierAPI.getName())) {
			errorList.append(QObject::tr("Design contains several hierarchical "
				"API dependencies with name %1").arg(hierAPI.getName()));
			valid = false;
		}
		else {
			hierAPINames.append(hierAPI.getName());
		}

		if (!hierAPI.isValid(errorList, allInstanceNames, thisIdentifier)) {
			valid = false;
		}
	}

	QStringList adHocNames;
	foreach (AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name())) {
			errorList.append(QObject::tr("Design contains several ad hoc connections"
				" with name %1").arg(adHoc.name()));
			valid = false;
		}
		else {
			adHocNames.append(adHoc.name());
		}

		if (!adHoc.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	foreach (Design::HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames, errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

bool Design::isValid() const {
	if (!vlnv_) {
		return false;
	}
	else if (!vlnv_->isValid()) {
		return false;
	}

	QStringList instanceNames;
    QStringList hwUUIDs;

	foreach (ComponentInstance const& instance, componentInstances_)
    {
        hwUUIDs.append(instance.getUuid());

		// if there are several instances with same name
		if (instanceNames.contains(instance.getInstanceName())) {
			return false;
		}
		else {
			instanceNames.append(instance.getInstanceName());
		}

		if (!instance.isValid()) {
			return false;
		}
	}

	// check the SW instances
	QStringList swInstanceNames;
	foreach (const SWInstance& swInstance, swInstances_) {
		
		if (swInstanceNames.contains(swInstance.getInstanceName())) {
			return false;
		}
		else {
			swInstanceNames.append(swInstance.getInstanceName());
		}

		if (!swInstance.isValid(hwUUIDs)) {
			return false;
		}
	}

	QStringList interconnectionNames;
	foreach (Design::Interconnection interconnection, interconnections_) {

		// if there are several interconnections with same name
		if (interconnectionNames.contains(interconnection.name)) {
			return false;
		}
		else {
			interconnectionNames.append(interconnection.name);
		}

		if (!interconnection.isValid(instanceNames)) {
			return false;
		}
	}

	QStringList adHocNames;
	foreach (AdHocConnection adHoc, adHocConnections_) {
		if (adHocNames.contains(adHoc.name())) {
			return false;
		}
		else {
			adHocNames.append(adHoc.name());
		}

		if (!adHoc.isValid(instanceNames)) {
			return false;
		}
	}

	foreach (Design::HierConnection hierConn, hierConnections_) {
		if (!hierConn.isValid(instanceNames)) {
			return false;
		}
	}

	QStringList comConnectionNames;
	QStringList allInstanceNames = instanceNames + swInstanceNames;

	foreach (ComConnection const& conn, comConnections_)
	{
		if (comConnectionNames.contains(conn.getName())) {
			return false;
		}
		else {
			comConnectionNames.append(conn.getName());
		}

		if (!conn.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList hierComNames;
	foreach (const HierComConnection& hierComm, hierComConnections_) {
		if (hierComNames.contains(hierComm.getName())) {
			return false;
		}
		else {
			hierComNames.append(hierComm.getName());
		}

		if (!hierComm.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList apiDepNames;
	foreach (const ApiDependency& apiDep, apiDependencies_) {
		if (apiDepNames.contains(apiDep.getName())) {
			return false;
		}
		else {
			apiDepNames.append(apiDep.getName());
		}

		if (!apiDep.isValid(allInstanceNames)) {
			return false;
		}
	}

	QStringList hierAPINames;
	foreach (const HierApiDependency& hierAPI, hierApiDependencies_) {
		if (hierAPINames.contains(hierAPI.getName())) {
			return false;
		}
		else {
			hierAPINames.append(hierAPI.getName());
		}

		if (!hierAPI.isValid(allInstanceNames)) {
			return false;
		}
	}

	return true;
}

QList<ComponentInstance> const& Design::getComponentInstances() const
{
	return componentInstances_;
}

const QList<Design::Interconnection> &Design::getInterconnections()
{
	return interconnections_;
}

const QList<Design::HierConnection> &Design::getHierarchicalConnections()
{
	return hierConnections_;
}

const QList<AdHocConnection> &Design::getAdHocConnections()
{
	return adHocConnections_;
}

void Design::setComponentInstances(QList<ComponentInstance> const& componentInstances)
{
	componentInstances_ = componentInstances;
}

void Design::setInterconnections(QList<Design::Interconnection> const& interconnections)
{
	interconnections_ = interconnections;
}

void Design::setHierarchicalConnections(QList<Design::HierConnection> const& hierConnections)
{
	hierConnections_ = hierConnections;
}

void Design::setAdHocConnections(QList<AdHocConnection> const& adHocConnections)
{
	adHocConnections_ = adHocConnections;
}

const QStringList Design::getDependentFiles() const
{
	// TODO implement this
	return QStringList();
}

const QList<VLNV> Design::getDependentVLNVs() const {
	QList<VLNV> instanceList;

	// go through all component instances within the design
	for (int i = 0; i < componentInstances_.size(); ++i) {

		// if the pointer is valid and it is not already added to the list of
		// component instances (two instances of same component are not added
		// twice)
		if ((componentInstances_.at(i).getComponentRef().isValid()) &&
			(!instanceList.contains(componentInstances_.at(i).getComponentRef()))) {

				// add the component VLNV to the list
				instanceList.append(componentInstances_.at(i).getComponentRef());
		}
	}

	foreach (const SWInstance swInstance, swInstances_) {
		
		// if sw instance vlnv reference is valid and it is not yet on the list
		// of vlnv references
		if (swInstance.getComponentRef().isValid() && 
			!instanceList.contains(swInstance.getComponentRef())) {
			instanceList.append(swInstance.getComponentRef());
		}
	}
	return instanceList;
}

//-----------------------------------------------------------------------------
// Function: parseVendorExtensions()
//-----------------------------------------------------------------------------
void Design::parseVendorExtensions(QDomNode &node)
{
	QDomNodeList childNodes = node.childNodes();

    for (int i = 0; i < childNodes.size(); i++)
    {
        QDomNode childNode = childNodes.at(i);

        // Compatibility note: 
        // Kactus extensions are found under kactus:extensions or spirit:vendorExtensions.
        if (childNode.nodeName() == "kactus2:extensions")
        {            
            parseVendorExtensions(childNode);
        }
        // If the column layout was found, read the column descriptions.
        else if (childNode.nodeName() == "kactus2:columnLayout")
        {
            parseColumnLayout(childNode);
        }
        // Otherwise read the interconnection routes if they were found.
        else if (childNode.nodeName() == "kactus2:routes")
        {
            parseRoutes(childNode);
        }
        else if (childNode.nodeName() == "kactus2:adHocVisibilities")
        {
            parseAdHocVisibilities(childNode);
        }
        else if (childNode.nodeName() == "kactus2:swInstances")
        {
            parseSWInstances(childNode);
        }
        else if (childNode.nodeName() == "kactus2:apiDependencies")
        {
            parseApiDependencies(childNode);
        }
        else if (childNode.nodeName() == "kactus2:hierApiDependencies")
        {
            parseHierApiDependencies(childNode);
        }
        else if (childNode.nodeName() == "kactus2:comConnections")
        {
            parseComConnections(childNode);
        }
        else if (childNode.nodeName() == "kactus2:hierComConnections")
        {
            parseHierComConnections(childNode);
        }
        else if (childNode.nodeName() == "kactus2:kts_attributes")
        {
            parseKactus2Attributes(childNode);
        }
        else if (childNode.nodeName() == "kactus2:note")
        {
            parseStickyNote(childNode);
        }
        else
        {
            QSharedPointer<VendorExtension> extension = XmlUtils::createVendorExtensionFromNode(childNode);
            vendorExtensions_.append(extension);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseColumnLayout()
//-----------------------------------------------------------------------------
void Design::parseColumnLayout(QDomNode& layoutNode)
{
    QDomNodeList columnNodes = layoutNode.childNodes();

    for (int i = 0; i < columnNodes.size(); ++i)
    {
        if (columnNodes.at(i).nodeName() == "kactus2:column")
        {
            columns_.append(ColumnDesc(columnNodes.at(i)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseRoutes()
//-----------------------------------------------------------------------------
void Design::parseRoutes(QDomNode& routesNode)
{
    QDomNodeList connNodes = routesNode.childNodes();

    for (int i = 0; i < connNodes.size(); ++i)
    {
        QDomNode connNode = connNodes.at(i);

        if (connNode.nodeName() == "kactus2:route")
        {
            parseRoute(connNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseRoute()
//-----------------------------------------------------------------------------
void Design::parseRoute(QDomNode& routeNode)
{
    QString name = routeNode.attributes().namedItem("kactus2:connRef").nodeValue();
    QString offPageValue = routeNode.attributes().namedItem("kactus2:offPage").nodeValue();
    QList<QPointF> route;

    // Parse the route.
    for (int i = 0; i < routeNode.childNodes().size(); ++i)
    {
        QDomNode posNode = routeNode.childNodes().at(i);

        if (posNode.nodeName() == "kactus2:position")
        {
            route.append(XmlUtils::parsePoint(posNode));
        }
    }

    // Apply the route to the correct interconnection or ad-hoc connection.
    bool found = false;

    for (int i = 0; i < interconnections_.size() && !found; ++i)
    {
        if (interconnections_[i].name == name)
        {
            interconnections_[i].route = route;
            interconnections_[i].offPage = offPageValue == "true";
            found = true;
        }
    }

    for (int i = 0; i < adHocConnections_.size() && !found; ++i)
    {
        if (adHocConnections_[i].name() == name)
        {
            adHocConnections_[i].setRoute(route);
            adHocConnections_[i].setOffPage(offPageValue == "true");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseAdHocVisibilities()
//-----------------------------------------------------------------------------
void Design::parseAdHocVisibilities(QDomNode& visibilitiesNode)
{
    for (int i = 0; i < visibilitiesNode.childNodes().size(); ++i)
    {
        QDomNode adHocNode = visibilitiesNode.childNodes().at(i);

        if (adHocNode.nodeName() == "kactus2:adHocVisible")
        {
            QString name = adHocNode.attributes().namedItem("portName").nodeValue();
            portAdHocVisibilities_[name] = true;

            QPointF pos = XmlUtils::parsePoint(adHocNode);

            adHocPortPositions_[name] = pos;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseSWInstances()
//-----------------------------------------------------------------------------
void Design::parseSWInstances(QDomNode& swInstancesNode)
{
    for (int i = 0; i < swInstancesNode.childNodes().count(); ++i)
    {
        QDomNode swNode = swInstancesNode.childNodes().at(i);

        if (swNode.nodeName() == "kactus2:swInstance")
        {
            swInstances_.append(SWInstance(swNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseApiDependencies()
//-----------------------------------------------------------------------------
void Design::parseApiDependencies(QDomNode& dependenciesNode)
{
    for (int i = 0; i < dependenciesNode.childNodes().count(); ++i)
    {
        QDomNode apiNode = dependenciesNode.childNodes().at(i);

        if (apiNode.nodeName() == "kactus2:apiDependency")
        {
            apiDependencies_.append(ApiDependency(apiNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseHierApiDependencies()
//-----------------------------------------------------------------------------
void Design::parseHierApiDependencies(QDomNode& hierDependenciesNode)
{
    for (int i = 0; i < hierDependenciesNode.childNodes().count(); ++i)
    {
        QDomNode apiNode = hierDependenciesNode.childNodes().at(i);

        if (apiNode.nodeName() == "kactus2:hierApiDependency")
        {
            hierApiDependencies_.append(HierApiDependency(apiNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseComConnections()
//-----------------------------------------------------------------------------
void Design::parseComConnections(QDomNode& comConnectionsNode)
{
    for (int i = 0; i < comConnectionsNode.childNodes().count(); ++i)
    {
        QDomNode comNode = comConnectionsNode.childNodes().at(i);

        if (comNode.nodeName() == "kactus2:comConnection")
        {
            comConnections_.append(ComConnection(comNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseHierComConnections()
//-----------------------------------------------------------------------------
void Design::parseHierComConnections(QDomNode& hierComConnectionsNode)
{
    for (int i = 0; i < hierComConnectionsNode.childNodes().count(); ++i)
    {
        QDomNode comNode = hierComConnectionsNode.childNodes().at(i);

        if (comNode.nodeName() == "kactus2:hierComConnection")
        {
            hierComConnections_.append(HierComConnection(comNode));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Design::parseStickyNote()
//-----------------------------------------------------------------------------
void Design::parseStickyNote(QDomNode& noteNode)
{
    QSharedPointer<Kactus2Group> noteExtension(new Kactus2Group("kactus2:note"));

    for (int i = 0; i < noteNode.childNodes().count(); ++i)
    {
        QDomNode childNode = noteNode.childNodes().at(i);

        if (childNode.nodeName() == "kactus2:position")
        {
            QPointF position = XmlUtils::parsePoint(childNode);
            QSharedPointer<Kactus2Position> notePosition(new Kactus2Position(position));
            noteExtension->addToGroup(notePosition);
        }
        else if (childNode.nodeName() == "kactus2:content")
        {
            QString content = childNode.childNodes().at(0).nodeValue();
            QSharedPointer<Kactus2Value> noteContent(new Kactus2Value("kactus2:content", content));
            noteExtension->addToGroup(noteContent);
        }
    }

    vendorExtensions_.append(noteExtension);
}

//-----------------------------------------------------------------------------
// Function: Design::getPortAdHocVisibilities()
//-----------------------------------------------------------------------------
QMap<QString, bool> const& Design::getPortAdHocVisibilities() const
{
    return portAdHocVisibilities_;
}

//-----------------------------------------------------------------------------
// Function: Design::getAdHocPortPositions()
//-----------------------------------------------------------------------------
QMap<QString, QPointF> const& Design::getAdHocPortPositions() const
{
    return adHocPortPositions_;
}

//-----------------------------------------------------------------------------
// Function: getColumns()
//-----------------------------------------------------------------------------
QList<ColumnDesc> const& Design::getColumns() const
{
	return columns_;
}

//-----------------------------------------------------------------------------
// Function: Design::setPortAdHocVisibilities()
//-----------------------------------------------------------------------------
void Design::setPortAdHocVisibilities(QMap<QString, bool> const& portAdHocVisibilities)
{
    portAdHocVisibilities_ = portAdHocVisibilities;
}

//-----------------------------------------------------------------------------
// Function: Design::setAdHocPortPositions()
//-----------------------------------------------------------------------------
void Design::setAdHocPortPositions(QMap<QString, QPointF> const& val)
{
    adHocPortPositions_ = val;
}

//-----------------------------------------------------------------------------
// Function: setColumns()
//-----------------------------------------------------------------------------
void Design::setColumns(QList<ColumnDesc> const& columns)
{
	columns_ = columns;
}


QList<VLNV> Design::getComponents() const {

	QList<VLNV> list;

	foreach (ComponentInstance const& instance, componentInstances_) {
		if (instance.getComponentRef().isValid())
			list.append(instance.getComponentRef());
	}

    foreach (SWInstance const& instance, swInstances_) {
        if (instance.getComponentRef().isValid())
            list.append(instance.getComponentRef());
    }

	return list;
}

void Design::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::DESIGN);
}

//-----------------------------------------------------------------------------
// Function: Design::setApiDependencies()
//-----------------------------------------------------------------------------
void Design::setApiDependencies(QList<ApiDependency> const& apiDependencies)
{
    apiDependencies_ = apiDependencies;
}

//-----------------------------------------------------------------------------
// Function: Design::setApiDependencies()
//-----------------------------------------------------------------------------
void Design::setHierApiDependencies(QList<HierApiDependency> const& hierApiDependencies)
{
    hierApiDependencies_ = hierApiDependencies;
}

//-----------------------------------------------------------------------------
// Function: Design::setComConnections()
//-----------------------------------------------------------------------------
void Design::setComConnections(QList<ComConnection> const& comConnections)
{
    comConnections_ = comConnections;
}

//-----------------------------------------------------------------------------
// Function: Design::setHierComConnections()
//-----------------------------------------------------------------------------
void Design::setHierComConnections(QList<HierComConnection> const& hierComConnections)
{
    hierComConnections_ = hierComConnections;
}

//-----------------------------------------------------------------------------
// Function: Design::getSWInstances()
//-----------------------------------------------------------------------------
QList<SWInstance> const& Design::getSWInstances() const
{
    return swInstances_;
}

//-----------------------------------------------------------------------------
// Function: Design::setSWInstances()
//-----------------------------------------------------------------------------
void Design::setSWInstances(QList<SWInstance> const& swInstances)
{
    swInstances_ = swInstances;
}

//-----------------------------------------------------------------------------
// Function: Design::getApiDependencies()
//-----------------------------------------------------------------------------
QList<ApiDependency> const& Design::getApiDependencies() const
{
    return apiDependencies_;
}

//-----------------------------------------------------------------------------
// Function: Design::getHierApiDependencies()
//-----------------------------------------------------------------------------
QList<HierApiDependency> const& Design::getHierApiDependencies() const
{
    return hierApiDependencies_;
}

//-----------------------------------------------------------------------------
// Function: Design::getComConnections()
//-----------------------------------------------------------------------------
QList<ComConnection> const& Design::getComConnections() const
{
    return comConnections_;
}

//-----------------------------------------------------------------------------
// Function: Design::getHierComConnections()
//-----------------------------------------------------------------------------
QList<HierComConnection> const& Design::getHierComConnections() const
{
    return hierComConnections_;
}

bool Design::hasInterconnection( const QString& instanceName, const QString& interfaceName ) const {

	foreach (Design::Interconnection conn, interconnections_) {
		
		// check if either interface of the interconnection matches
		if (conn.interface1.componentRef == instanceName &&
			conn.interface1.busRef == interfaceName) {
				return true;
		}
		else if (conn.interface2.componentRef == instanceName &&
			conn.interface2.busRef == interfaceName) {
				return true;
		}
		
	}

	// no interconnection was found
	return false;
}

QList<Design::Interface> Design::getConnectedInterfaces( const QString& instanceName, const QString& interfaceName ) const {
	Design::Interface interface(instanceName, interfaceName);
	return getConnectedInterfaces(interface);
}

QList<Design::Interface> Design::getConnectedInterfaces( const Interface& sourceInterface ) const {
	QList<Design::Interface> interfaces;

	foreach (Design::Interconnection conn, interconnections_) {
		if (conn.interface1 == sourceInterface) {
			interfaces.append(conn.interface2);
		}
		else if (conn.interface2 == sourceInterface) {
			interfaces.append(conn.interface1);
		}
	}

	return interfaces;
}

VLNV Design::getHWComponentVLNV( const QString& instanceName ) const {
	foreach (ComponentInstance instance, componentInstances_) {
		// if instance is found
		if (0 == instanceName.compare(instance.getInstanceName(), Qt::CaseInsensitive)) {
			return instance.getComponentRef();
		}
	}

	// no instance was found
	return VLNV();
}

bool Design::containsHWInstance( const QString& instanceName ) const {
	foreach (ComponentInstance instance, componentInstances_) {
		// if instance is found
		if (0 == instanceName.compare(instance.getInstanceName(), Qt::CaseInsensitive)) {
			return true;
		}
	}

	// no instance with given name was found
	return false;
}

bool Design::hasConfElementValue( const QString& instanceName, const QString& confElementName ) const {
	foreach (ComponentInstance instance, componentInstances_) {
		
		// when the instance is found
		if (0 == instanceName.compare(instance.getInstanceName(), Qt::CaseInsensitive)) {
			return instance.hasConfElementValue(confElementName);
		}
	}

	// specified instance was not found
	return false;
}

QString Design::getConfElementValue( const QString& instanceName, const QString& confElementName ) const {
	foreach (ComponentInstance instance, componentInstances_) {

		// when the instance is found
		if (0 == instanceName.compare(instance.getInstanceName(), Qt::CaseInsensitive)) {
			return instance.getConfElementValue(confElementName);
		}
	}

	// specified instance was not found
	return QString();
}
	
void Design::setDesignImplementation(KactusAttribute::Implementation implementation){
	kactus2Attributes_.insert("kts_implementation", KactusAttribute::valueToString(implementation));
}

KactusAttribute::Implementation Design::getDesignImplementation() const {
    KactusAttribute::Implementation implementation = KactusAttribute::KTS_HW;

    if (!kactus2Attributes_.contains(QString("kts_implementation")))
        return implementation;
    else
        KactusAttribute::stringToValue(kactus2Attributes_.value(QString("kts_implementation")),
        implementation);
    return implementation;
}

QString Design::getHWInstanceDescription( const QString& instanceName ) const {
	foreach (ComponentInstance instance, componentInstances_) {
		if (0 == instanceName.compare(instance.getInstanceName(), Qt::CaseInsensitive)) {
			return instance.getDescription();
		}
	}

	// no instance found
	return QString();
}

Design::Interface::Interface(QDomNode &interfaceNode):
componentRef(""), 
busRef("")
{
    QDomNamedNodeMap attributes = interfaceNode.attributes();

    componentRef = attributes.namedItem("spirit:componentRef").nodeValue();
    busRef = attributes.namedItem("spirit:busRef").nodeValue();
}

Design::Interface::Interface(QString componentRef_, QString busRef_)
    : componentRef(componentRef_), busRef(busRef_)
{
}

Design::Interface::Interface( const Interface& other ):
componentRef(other.componentRef),
busRef(other.busRef) {
}

Design::Interface& Design::Interface::operator=( const Interface& other ) {
	if (this != &other) {
		componentRef = other.componentRef;
		busRef = other.busRef;
	}
	return *this;
}

bool Design::Interface::operator==( const Design::Interface& other ) {
	if (0 == componentRef.compare(other.componentRef, Qt::CaseInsensitive) &&
		0 == busRef.compare(other.busRef, Qt::CaseInsensitive)) {
			return true;
	} 
	return false;
}

bool Design::Interface::operator!=( const Design::Interface& other ) {
	if (0 != componentRef.compare(other.componentRef, Qt::CaseInsensitive)) {
		return true;
	}
	else if (0 != busRef.compare(other.busRef, Qt::CaseInsensitive)) {
		return true;
	}
	else {
		return false;
	}
}

bool Design::Interface::operator<( const Design::Interface& other ) {
	int compResult = componentRef.compare(other.componentRef, Qt::CaseInsensitive);
	if (compResult == 0) {
		return busRef.compare(other.busRef, Qt::CaseInsensitive) < 0;
	}
	else {
		return compResult < 0;
	}
}

bool Design::Interface::isValid( const QStringList& instanceNames,
								QStringList& errorList,
								const QString& parentIdentifier ) const {
	bool valid = true;
	
	if (componentRef.isEmpty()) {
		errorList.append(QObject::tr("No component reference set for active"
			" interface within %1").arg(parentIdentifier));
		valid = false;
	}
	else if (!instanceNames.contains(componentRef)) {
		errorList.append(QObject::tr("Active interface within %1 contains "
			"reference to component instance %2 that does not exist.").arg(
			parentIdentifier).arg(componentRef));
		valid = false;
	}

	if (busRef.isEmpty()) {
		errorList.append(QObject::tr("No bus reference set for active interface"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	return valid;
}

bool Design::Interface::isValid( const QStringList& instanceNames ) const {
	if (componentRef.isEmpty()) {
		return false;
	}
	else if (!instanceNames.contains(componentRef)) {
		return false;
	}

	if (busRef.isEmpty()) {
		return false;
	}

	return true;
}

Design::Interconnection::Interconnection(QDomNode &interconnectionNode)
    : name(), displayName(), description(),
      interface1(QString(), QString()), interface2(QString(), QString()), route(), offPage(false)
{
    QDomNodeList nodes = interconnectionNode.childNodes();

    for (int i = 0; i < nodes.size(); i++) {
	QDomNode node = nodes.at(i);

	if (node.nodeName() == "spirit:name") {
	    name = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:displayName") {
	    displayName = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:description") {
	    description = node.firstChild().nodeValue();
	} else if (node.nodeName() == "spirit:activeInterface") {
	    if (interface1.componentRef.isNull())
		interface1 = Interface(node);
	    else
		interface2 = Interface(node);
	} // TODO: spirit:monitoredActiveInterface spirit:monitorInterface
    }
}

Design::Interconnection::Interconnection(QString name,
					 Interface interface1,
					 Interface interface2,
                     QList<QPointF> const& route,
                     bool offPage,
					 QString displayName,
					 QString description)
    : name(name), displayName(displayName), description(description),
      interface1(interface1), interface2(interface2), route(route), offPage(offPage)
{
}

Design::Interconnection::Interconnection( const Interconnection& other ):
name(other.name),
displayName(other.displayName),
description(other.description),
interface1(other.interface1),
interface2(other.interface2),
route(other.route),
offPage(other.offPage)
{
}

Design::Interconnection& Design::Interconnection::operator=( const Interconnection& other ) {
	if (this != &other) {
		name = other.name;
		displayName = other.displayName;
		description = other.description;
		interface1 = other.interface1;
		interface2 = other.interface2;
		route = other.route;
        offPage = other.offPage;
	}
	return *this;
}

bool Design::Interconnection::isValid( const QStringList& instanceNames, 
									  QStringList& errorList,
									  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("interconnection within %1").arg(
		parentIdentifier));

	if (name.isEmpty()) {
		errorList.append(QObject::tr("No name specified for interconnection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!interface1.isValid(instanceNames, errorList, thisIdentifier)) {
		valid = false;
	}

	if (!interface2.isValid(instanceNames, errorList, thisIdentifier)) {
		valid = false;
	}

	return valid;
}

bool Design::Interconnection::isValid( const QStringList& instanceNames ) const {
	if (name.isEmpty()) {
		return false;
	}

	if (!interface1.isValid(instanceNames)) {
		return false;
	}

	if (!interface2.isValid(instanceNames)) {
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: Design::Interconnection::write()
//-----------------------------------------------------------------------------
void Design::Interconnection::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:interconnection");

    writer.writeTextElement("spirit:name", name);
    writer.writeTextElement("spirit:displayName", displayName);
    writer.writeTextElement("spirit:description", description);

    writer.writeEmptyElement("spirit:activeInterface");
    writer.writeAttribute("spirit:componentRef", interface1.componentRef);
    writer.writeAttribute("spirit:busRef", interface1.busRef);

    writer.writeEmptyElement("spirit:activeInterface");
    writer.writeAttribute("spirit:componentRef", interface2.componentRef);
    writer.writeAttribute("spirit:busRef", interface2.busRef);

    writer.writeEndElement();
}

Design::HierConnection::HierConnection(QDomNode &hierConnectionNode)
    : interfaceRef(), interface_(QString(""), QString("")), position(), route(), offPage(false),
      vendorExtensions_()
{
    QDomNamedNodeMap attributes = hierConnectionNode.attributes();

    interfaceRef = attributes.namedItem("spirit:interfaceRef").nodeValue();

    for (int i = 0; i < hierConnectionNode.childNodes().size(); i++) {
        QDomNode node = hierConnectionNode.childNodes().at(i);

        if (node.nodeName() == "spirit:interface")
            interface_ = Interface(node);
        else if (node.nodeName() == "spirit:vendorExtensions")
        {
            for (int i = 0; i < node.childNodes().size(); ++i)
            {
                QDomNode childNode = node.childNodes().at(i);

                if (childNode.nodeName() == "kactus2:position")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    position.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    position.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:direction")
                {
                    QDomNamedNodeMap attributeMap = childNode.attributes();
                    direction.setX(attributeMap.namedItem("x").nodeValue().toInt());
                    direction.setY(attributeMap.namedItem("y").nodeValue().toInt());
                }
                else if (childNode.nodeName() == "kactus2:route")
                {
                    offPage = childNode.attributes().namedItem("kactus2:offPage").nodeValue() == "true";

                    // Parse the route.
                    for (int i = 0; i < childNode.childNodes().size(); ++i)
                    {
                        QDomNode posNode = childNode.childNodes().at(i);
                        QPointF pos;

                        if (posNode.nodeName() == "kactus2:position")
                        {
                            pos.setX(posNode.attributes().namedItem("x").nodeValue().toInt());
                            pos.setY(posNode.attributes().namedItem("y").nodeValue().toInt());
                            route.append(pos);
                        }
                    }
                }
                else
                {
                    QSharedPointer<VendorExtension> extension = 
                        XmlUtils::createVendorExtensionFromNode(childNode); 
                    vendorExtensions_.append(extension);
                }
            }
        }
    }
}

Design::HierConnection::HierConnection(QString interfaceRef_,
                                       Interface interface_,
                                       QPointF const& position,
                                       QVector2D const& direction,
                                       QList<QPointF> const& route,
                                       bool offPage)
    : interfaceRef(interfaceRef_), interface_(interface_),
      position(position), direction(direction), route(route), offPage(offPage), vendorExtensions_()
{
}

Design::HierConnection::HierConnection( const HierConnection& other ):
interfaceRef(other.interfaceRef),
interface_(other.interface_),
position(other.position),
direction(other.direction),
route(other.route),
offPage(other.offPage),
vendorExtensions_(other.vendorExtensions_)
{
}

Design::HierConnection& Design::HierConnection::operator=( const HierConnection& other ) {
	if (this != &other) {
		interfaceRef = other.interfaceRef;
		interface_ = other.interface_;
		position = other.position;
		direction = other.direction;
		route = other.route;
        offPage = other.offPage;
        vendorExtensions_ = other.vendorExtensions_;
	}
	return *this;
}

bool Design::HierConnection::isValid( const QStringList& instanceNames,
									 QStringList& errorList,
									 const QString& parentIdentifier ) const {
	bool valid = true;

	if (interfaceRef.isEmpty()) {
		errorList.append(QObject::tr("No interface reference set for hier connection"
			" within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!interface_.isValid(instanceNames, errorList, QObject::tr("hier connection"
		" within %1").arg(parentIdentifier))) {
			valid = false;
	}

	return valid;
}

bool Design::HierConnection::isValid( const QStringList& instanceNames ) const {
	if (interfaceRef.isEmpty()) {
		return false;
	}

	if (!interface_.isValid(instanceNames)) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: Design::HierConnection::write()
//-----------------------------------------------------------------------------
void Design::HierConnection::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement("spirit:hierConnection");

    writer.writeAttribute("spirit:interfaceRef", interfaceRef);

    writer.writeEmptyElement("spirit:interface");
    writer.writeAttribute("spirit:componentRef", interface_.componentRef);
    writer.writeAttribute("spirit:busRef", interface_.busRef);

    // Write custom data to vendor extensions.
    writer.writeStartElement("spirit:vendorExtensions");

    XmlUtils::writePosition(writer, position);
    XmlUtils::writeDirection(writer, direction);

    if (!route.empty())
    {
        writer.writeStartElement("kactus2:route");

        if (offPage)
        {
            writer.writeAttribute("kactus2:offPage", "true");
        }
        else
        {
            writer.writeAttribute("kactus2:offPage", "false");
        }

        foreach (QPointF const& point, route)
        {
            XmlUtils::writePosition(writer, point);
        }

        writer.writeEndElement();
    }

    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);

    writer.writeEndElement(); // spirit:vendorExtensions.

    writer.writeEndElement();
}
