/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfacesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORAPIINTERFACESITEM_H
#define COMPONENTEDITORAPIINTERFACESITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/ApiInterface.h>

#include <QList>

/*! The API interfaces-item in the component editor's navigation tree.
 *
 */
class ComponentEditorAPIInterfacesItem : public ComponentEditorItem {
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
	ComponentEditorAPIInterfacesItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorAPIInterfacesItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* The font is bold, if API interfaces exist, otherwise not bold.
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

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Add a new child to the item.
	 *
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

private:
	//! \brief No copying
	ComponentEditorAPIInterfacesItem(const ComponentEditorAPIInterfacesItem& other);

	//! \brief No assignment
	ComponentEditorAPIInterfacesItem& operator=(const ComponentEditorAPIInterfacesItem& other);

	//! \brief Contains the api interfaces to edit.
	QList<QSharedPointer<ApiInterface> >& apiInterfaces_;
};

#endif // COMPONENTEDITORAPIINTERFACESITEM_H
