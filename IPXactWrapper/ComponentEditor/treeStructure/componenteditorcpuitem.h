/* 
 *  	Created on: 15.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpuitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORCPUITEM_H
#define COMPONENTEDITORCPUITEM_H

#include "componenteditoritem.h"
#include <models/cpu.h>
#include <IPXactWrapper/ComponentEditor/cpus/cpueditor.h>

#include <QSharedPointer>

/*! \brief The item for a single cpu in component editor's navigation tree.
 *
 */
class ComponentEditorCpuItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param cpu Pointer to the cpu being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param widget Pointer to the QWidget that displays the editor.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorCpuItem(QSharedPointer<Cpu> cpu,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		QWidget* widget,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorCpuItem();

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
	ComponentEditorCpuItem(const ComponentEditorCpuItem& other);

	//! \brief No assignment
	ComponentEditorCpuItem& operator=(const ComponentEditorCpuItem& other);

	//! \brief Pointer to the cpu being edited.
	QSharedPointer<Cpu> cpu_;

	//! \brief The editor to edit the cpu.
	CpuEditor editor_;
};

#endif // COMPONENTEDITORCPUITEM_H
