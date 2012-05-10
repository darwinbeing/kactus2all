/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILESETITEM_H
#define COMPONENTEDITORFILESETITEM_H

#include "componenteditoritem.h"
#include <IPXactWrapper/ComponentEditor/fileSet/fileseteditor.h>
#include <models/fileset.h>
#include <QSharedPointer>

/*! \brief The item for a single file set in the component editor's navigation tree.
 * 
 */
class ComponentEditorFileSetItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param fileSet Pointer to the file set being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileSetItem();

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
	ComponentEditorFileSetItem(const ComponentEditorFileSetItem& other);

	//! \brief No assignment
	ComponentEditorFileSetItem& operator=(const ComponentEditorFileSetItem& other);

	//! \brief Pointer to the file set being edited.
	QSharedPointer<FileSet> fileSet_;

	//! \brief The editor to edit the file set.
	FileSetEditor editor_;
};

#endif // COMPONENTEDITORFILESETITEM_H
