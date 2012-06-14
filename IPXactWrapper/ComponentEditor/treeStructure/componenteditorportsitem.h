/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORPORTSITEM_H
#define COMPONENTEDITORPORTSITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/ports/portseditor.h>
#include <models/port.h>

/*! \brief The Ports-item in the component editor's navigation tree.
 *
 */
class ComponentEditorPortsItem : public ComponentEditorItem {
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
	ComponentEditorPortsItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorPortsItem();

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

private:
	//! \brief No copying
	ComponentEditorPortsItem(const ComponentEditorPortsItem& other);

	//! \brief No assignment
	ComponentEditorPortsItem& operator=(const ComponentEditorPortsItem& other);

	//! \brief Contains the ports to edit
	QMap<QString, QSharedPointer<Port> >& ports_;

	//! \brief The editor to edit the ports
	PortsEditor editor_;
};

#endif // COMPONENTEDITORPORTSITEM_H
