/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: otherclockdriverseditor.h
 */

#ifndef OTHERCLOCKDRIVERSEDITOR_H
#define OTHERCLOCKDRIVERSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include "otherclockdriversmodel.h"

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;

/*! \brief Widget to edit the otherClockDrivers of a component.
 *
 */
class OtherClockDriversEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	OtherClockDriversEditor(QSharedPointer<Component> component,
		QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~OtherClockDriversEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private:
	
	//! \brief No copying
	OtherClockDriversEditor(const OtherClockDriversEditor& other);

	//! No assignment
	OtherClockDriversEditor& operator=(const OtherClockDriversEditor& other);

	//! \brief The view that displays the parameters.
	EditableTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	OtherClockDriversModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

#endif // OTHERCLOCKDRIVERSEDITOR_H
