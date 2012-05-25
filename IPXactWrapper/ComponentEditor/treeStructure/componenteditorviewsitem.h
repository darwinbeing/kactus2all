/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorviewsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORVIEWSITEM_H
#define COMPONENTEDITORVIEWSITEM_H

#include "componenteditoritem.h"

/*! \brief The Views-item in the component editor's navigation tree.
 *
 */
class ComponentEditorViewsItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorViewsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorViewsItem();

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

private:
	//! \brief No copying
	ComponentEditorViewsItem(const ComponentEditorViewsItem& other);

	//! \brief No assignment
	ComponentEditorViewsItem& operator=(const ComponentEditorViewsItem& other);

	//! \brief The views being edited.
	QList<QSharedPointer<View> >& views_;
};

#endif // COMPONENTEDITORVIEWSITEM_H
