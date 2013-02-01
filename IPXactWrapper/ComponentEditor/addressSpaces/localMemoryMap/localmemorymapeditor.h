/* 
 *  	Created on: 13.10.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapeditor.h
 *		Project: Kactus 2
 */

#ifndef LOCALMEMORYMAPEDITOR_H
#define LOCALMEMORYMAPEDITOR_H

#include <models/memorymap.h>
#include <models/component.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class NameGroupEditor;
class EditableTableView;
class MemoryMapModel;
class MemoryMapProxy;
class LibraryInterface;

/*! \brief LocalMemoryMapEditor is used to edit a local memory map of an address space.
 *
 */
class LocalMemoryMapEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param memoryMap Pointer to the local memory map being edited.
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance managing the library.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	LocalMemoryMapEditor(QSharedPointer<MemoryMap> memoryMap,
		QSharedPointer<Component> component,
		LibraryInterface* handler,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~LocalMemoryMapEditor();

	/*! \brief Check for the validity of the editor.
	* 
	* \return True if local memory map is valid.
	*/
	bool isValid() const;

	//! \brief Reload the information from the model to the editor.
	void refresh();

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	//! \brief Emitted when a new memory map item is added to the given index.
	void itemAdded(int index);

	//! \brief Emitted when a memory map item is removed from the given index.
	void itemRemoved(int index);

private:
	
	//! \brief No copying
	LocalMemoryMapEditor(const LocalMemoryMapEditor& other);

	//! \brief No assignment
	LocalMemoryMapEditor& operator=(const LocalMemoryMapEditor& other);

	//! \brief Pointer to the local memory map being edited.
	QSharedPointer<MemoryMap> localMemoryMap_;

	//! \brief Used to edit the name, display name and description.
	NameGroupEditor* nameEditor_;

	//! \brief The view to display the table of local memory address blocks
	EditableTableView* view_;

	//! \brief The proxy that does the sorting
	MemoryMapProxy* proxy_;

	//! \brief The model that manages the items.
	MemoryMapModel* model_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance managing the library.
	LibraryInterface* handler_;
};

#endif // LOCALMEMORYMAPEDITOR_H
