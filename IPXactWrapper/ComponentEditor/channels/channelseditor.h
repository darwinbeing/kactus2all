/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: channelseditor.h
 *		Project: Kactus 2
 */

#ifndef CHANNELSEDITOR_H
#define CHANNELSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>
#include <models/component.h>
#include <common/views/EditableTableView/editabletableview.h>
#include "channelsmodel.h"

#include <QSharedPointer>
#include <QSortFilterProxyModel>

/*! \brief The editor to add/remove/edit the channels of a component.
 *
 */
class ChannelsEditor : public ItemEditor {
	Q_OBJECT

public:
	
	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent of this editor.
	 *
	*/
	ChannelsEditor(QSharedPointer<Component> component,
				QWidget* parent = 0);
	
	//! \brief The destructor
	virtual ~ChannelsEditor();

	/*! \brief Check for the validity of the editor
	* 
	* \return True if the editor is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the editor to the IPXact model.
	 *
	*/
	virtual void makeChanges();

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

private:

	//! \brief No copying
	ChannelsEditor(const ChannelsEditor& other);

	//! \brief No assignment
	ChannelsEditor& operator=(const ChannelsEditor& other);

	//! \brief The view that displays the channels.
	EditableTableView view_;

	//! \brief The proxy that does the editing.
	QSortFilterProxyModel proxy_;

	//! \brief The model that manages the channels being edited.
	ChannelsModel model_;
};

#endif // CHANNELSEDITOR_H
