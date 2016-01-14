//-----------------------------------------------------------------------------
// File: ViewConfigurationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 13.01.2016
//
// Description:
// Validator for the ipxact:viewConfiguration.
//-----------------------------------------------------------------------------

#ifndef VIEWCONFIGURATIONVALIDATOR_H
#define VIEWCONFIGURATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class ViewConfiguration;

class ExpressionParser;

class View;
class ComponentInstance;

class LibraryInterface;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:viewConfiguration.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewConfigurationValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] library     The used library interface.
     *      @param [in] parser      The used expression parser.
     */
    ViewConfigurationValidator(LibraryInterface* library, QSharedPointer<ExpressionParser> parser);

	//! The destructor.
	~ViewConfigurationValidator();
    
    /*!
     *  Change the available component instances.
     *
     *      @param [in] newInstances    The new component instances.
     */
    void changeComponentInstances(QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances);

    /*!
     *  Validates the given view configuration.
     *
     *      @param [in] configuration   The selected view configuration.
     *
     *      @return True, if the view configuration is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<ViewConfiguration> configuration);

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] configuration   The selected view configuration.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<ViewConfiguration> configuration) const;

    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] configuration   The selected view configuration.
     *
     *      @return True, if the is present value is valid, otherwise false.
     */
    bool hasValidIsPresent(QSharedPointer<ViewConfiguration> configuration) const;

    /*!
     *  Check if the view reference is valid.
     *
     *      @param [in] configuration   The selected view configuration.
     *
     *      @return True, if the view reference is valid, otherwise false.
     */
    bool hasValidViewReference(QSharedPointer<ViewConfiguration> configuration);

    /*!
     *  Locate errors within a view configuration.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected view configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<ViewConfiguration> configuration,
        QString const& context);

private:

	// Disable copying.
	ViewConfigurationValidator(ViewConfigurationValidator const& rhs);
	ViewConfigurationValidator& operator=(ViewConfigurationValidator const& rhs);

    /*!
     *  Change the currently available views.
     *
     *      @param [in] configuration   The selected view configuration.
     */
    void changeAvailableViews(QSharedPointer<ViewConfiguration> configuration);

    /*!
     *  Find errors in name.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected view configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<ViewConfiguration> configuration,
        QString const& context) const;

    /*!
     *  Find errors in is present value.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected view configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QSharedPointer<ViewConfiguration> configuration,
        QString const& context) const;

    /*!
     *  Find errors in view reference.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected view configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInViewReference(QVector<QString>& errors, QSharedPointer<ViewConfiguration> configuration,
        QString const& context);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used library handler.
    LibraryInterface* libraryHandler_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! The currently available views.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! The currently available component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > availableInstances_;
};

#endif // VIEWCONFIGURATIONVALIDATOR_H
