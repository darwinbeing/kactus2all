/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: cpusdelegate.h
 *		Project: Kactus 2
 */

#ifndef CPUSDELEGATE_H
#define CPUSDELEGATE_H

#include <IPXACTmodels/component.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>

/*! \brief The delegate to provide editors to add/remove/edit the cpus of a component.
 *
 */
class CpusDelegate : public QStyledItemDelegate {
	Q_OBJECT

public:

	//! \brief The minimum height for list editor for address space references.
	static const int LIST_EDITOR_MIN_HEIGHT = 100;

	//! \brief Contains the numbers for columns.
	enum Column {
		NAME_COLUMN = 0,
		DISPLAY_NAME_COLUMN,
		ADDRSPACE_COLUMN,
		DESCRIPTION_COLUMN,
		COLUMN_COUNT
	};

	//! \brief The roles that can operate on QStringList on address space column
	enum Role {
		USER_DISPLAY_ROLE = Qt::UserRole,
		USER_EDIT_ROLE
	};

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of the delegate.
	 *
	*/
	CpusDelegate(QSharedPointer<Component> component,
		QObject *parent);
	
	//! \brief The destructor.
	virtual ~CpusDelegate();

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
	CpusDelegate(const CpusDelegate& other);

	//! \brief No assignment
	CpusDelegate& operator=(const CpusDelegate& other);

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // CPUSDELEGATE_H
