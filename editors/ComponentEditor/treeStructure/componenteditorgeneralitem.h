/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgeneralitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORGENERALITEM_H
#define COMPONENTEDITORGENERALITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/component.h>

#include <QSharedPointer>
#include <QString>

class ComponentEditorTreeModel;
class ItemEditor;

/*! \brief The general-item in the component editor navigation tree.
 *
 */
class ComponentEditorGeneralItem : public ComponentEditorItem {
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
	ComponentEditorGeneralItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorGeneralItem();

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

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
    virtual QFont getFont() const;

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
	ComponentEditorGeneralItem(const ComponentEditorGeneralItem& other);

	//! \brief No assignment
	ComponentEditorGeneralItem& operator=(const ComponentEditorGeneralItem& other);

};

#endif // COMPONENTEDITORGENERALITEM_H
