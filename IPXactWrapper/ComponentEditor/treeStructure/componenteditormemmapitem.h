/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMEMMAPITEM_H
#define COMPONENTEDITORMEMMAPITEM_H

#include "componenteditoritem.h"
#include <models/memorymap.h>

/*! \brief The item for a single memory map in component editor's navigation tree.
 *
 */
class ComponentEditorMemMapItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param memoryMap Pointer to the memory map being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapItem();

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

private:
	//! \brief No copying
	ComponentEditorMemMapItem(const ComponentEditorMemMapItem& other);

	//! \brief No assignment
	ComponentEditorMemMapItem& operator=(const ComponentEditorMemMapItem& other);

	//! \brief Pointer to the memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;
};

#endif // COMPONENTEDITORMEMMAPITEM_H
