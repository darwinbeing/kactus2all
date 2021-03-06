/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfaceitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCOMINTERFACEITEM_H
#define COMPONENTEDITORCOMINTERFACEITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/ComInterface.h>

/*! \brief The item for a single COM interface in component editor's navigation tree.
 *
 */
class ComponentEditorComInterfaceItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param comInterface Pointer to the COM interface being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorComInterfaceItem();

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

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * If the com interface contains valid com definition reference then it can be opened.
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	/*! \brief Open the COM definition in an editor.
	 * 
	*/
	virtual void openItem();

private:
	//! \brief No copying
	ComponentEditorComInterfaceItem(const ComponentEditorComInterfaceItem& other);

	//! \brief No assignment
	ComponentEditorComInterfaceItem& operator=(const ComponentEditorComInterfaceItem& other);

	//! \brief Pointer to the com interface being edited.
	QSharedPointer<ComInterface> interface_;

    //! Action to open the COM interface for editing.
    QAction* editAction_;
};

#endif // COMPONENTEDITORCOMINTERFACEITEM_H
