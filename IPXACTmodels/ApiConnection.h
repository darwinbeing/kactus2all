//-----------------------------------------------------------------------------
// File: ApiDependency.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 11.4.2012
//
// Description:
// Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------

#ifndef APIDEPENDENCY_H
#define APIDEPENDENCY_H

#include "ipxactmodels_global.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>
#include <QStringList>

//-----------------------------------------------------------------------------
//! ApiInterfaceRef structure.
//-----------------------------------------------------------------------------
struct ApiInterfaceRef
{
    QString componentRef;   //!< Name reference to an SW component instance.
    QString apiRef;         //!< Name reference to an API interface that is contained by the SW component instance.

    /*!
     *  Default constructor.
     */
    ApiInterfaceRef() : componentRef(), apiRef()
    {
    }

    /*!
     *  Constructor.
     */
    ApiInterfaceRef(QString const& componentRef_, QString const& apiRef_)
        : componentRef(componentRef_),
          apiRef(apiRef_)
    {
    }
};

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiConnection
{
public:
    /*!
     *  Default constructor.
     */
    ApiConnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name         The name to set.
     *      @param [in] displayName  The display name to set.
     *      @param [in] description  The description to set.
     *      @param [in] ref1         The first interface reference.
     *      @param [in] ref2         The second interface reference.
     *      @param [in] route        The connection route.
     *      @param [in] imported     If true, the connection is an imported one.
     */
    ApiConnection(QString const& name, QString const& displayName, QString const& description,
                  ApiInterfaceRef const& ref1, ApiInterfaceRef const& ref2,
                  QList<QPointF> const& route, bool imported = false);

    /*!
     *  Copy constructor.
     */
    ApiConnection(ApiConnection const& rhs);

    /*!
     *  Constructor which reads the API dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiConnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiConnection();

    /*!
     *  Writes the contents to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

	/*! \brief Check if the API dependency is in valid state.
	 *
	 * \param errorList The string list to add the possible error messages to.
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 * \param parentId The identifier for the design containing the dependencies.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	bool isValid(QStringList& errorList, QStringList const& instanceNames,
		QString const& parentId) const;

	/*! \brief Check if the API dependency is in valid state.
	 *
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the name of the dependency.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the dependency.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the dependency.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the provider reference.
     *
     *      @param [in] ref The interface reference.
     */
    void setInterface1(ApiInterfaceRef const& ref);

    /*!
     *  Sets the requester reference.
     *
     *      @param [in] ref The interface reference.
     */
    void setInterface2(ApiInterfaceRef const& ref);

    /*!
     *  Sets the connection route.
     *
     *      @param [in] route The route to set.
     */
    void setRoute(QList<QPointF> const& route);

    /*!
     *  Sets the flag whether the connection is an imported one and should be auto-synced.
     *
     *      @param [in] imported If true, the connection is set as imported.
     */
    void setImported(bool imported);

    /*!
     *  Sets the flag whether the connection is off-page or not.
     *
     *      @param [in] offPage If true, the connection is set off-page.
     */
    void setOffPage(bool offPage);

    /*!
     *  Returns the name of the dependency.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the SW instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the SW instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the provider reference.
     */
    ApiInterfaceRef const& getInterface1() const;

    /*!
     *  Returns the requester reference.
     */
    ApiInterfaceRef const& getInterface2() const;

    /*!
     *  Returns the connection route.
     */
    QList<QPointF> const& getRoute() const;

    /*!
     *  Returns true if the connection is an imported one.
     */
    bool isImported() const;

    /*!
     *  Returns true if the connection is off-page.
     */
    bool isOffPage() const;

    /*!
     *  Assignment operator.
     */
    ApiConnection& operator=(ApiConnection const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the dependency.
    QString name_;

    //! The display name of the dependency.
    QString displayName_;

    //! The description of the dependency.
    QString desc_;

    //! Provider reference.
    ApiInterfaceRef interface1_;

    //! Requester reference.
    ApiInterfaceRef interface2_;

    //! The connection route.
    QList<QPointF> route_;

    //! If true, the connection is an imported one.
    bool imported_;

    //! If true, the connection is off-page.
    bool offPage_;
};

//-----------------------------------------------------------------------------

#endif // APIDEPENDENCY_H
