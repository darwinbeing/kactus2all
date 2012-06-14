/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswviewsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORSWVIEWSITEM_H
#define COMPONENTEDITORSWVIEWSITEM_H

#include "componenteditoritem.h"

/*! \brief The Software Views-item in the component editor's navigation tree.
 *
 */
class ComponentEditorSWViewsItem : public ComponentEditorItem {
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
	ComponentEditorSWViewsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorSWViewsItem();

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

private:
	//! \brief No copying
	ComponentEditorSWViewsItem(const ComponentEditorSWViewsItem& other);

	//! \brief No assignment
	ComponentEditorSWViewsItem& operator=(const ComponentEditorSWViewsItem& other);

	//! \brief Contains the software views being edited.
	QList<QSharedPointer<SWView> >& swViews_;
};

#endif // COMPONENTEDITORSWVIEWSITEM_H
