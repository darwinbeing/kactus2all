/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditor.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <models/component.h>
#include "componenttreeview.h"
#include "componenteditortreemodel.h"
#include "componenteditorgroupslot.h"

#include <QSharedPointer>
#include <QSplitter>

class LibraryInterface;

/*! \brief The editor to edit/packet IP-Xact components.
 *
 */
class ComponentEditor : public TabDocument {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Parent widget of the editor.
	 *
	*/
	ComponentEditor(LibraryInterface* libHandler, 
		QSharedPointer<Component> component, 
		QWidget *parent);

	//! \brief The destructor
	~ComponentEditor();

	/*! \brief Set the protection state of the component.
	 *
	 * \param locked The locked state to set.
	 *
	*/
	virtual void setProtection(bool locked);

	/*! \brief Get the vlnv of the current component.
	 *
	 * \return VLNV of the component being edited.
	*/
	virtual VLNV getComponentVLNV() const;

	/*! \brief Check if the editor is editing a hardware implementation or not.
	 *
	 * \return Component editor always returns true.
	*/
	virtual bool isHWImplementation() const;

	//! \brief Refreshes the editor to display the changes made.
    virtual void refresh();

public slots:

    /*! \brief Validates the document against the IP-XACT standard.
     *
     * \param errorList Error message list for reporting standard violations.
     *
     * \return True if the document is valid. False if there were any violations.
    */
    virtual bool validate(QStringList& errorList);

	//! \brief Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

	/*! \brief Run vhdl generator for the component.
	 *
	 * \return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onVhdlGenerate();

	/*! \brief Run modelsim generator for the component.
	 *
	 * \return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onModelsimGenerate();

private slots:

	/*! \brief This slot is called when an item is selected in the navigation tree.
	 *
	 * \param index The index identifying the item.
	 *
	*/
	void onItemActivated(const QModelIndex& index);

private:
	//! \brief No copying
	ComponentEditor(const ComponentEditor& other);

	//! \brief No assignment
	ComponentEditor& operator=(const ComponentEditor& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief The component being edited.
	QSharedPointer<Component> component_;

	//! \brief The tree view to navigate in the editor.
	ComponentTreeView navigationView_;

	//! \brief The model to control the navigation view.
	ComponentEditorTreeModel navigationModel_;

	//! \brief The splitter to contain the navigation tree.
	QSplitter navigationSplitter_;

	//! \brief The splitter to display the editors and visualizers.
	QSplitter editorVisualizerSplitter;

	//! \brief The slot to display the editors in.
	ComponentEditorGroupSlot editorSlot_;

	//! \brief The slot to display the visualizers in.
	ComponentEditorGroupSlot visualizerSlot_;
};

#endif // COMPONENTEDITOR_H
