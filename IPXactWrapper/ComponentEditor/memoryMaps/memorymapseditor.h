/* 
 *  	Created on: 11.4.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapseditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSEDITOR_H
#define MEMORYMAPSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>

#include <QWidget>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class LibraryInterface;
class EditableTableView;
class MemoryMapsModel;

/*! \brief Editor to edit the memory maps of a component.
 *
 */
class MemoryMapsEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	MemoryMapsEditor(QSharedPointer<Component> component,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapsEditor();

	/*! \brief Check for the validity of the edited memory maps.
	*
	* \return True if all memory maps are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:
	
	//! \brief No copying
	MemoryMapsEditor(const MemoryMapsEditor& other);

	//! \brief No assignment
	MemoryMapsEditor& operator=(const MemoryMapsEditor& other);

	//! \brief The view to display the table.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	QSortFilterProxyModel* proxy_;

	//! \brief The model that manages the memory maps.
	MemoryMapsModel* model_;
};

#endif // MEMORYMAPSEDITOR_H
