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

#include <QSharedPointer>
#include <QList>

class MemoryMapEditor;
class MemoryMapsVisualizer;

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
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorMemMapItem(QSharedPointer<MemoryMap> memoryMap,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorMemMapItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

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

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 * 
	 * \return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Set the visualizer for this item.
	 *
	 * \param visualizer Pointer to the visualizer.
	 *
	*/
	virtual void setVisualizer(MemoryMapsVisualizer* visualizer);

private:
	//! \brief No copying
	ComponentEditorMemMapItem(const ComponentEditorMemMapItem& other);

	//! \brief No assignment
	ComponentEditorMemMapItem& operator=(const ComponentEditorMemMapItem& other);

	//! \brief Pointer to the memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

	//! \brief Contains the address blocks that are children of this tree item.
	QList<QSharedPointer<MemoryMapItem> >& items_;

	//! \brief The editor to edit the details of a single memory map.
	MemoryMapEditor* editor_;

	//! \brief The visualizer to display the memory maps
	MemoryMapsVisualizer* visualizer_;
};

#endif // COMPONENTEDITORMEMMAPITEM_H
