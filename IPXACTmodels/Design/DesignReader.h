//-----------------------------------------------------------------------------
// File: DesignReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.08.2015
//
// Description:
// XML reader class for IP-XACT design element.
//-----------------------------------------------------------------------------

#ifndef DESIGNREADER_H
#define DESIGNREADER_H

#include "Design.h"
#include "ActiveInterface.h"
#include "HierInterface.h"
#include "MonitorInterface.h"
#include "PortReference.h"

#include <IPXACTmodels/common/DocumentReader.h>

#include <QObject>
#include <QDomNode>
#include <QDomNodeList>

//-----------------------------------------------------------------------------
//! XML reader class for IP-XACT design element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT DesignReader : public DocumentReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent object.
     */
    DesignReader(QObject* parent = 0);

    /*!
	 *  The destructor.
	 */
    ~DesignReader();

    /*!
     *  Creates a design from XML definition.
     *
     *      @param [in] document    The XML document to create the design from.
     *
     *      @return The created design.
     */
    QSharedPointer<Design> createDesignFrom(QDomDocument const& document) const;

private:

    // Disable copying.
    DesignReader(DesignReader const& rhs);
    DesignReader& operator=(DesignReader const& rhs);

    /*!
     *  Reads the component instances from XML to design.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseComponentInstances(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads the interconnections from XML to design.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseInterconnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads a single interconnection from XML.
     *
     *      @param [in] interconnectionNode     The XML description of the interconnection.
     *      @param [in] newDesign               The new design item.
     */
    void parseSingleInterconnection(const QDomNode& interconnectionNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads an interconnection between components.
     *
     *      @param [in] interconnectionNode     The XML description of the interconnection.
     *      @param [in] newDesign               The new design item.
     *      @param [in] name                    The name of the interconnection.
     *      @param [in] displayName             The display name of the interconnection.
     *      @param [in] description             The description of the interconnection.
     *      @param [in] isPresent               The presence of the interconnection.
     */
    void parseComponentInterconnection(const QDomNode& interconnectionNode, QSharedPointer<Design> newDesign,
        QString const& name, QString const& displayName, QString const& description, QString const& isPresent)
        const;

    /*!
     *  Reads interconnection extensions.
     *
     *      @param [in] interconnectionNode     XML description of the interconnection.
     *      @param [in] interconnection         The new interconnection item.
     */
    void parseInterconnectionExtensions(const QDomNode& interconnectionNode,
        QSharedPointer<Interconnection> interconnection) const;

    /*!
     *  Reads an active interface.
     *
     *      @param [in] interfaceNode   The XML description of the active interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseActiveInterface(const QDomNode& interfaceNode, QSharedPointer<ActiveInterface> newInterface) const;
    
    /*!
     *  Reads a hierarchical interface.
     *
     *      @param [in] interfaceNode   The XML description of the hierarchical interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseHierInterface(const QDomNode& interfaceNode, QSharedPointer<HierInterface> newInterface) const;

    /*!
     *  Reads interface extensions.
     *
     *      @param [in] interfaceNode   XML description of the hierarchical interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseHierInterfaceExtensions(const QDomNode& interfaceNode, QSharedPointer<HierInterface> newInterface)
        const;

    /*!
     *  Reads a monitor interconnection.
     *
     *      @param [in] monitorNode     The XML description of the monitor interconnection.
     *      @param [in] newDesign       The new design item.
     *      @param [in] name            The name of the interconnection.
     *      @param [in] displayName     The display name of the interconnection.
     *      @param [in] description     The description of the interconnection.
     *      @param [in] isPresent       The presence of the interconnection.
     */
    void parseMonitorInterconnection(const QDomNode& monitorNode, QSharedPointer<Design> newDesign,
        QString const& name, QString const& displayName, QString const& description, QString const& isPresent)
        const;

    /*!
     *  Reads a monitor interface.
     *
     *      @param [in] interfaceNode   The XML description of the monitor interface.
     *      @param [in] newInterface    The new interface item.
     */
    void parseMonitorInterface(const QDomNode& interfaceNode, QSharedPointer<MonitorInterface> newInterface) const;

    /*!
     *  Reads the ad-hoc connections.
     *
     *      @param [in] designNode  The XML description of the design.
     *      @param [in] newDesign   The new design item.
     */
    void parseAdHocConnections(const QDomNode& designNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads a single ad-hoc connection.
     *
     *      @param [in] adHocNode   XML description of an ad-hoc connection.
     *      @param [in] newDesign   The new design item.
     */
    void parseSingleAdHocConnection(const QDomNode& adHocNode, QSharedPointer<Design> newDesign) const;

    /*!
     *  Reads the internal port references.
     *
     *      @param [in] internalNodes       XML description of the internal port references.
     *      @param [in] newAdHocConnection  The new ad-hoc connection item.
     */
    void parseInternalPortReferences(const QDomNodeList& internalNodes,
        QSharedPointer<AdHocConnection> newAdHocConnection) const;

    /*!
     *  Reads the external port references.
     *
     *      @param [in] externalNodes       XML description of the external port references.
     *      @param [in] newAdHocConnection  The new ad-hoc connection item.
     */
    void parseExternalPortReferences(const QDomNodeList& externalNodes,
        QSharedPointer<AdHocConnection> newAdHocConnection) const;

    /*!
     *  Creates a port reference.
     *
     *      @param [in] portReferenceNode   The XML description of the port.
     */
    QSharedPointer<PortReference> createPortReference(const QDomNode& portReferenceNode) const;

    /*!
     *  Reads ad-hoc connection extensions.
     *
     *      @param [in] adHocNode           XML description of the ad-hoc connection.
     *      @param [in] newAdHocConnection  The new ad-hoc connection item.
     */
    void parseAdHocConnectionExtensions(const QDomNode& adHocNode,
        QSharedPointer<AdHocConnection> newAdHocConnection) const;

    /*!
     *  Reads the design vendor extensions.
     *
     *      @param [in] documentNode    The XML description of the document.
     *      @param [in] design          The new design item.
     */
    void parseDesignExtensions(QDomNode const& documentNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the design column layout (Kactus2 extension).
     *
     *      @param [in] columnNode  The XML description of the column layout.
     *      @param [in] design      The new design item.
     */
    void parseColumnLayout(QDomNode const& columnNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the software instances (Kactus2 extension).
     *
     *      @param [in] swInstancesNode     The XML description of the software instances.
     *      @param [in] design              The new design item.
     */
    void parseSwInstances(QDomNode const& swInstancesNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the ad-hoc port (Kactus2:extension).
     *
     *      @param [in] adHocsNode  The XML description of the ad-hoc ports.
     *      @param [in] design      The new design item.
     */
    void parseAdHocPortPositions(QDomNode const& adHocsNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the api connections (Kactus2 extension).
     *
     *      @param [in] apiConnectionsNode  The XML description of the api connections.
     *      @param [in] design              The new design item.
     */
    void parseApiConnections(QDomNode const& apiConnectionsNode, QSharedPointer<Design>design) const;

    /*!
     *  Reads the hierarchical api connections (Kactus2 extension).
     *
     *      @param [in] hierApiConnectionsNode  The XML description of the hierarchical api connections.
     *      @param [in] design                  The new design item.
     */
    void parseHierApiConnections(QDomNode const& hierApiConnectionsNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the com connections (Kactus2 extension).
     *
     *      @param [in] comConnectionsNode  The XML description of the com connections.
     *      @param [in] design              The new design item.
     */
    void parseComConnections(QDomNode const& comConnectionsNode, QSharedPointer<Design> design) const;

    /*!
     *  Reads the hierarchical com connections.
     *
     *      @param [in] hierComConnectionsNode  The XML description of the hierarchical com connections.
     *      @param [in] design                  The new design item.
     */
    void parseHierComConnections(QDomNode const& hierComConnectionsNode, QSharedPointer<Design> design) const;
};

#endif // DESIGNREADER_H
