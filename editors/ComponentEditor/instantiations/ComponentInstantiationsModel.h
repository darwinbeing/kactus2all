//-----------------------------------------------------------------------------
// File: ComponentInstantiationsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for component instantiations summary.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANTIATIONSMODEL_H
#define COMPONENTINSTANTIATIONSMODEL_H

#include <IPXACTmodels/Component/Component.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

class InstantiationsValidator;

//-----------------------------------------------------------------------------
//! Model for component instantiations summary.
//-----------------------------------------------------------------------------
class ComponentInstantiationsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] component       The component being edited.
     *      @param [in] validator       The validator used for component instances.
	 *      @param [in] parent          Pointer to the owner of the model.
	 */
	ComponentInstantiationsModel(QSharedPointer<Component> component, 
        QSharedPointer<InstantiationsValidator> validator, QObject* parent);
	
	//! The destructor.
	virtual ~ComponentInstantiationsModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
	 *
	 *      @return The requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *      @return The data for the item.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True, if saving happened successfully, otherwise false.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

	//! Emitted when a new component instantiation is added to the given index.
	void componentInstantiationAdded(int index);

	//! Emitted when a component instantiation is removed from the given index.
	void componentInstantiationRemoved(int index);

private:
	//! No copying.
	ComponentInstantiationsModel(const ComponentInstantiationsModel& other);
	ComponentInstantiationsModel& operator=(const ComponentInstantiationsModel& other);

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the component instantiations to manage.
    QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations_;

    //! The validator used for component instantiations.
    QSharedPointer<InstantiationsValidator> validator_;
};

#endif // COMPONENTINSTANTIATIONSMODEL_H
