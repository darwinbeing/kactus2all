/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapdelegate.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPDELEGATE_H
#define MEMORYMAPDELEGATE_H

#include <QStyledItemDelegate>

/*! \brief The delegate that provides editors to add/remove/edit details of a single memory map.
 *
 */
class MemoryMapDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief Contains the columns for the table.
	enum Column {
		USAGE_COLUMN = 0,
		NAME_COLUMN,
		BASE_COLUMN,
		RANGE_COLUMN,
		WIDTH_COLUMN,
		DESCRIPTION_COLUMN,
		ACCESS_COLUMN,
		VOLATILE_COLUMN,
// 		LATENCY_COLUMN,
// 		BANDWIDTH_COLUMN,
		COLUMN_COUNT
	};
	
	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner the delegate.
	 *
	*/
	MemoryMapDelegate(QObject *parent);
	
	//! \brief The destructor.
	virtual ~MemoryMapDelegate();

	/*! \brief Create a new editor for the given item
	 *
	 * \param parent Owner for the editor.
	 * \param option Contains options for the editor.
	 * \param index Model index identifying the item.
	 *
	 * \return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, 
		const QStyleOptionViewItem& option, 
		const QModelIndex& index) const;

	/*! \brief Set the data for the editor.
	 *
	 * \param editor Pointer to the editor where the data is to be set.
	 * \param index Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*! \brief Save the data from the editor to the model.
	 *
	 * \param editor Pointer to the editor that contains the data to store.
	 * \param model Model that contains the data structure where data is to be saved to.
	 * \param index Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, 
		const QModelIndex& index) const;

private slots:

	/*! \brief Commit the data from the sending editor and close the editor.
	 *
	*/
	void commitAndCloseEditor();

private:
	
	//! \brief No copying
	MemoryMapDelegate(const MemoryMapDelegate& other);

	//! \brief No assignment
	MemoryMapDelegate& operator=(const MemoryMapDelegate& other);
};

#endif // MEMORYMAPDELEGATE_H
