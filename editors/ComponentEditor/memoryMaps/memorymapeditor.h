/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapeditor.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPEDITOR_H
#define MEMORYMAPEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/memorymap.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class EditableTableView;
class MemoryMapModel;
class MemoryMapProxy;
class LibraryInterface;

/*! \brief The editor to edit the details of a single memory map.
 *
 */
class MemoryMapEditor : public ItemEditor {
	Q_OBJECT

public:

	//! \brief The default height and width of the editor.
	enum Sizes {
		HEIGHT = 300,
		WIDTH = 700
	};
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
	 *      @param [in] memoryMap               Pointer to the memory map being edited.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the parent of this editor.
	 */
	MemoryMapEditor(QSharedPointer<Component> component,
		LibraryInterface* handler, 
		QSharedPointer<MemoryMap> memoryMap,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~MemoryMapEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief The size hint for the editor.
	 *
	 * \return QSize contains the size hint.
	*/
	virtual QSize sizeHint() const;

protected:

	//! \brief Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:
	
	//! \brief No copying
	MemoryMapEditor(const MemoryMapEditor& other);

	//! \brief No assignment
	MemoryMapEditor& operator=(const MemoryMapEditor& other);

	//! \brief The view to show the details of a memory map.
	EditableTableView* view_;

	//! \brief The proxy that does the sorting of items.
	MemoryMapProxy* proxy_;

	//! \brief The model that manages the items.
	MemoryMapModel* model_;
};

#endif // MEMORYMAPEDITOR_H
