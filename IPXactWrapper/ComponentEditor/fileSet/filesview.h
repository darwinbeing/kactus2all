/* 
 *  	Created on: 4.6.2012
 *      Author: Antti Kamppi
 * 		filename: filesview.h
 *		Project: Kactus 2
 */

#ifndef FILESVIEW_H
#define FILESVIEW_H

#include <common/views/EditableTableView/editabletableview.h>
#include <QMouseEvent>

/*! \brief The view to display the files contained in a file set.
 *
 */
class FilesView : public EditableTableView {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this view.
	 *
	*/
	FilesView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~FilesView();

signals:

	/*! \brief A new file should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 * \param filePath Path to the file that is to be added.
	 *
	*/
	void addItem(const QModelIndex& index, const QString& filePath);

protected:

	//! \brief Handler for mouse double click events
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private slots:

	//! \brief Handler for add action
	virtual void onAddAction();

private:
	
	//! \brief No copying
	FilesView(const FilesView& other);

	//! \brief No assignment
	FilesView& operator=(const FilesView& other);
};

#endif // FILESVIEW_H
