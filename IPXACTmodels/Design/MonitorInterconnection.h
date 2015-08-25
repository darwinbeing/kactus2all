//-----------------------------------------------------------------------------
// File: MonitorInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes the ipxact:monitorInterconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef MONITORINTERCONNECTION_H
#define MONITORINTERCONNECTION_H

#include "MonitorInterface.h"

#include <IPXACTmodels/common/NameGroup.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
// Describes the ipxact:monitorInterconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT MonitorInterconnection : public NameGroup
{
public:
    /*! The constructor
    *
    *       @param [in] interconnectionNode     A DOM node where the information should be parsed from.
    */
    //Interconnection(QDomNode& interconnectionNode);

    /*!
     *  The constructor.
     *
     *      @param [in] name                The name of the interconnection.
     *      @param [in] activeInterface     The monitored active interface.
     *      @param [in] displayName         Display name.
     *      @param [in] description         Description.
     */
    MonitorInterconnection(QString const name, QSharedPointer<MonitorInterface> activeInterface,
        QString const& displayName = QString(), QString const& description = QString());

    /*!
     *  Constructor.
     */
    MonitorInterconnection();

    /*!
     *  Copy constructor.
     */
    MonitorInterconnection(const MonitorInterconnection& other);

    /*!
     *  The destructor.
     */
    virtual ~MonitorInterconnection();

    /*!
     *  Assignment operator.
     */
    MonitorInterconnection& operator=(const MonitorInterconnection& other);

    /*! Check if the interconnection is in a valid state.
    * 
    *       @param [in] instanceNames       List of component instance names contained in the design.
    *       @param [in] errorList           The list to add the possible error messages to.
    *       @param [in] parentIdentifier    String from parent to help to identify the location of the error.
    *
    *       @return True if the state is valid and writing is possible.
    */
    //bool isValid(QStringList const& instanceNames, QStringList& errorList, QString const& parentIdentifier) const;

    /*! Check if the interconnection is in a valid state.
    * 
    *       @param [in] instanceNames   List of component instance names contained in the design.
    * 
    *       @return True if the state is valid and writing is possible.
    */
    //bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Get the monitored active interface.
     *
     *      @return Pointer to the monitored active interface.
     */
    QSharedPointer<MonitorInterface> getMonitoredActiveInterface() const;

    /*!
     *  Set the monitored active interface.
     *
     *      @param [in] newActiveInterface  The new monitored active interface.
     */
    void setMonitoredctiveInterface(QSharedPointer<MonitorInterface> newActiveInterface);

    /*!
     *  Get the monitor interfaces.
     *
     *      @return A pointer to a list of monitor interfaces.
     */
    QSharedPointer<QList<QSharedPointer<MonitorInterface> > > getMonitorInterfaces() const;

    /*!
     *  Set the monitor interfaces.
     *
     *      @param [in] newMonitorInterfaces    The new monitor interfaces.
     */
    void setMonitorInterfaces(QSharedPointer<QList<QSharedPointer<MonitorInterface> > > newMonitorInterfaces);

    /*!
     *  Get the presence.
     *
     *      @return The presence.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence.
     */
    void setIsPresent(QString const& newIsPresent);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The monitored active interface
    QSharedPointer<MonitorInterface> monitoredActiveInterface_;

    //! A list of monitor interfaces.
    QSharedPointer<QList<QSharedPointer<MonitorInterface> > > monitorInterfaces_;

    //! The presence.
    QString isPresent_;
};
#endif // MONITORINTERCONNECTION_H