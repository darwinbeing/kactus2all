//-----------------------------------------------------------------------------
// File: ComProperty.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 2.4.2012
//
// Description:
// Property class for communication definitions.
//-----------------------------------------------------------------------------

#ifndef COMPROPERTY_H
#define COMPROPERTY_H

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Property class for communication definitions.
//-----------------------------------------------------------------------------
class ComProperty
{
public:
    /*!
     *  Default constructor.
     */
    ComProperty();

    /*!
     *  Copy constructor.
     */
    ComProperty(ComProperty const& rhs);

    /*!
     *  Constructor which reads the property from an XML node.
     */
    ComProperty(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ComProperty();

    /*!
     *  Writes the contents of the property to an XML stream.
     *
     *      @param [in] write The XML stream writer.
     */
    void write(QXmlStreamWriter& writer);

    /*!
     *  Returns true if the property is in valid state.
     */
    bool isValid() const;

    /*!
     *  Sets the name of the property.
     */
    void setName(QString const& name);

    /*!
     *  Sets the property required/optional.
     *
     *      @param [in] required If true, the property is a required one; otherwise it is optional.
     */
    void setRequired(bool required);

    /*!
     *  Sets the property type.
     *
     *      @param [in] type The property type as a string.
     */
    void setType(QString const& type);

    /*!
     *  Sets the default value for the property.
     */
    void setDefaultValue(QString const& defaultValue);

    /*!
     *  Returns the name of the property.
     */
    QString const& getName() const;

    /*!
     *  Returns true if the property is required.
     */
    bool isRequired() const;

    /*!
     *  Returns the property type.
     */
    QString const& getType() const;

    /*!
     *  Returns the default value for the property.
     */
    QString const& getDefaultValue() const;

    /*!
     *  Assignment operator.
     */
    ComProperty& operator=(ComProperty const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the property.
    QString name_;

    //! If true, the property is a required one.
    bool required_;

    //! The property type.
    QString type_;

    //! The default value.
    QString defaultValue_;
};

//-----------------------------------------------------------------------------

#endif // COMPROPERTY_H
