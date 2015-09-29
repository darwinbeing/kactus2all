//-----------------------------------------------------------------------------
// File: AlternateRegister.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Describes the ipxact:alternateRegister element.
//-----------------------------------------------------------------------------

#ifndef ALTERNATEREGISTER_H
#define ALTERNATEREGISTER_H

#include "RegisterDefinition.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QStringList>
#include <QString>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Describes the ipxact:alternateRegister element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AlternateRegister : public RegisterDefinition
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            Name of the alternate register.
     *      @param [in] alternateGroup  Name of the first alternate group.
     */
    AlternateRegister(QString const& name = QString(), QString const& alternateGroup = QString());

	//! Copy constructor.
	AlternateRegister(const AlternateRegister& other);

	//! Assignment operator.
	AlternateRegister& operator=(const AlternateRegister& other);

	//! The destructor.
	virtual ~AlternateRegister();

	/*!
     *  Clone the alternate register and return pointer to the copy.
	 * 
	 *      @return Pointer to the cloned alternate register.
	 */
    virtual QSharedPointer<RegisterBase> clone();

	/*! \brief Check if the alternate register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
    // virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    //    QStringList& errorList, const QString& parentIdentifier) const;

	/*! \brief Check if the alternate register is in a valid state.
	 *
     * \param componentChoices  Choices in the containing component.
     *
	 * \return bool True if the state is valid and writing is possible.
	*/
    //  virtual bool isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const;

	/*!
     *  Get the list of the alternate groups.
	 *
	 *      @return A QList containing the names of the groups.
	 */
    QSharedPointer<QStringList> getAlternateGroups() const;

	/*!
     *  Set the alternateGroups for this alternateRegister.
	 *
	 *      @param [in] newAlternateGroups  A QList containing names of the new groups.
	 */
    void setAlternateGroups(QSharedPointer<QStringList> newAlternateGroups);

private:

    /*!
     *  Copy the alternate groups.
     *
     *      @param [in] other   The alternate register being copied.
     */
    void copyAlternateGroups(const AlternateRegister& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! Contains the names of the alternateGroups.
    QSharedPointer<QStringList> alternateGroups_;
};

#endif // ALTERNATEREGISTER_H
