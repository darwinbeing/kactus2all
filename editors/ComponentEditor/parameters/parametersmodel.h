//-----------------------------------------------------------------------------
// File: parametersmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.4.2011
//
// Description:
// Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSMODEL_H
#define PARAMETERSMODEL_H

#include <IPXACTmodels/parameter.h>

#include <QAbstractTableModel>

#include <QSharedPointer>
#include <QList>

class Choice;
class Component;

//-----------------------------------------------------------------------------
//! Table model that can be used to display parameters to be edited.
//-----------------------------------------------------------------------------
class ParametersModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parameters The QList containing pointers to the parameters.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	ParametersModel(QList<QSharedPointer<Parameter> >& parameters, 
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~ParametersModel();

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Always returns 3
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * \param section The column that's header is wanted
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specified the type of data that is wanted.
	 *
	 * \return QVariant containing the data to be displayed
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified item.
	 *
	 * \param index Specifies the item that's data is modified
	 * \param value The value to be set.
	 * \param role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * \return True if data was successfully set.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*! \brief Get information on how specified item can be handled.
	 *
	 * \param index Specifies the item that's flags are wanted.
	 *
	 * \return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

    /*!
     *  \brief Check if the model is in valid state or not.
     *
     *      \param [inout] errorList           The list to add the possible error messages to.
     *      \param [in]    parentIdentifier    String from parent to help to identify the location of the error.
     *
     *      \return True if all items in model are valid.
     */
    bool isValid(QStringList& errorList, const QString& parentIdentifier) const;

public slots:

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void onAddItem(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void onRemoveItem(const QModelIndex& index);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

protected:

    /*!
     *  Finds the value for the given model parameter using either selected choice or model parameter value.
     *
     *      @param [in] modelParameter   The model parameter whose value to find.
     *
     *      @return The value to display for the model parameter.
     */
    QString evaluateValueFor(QSharedPointer<Parameter> modelParameter) const;

    /*!
     *  Finds the choice with the given name.
     *
     *      @param [in] choiceName   The name of the choice to find.
     *
     *      @return The found choice.
     */
    QSharedPointer<Choice> findChoice(QString const& choiceName) const;

    /*!
     *  Finds a human-readable value to display for a given enumeration.
     *
     *      @param [in] choice              The choice whose enumeration to find.
     *      @param [in] enumerationValue    The value used to search the enumeration.
     *
     *      @return A value for the enumeration to display.
     */
    QString findDisplayValueForEnumeration(QSharedPointer<Choice> choice, QString const& enumerationValue) const;

private:

	//! \brief No copying
	ParametersModel(const ParametersModel& other);

	//! No assignment
	ParametersModel& operator=(const ParametersModel& other);
	
	//! \brief Pointer to the component that's parameters are edited.
	QList<QSharedPointer<Parameter> >& parameters_;

    //! The choices available for model parameter values.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

};

#endif // PARAMETERSMODEL_H
