//-----------------------------------------------------------------------------
// File: ApiInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 3.4.2012
//
// Description:
// API interface class for defining properties of a specific API interface
// in a SW component.
//-----------------------------------------------------------------------------

#ifndef APIINTERFACE_H
#define APIINTERFACE_H

#include <LibraryManager/vlnv.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Dependency direction enumeration.
//-----------------------------------------------------------------------------
enum DependencyDirection
{
    DEPENDENCY_REQUESTER = 0,   //!< The API interface requires the API.
    DEPENDENCY_PROVIDER         //!< The API interface provides the API for those who need it.
};

/*!
 *  Converts the string to an equivalent enumeration value.
 */
DependencyDirection str2DependencyDirection(QString const& str, DependencyDirection defaultValue);

/*!
 *  Converts the enumeration value to an equivalent string.
 *
 *      @param [in] dir The dependency direction enum value.
 *
 *      @return The equivalent string.
 */
QString dependencyDirection2Str(DependencyDirection dir);

//-----------------------------------------------------------------------------
//! API interface class for defining properties of a specific API interface
//! in a SW component.
//-----------------------------------------------------------------------------
class ApiInterface
{
public:
    /*!
     *  Default constructor.
     */
    ApiInterface();

    /*!
     *  Copy constructor.
     */
    ApiInterface(ApiInterface const& rhs);

    /*!
     *  Constructor which reads the API interface from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiInterface(QDomNode& node);

    /*!
     *  Writes the contents of the API interface to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     *
     *      @param [in/out] errorList Error list which is appended with errors if found while validating.
     */
    bool isValid(QStringList& errorList) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     */
    bool isValid() const;

    /*!
     *  Sets the name of the API interface.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the API interface.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the API interface description.
     *
     *      @param [in] desc The description.
     */
    void setDescription(QString const& desc);

    /*!
     *  Sets the API type.
     *
     *      @param [in] vlnv The API definition VLNV.
     */
    void setApiType(VLNV const& vlnv);

    /*!
     *  Sets the dependency direction.
     *
     *      @param [in] dir The dependency direction.
     */
    void setDependencyDirection(DependencyDirection dir);

    /*!
     *  Returns the name of the API interface.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the API interface.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the API interface description.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the API type (API definition VLNV).
     */
    VLNV const& getApiType() const;

    /*!
     *  Returns the dependency direction.
     */
    DependencyDirection getDependencyDirection() const;

    /*!
     *  Assignment operator.
     */
    ApiInterface& operator=(ApiInterface const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the API interface.
    QString name_;

    //! The display name for the API interface.
    QString displayName_;

    //! The API interface description.
    QString desc_;

    //! The API type (i.e. a VLNV reference to an API definition).
    VLNV apiType_;

    //! Dependency direction.
    DependencyDirection dependencyDir_;    
};

//-----------------------------------------------------------------------------

#endif // APIINTERFACE_H
