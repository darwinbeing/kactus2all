/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditortreemodel.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORTREEMODEL_H
#define COMPONENTEDITORTREEMODEL_H

#include "componenteditoritem.h"

#include <QAbstractItemModel>

class ComponentEditorTreeModel : public QAbstractItemModel {
	Q_OBJECT

public:

	ComponentEditorTreeModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ComponentEditorTreeModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Does the specified item have any child items
     *
     * \param parent Model index of the item.
     *
     * \return True if the item has any child items. This function is implemented
     * for performance reasons.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get index that identifies the given item.
	 *
	 * \param row The row of the item.
	 * \param column The column of the item.
	 * \param parent The parent index of the item.
	 *
	 * \return QModelIndex The model index that identifies the item.
	*/
	virtual QModelIndex index(int row, int column, 
		const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the index that identifies the given item.
	 *
	 * \param item Pointer to the item that's index is wanted.
	 *
	 * \return QModelIndex The model index that identifies the item.
	*/
	virtual QModelIndex index(ComponentEditorItem* item) const;

	/*! \brief Get the index of the parent of given index.
	 *
	 * \param index The index that identifies the item that's parent is wanted.
	 *
	 * \return QModelIndex The model index that identifies the parent.
	*/
	virtual QModelIndex parent(const QModelIndex& index) const;

public slots:

	/*! \brief Handler for component editor item's content changed signal.
	 *
	 * \param item Pointer to the item that changed.
	 *
	*/
	void onContentChanged(ComponentEditorItem* item);

private:
	//! \brief No copying
	ComponentEditorTreeModel(const ComponentEditorTreeModel& other);

	//! \brief No assignment
	ComponentEditorTreeModel& operator=(const ComponentEditorTreeModel& other);

	//! \brief Pointer to the root item of the tree.
	QSharedPointer<ComponentEditorItem> rootItem_;
};

#endif // COMPONENTEDITORTREEMODEL_H
