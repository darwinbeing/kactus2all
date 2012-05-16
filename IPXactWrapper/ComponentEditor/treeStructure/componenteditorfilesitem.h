/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILESITEM_H
#define COMPONENTEDITORFILESITEM_H

#include "componenteditoritem.h"
#include <models/file.h>

/*! \brief The files-item in the component editor's navigation tree under file set.
 *
 */
class ComponentEditorFilesItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param files QList containing the files to edit.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFilesItem(QList<QSharedPointer<File> >& files,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFilesItem();

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
	ComponentEditorFilesItem(const ComponentEditorFilesItem& other);

	//! \brief No assignment
	ComponentEditorFilesItem& operator=(const ComponentEditorFilesItem& other);

	//! \brief The files being edited.
	QList<QSharedPointer<File> >& files_;
};

#endif // COMPONENTEDITORFILESITEM_H
