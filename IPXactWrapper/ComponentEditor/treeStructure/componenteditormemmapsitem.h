/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormemmapsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORMEMMAPSITEM_H
#define COMPONENTEDITORMEMMAPSITEM_H

#include "componenteditoritem.h"

class MemoryMapsEditor;

/*! \brief The Memory maps-item in the component navigation tree.
 *
 */
class ComponentEditorMemMapsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorMemMapsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapsItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const;

	/*! \brief Add a new child to the item.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

private:
	//! \brief No copying
	ComponentEditorMemMapsItem(const ComponentEditorMemMapsItem& other);

	//! \brief No assignment
	ComponentEditorMemMapsItem& operator=(const ComponentEditorMemMapsItem& other);

	//! \brief Contains the memory maps being edited.
	QList<QSharedPointer<MemoryMap> >& memoryMaps_;

	//! \brief The editor to manage the memory maps.
	MemoryMapsEditor* editor_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // COMPONENTEDITORMEMMAPSITEM_H
