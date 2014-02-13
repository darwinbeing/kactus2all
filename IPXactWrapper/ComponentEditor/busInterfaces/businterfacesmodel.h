/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfacesmodel.h
 *		Project: Kactus 2
 */

#ifndef BUSINTERFACESMODEL_H
#define BUSINTERFACESMODEL_H

#include <models/component.h>
#include <models/businterface.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class LibraryInterface;

/*! \brief The model that manages the objects for BusInterfacesEditor.
 *
 */
class BusInterfacesModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	BusInterfacesModel(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~BusInterfacesModel();

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

	/*! \brief Check if the bus interfaces model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

    /*!
     *  Returns the supported actions of a drop.
     *
     *      @return The drop actions supported by the model.
     */
    Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *      @return The supported MIME types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *      @param [in] data   The data associated to the drop.
     *      @param [in] action The drop action.  
     *      @param [in] row    The row beneath the drop position.
     *      @param [in] column The column beneath the drop position.
     *      @param [in] parent The parent index of the drop position.
     *
     *      @return True, if the model could handle the data, otherwise false.
     */
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
        const QModelIndex &parent);

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

	/*! \brief Move item from the original position to new position.
	 *
	 * \param originalPos The index identifying the item to move.
	 * \param newPos      The index identifying the position of the item after the move.
     *
	*/
    virtual void onMoveItem( const QModelIndex& originalPos, const QModelIndex& newPos );

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	/*! \brief Emitted when a new bus interface is added to the model.
	 *
	 * \param index The index of the added bus interface.
	 *
	*/
	void busifAdded(int index);

	/*! \brief Emitted when a bus interface is removed from the model.
	 *
	 * \param index The index of the bus interface to remove.
	 *
	*/
	void busifRemoved(int index);

    /*! \brief Emitted when a bus interface is moved from one position to another.
     *
     * \param source The row number of the bus interface before the move.
     * \param target The row number of the bus interface after the move.
     *
     */
    void busIfMoved(int source, int target);

private:

	//! \brief No copying
	BusInterfacesModel(const BusInterfacesModel& other);

	//! \brief No assignment
	BusInterfacesModel& operator=(const BusInterfacesModel& other);
   
	//! \brief The instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief The bus interfaces being edited.
	QList<QSharedPointer<BusInterface> >& busifs_;

};

#endif // BUSINTERFACESMODEL_H
