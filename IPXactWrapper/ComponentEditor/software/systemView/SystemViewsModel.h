//-----------------------------------------------------------------------------
// File: SystemViewsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 13.7.2012
//
// Description:
// The model to manage system views for a table view.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEWSMODEL_H
#define SYSTEMVIEWSMODEL_H

#include <models/SystemView.h>
#include <models/component.h>

#include <QSortFilterProxyModel>
#include <QAbstractTableModel>

/*! \brief The model to manage the system views for a table view.
 *
 */
class SystemViewsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of the model
	 *
	*/
	SystemViewsModel(QSharedPointer<Component> component,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~SystemViewsModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Check if the software views model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! \brief Emitted when the contents of the model changes.
	void contentChanged();

	/*! \brief Emitted when a new software view is added to the model.
	 *
	 * \param index The index of the added software view.
	 *
	*/
	void viewAdded(int index);

	/*! \brief Emitted when a view is removed from the model.
	 *
	 * \param index The index of the software view to remove.
	 *
	*/
	void viewRemoved(int index);

private:
	
	//! \brief No copying
	SystemViewsModel(const SystemViewsModel& other);

	//! \brief No assignment
	SystemViewsModel& operator=(const SystemViewsModel& other);

	//! \brief Contains the software views to edit.
	QList<QSharedPointer<SystemView> >& views_;
};

#endif // SYSTEMVIEWSMODEL_H
