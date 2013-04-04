/* 
 *  	Created on: 10.12.2011
 *      Author: Antti Kamppi
 * 		filename: editabletableview.h
 *		Project: Kactus 2
 */

#ifndef EDITABLETABLEVIEW_H
#define EDITABLETABLEVIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QPoint>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

/*! \brief This view can be used to display contents of a table model.
 * 
 * This view supports adding and removing items and also moving them within the
 * view.
 */
class EditableTableView : public QTableView {
	Q_OBJECT

public:

	//! \brief The minimum height for the view.
	static const int MINIMUM_TABLE_HEIGHT = 100;

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this view.
	 *
	*/
	EditableTableView(QWidget *parent);
	
	//! \brief The destructor
	virtual ~EditableTableView();

	/*! \brief Enable/disable the import/export csv file functionality.
	 *
	 * Method: 		setAllowImportExport
	 * Full name:	EditableTableView::setAllowImportExport
	 * Access:		public 
	 *
	 * \param allow If true then import/export is enabled.
	 *
	*/
	void setAllowImportExport(bool allow);

	/*! \brief Set the model class for this view.
	 *
	 * Method: 		setModel
	 * Full name:	EditableTableView::setModel
	 * Access:		virtual public 
	 *
	 * \param model Pointer to the model class which is displayed by this view.
	 *
	*/
	virtual void setModel(QAbstractItemModel* model);

public slots:

	/*! \brief Set the items in the view to be draggable or not.
	 *
	 * \param draggable If true the positions of the items can change by dragging.
	 *
	*/
	void setItemsDraggable(bool draggable);

	/*! \brief Create a csv-file of the editor contents.
	 *
	 * \param filePath Contains an absolute file path to the file to create.
	 *
	*/
	void onCSVExport(const QString& filePath = QString());

	/*! \brief Import contents of a csv-file to the editor.
	 *
	 * \param filePath The path to the csv-file to import.
	 *
	*/
	void onCSVImport(const QString& filePath = QString());

	/*! \brief Set the default path to use for import/export csv.
	 *
	 * \param path The path to use as default.
	 *
	*/
	void setDefaultImportExportPath(const QString& path);

signals:

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void addItem(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void removeItem(const QModelIndex& index);

	/*! \brief Move item to another position.
	 *
	 * \param originalPos Identifies the item that should be moved.
	 * \param newPos The new position the item should be moved to.
	 *
	*/
	void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

protected:

	//! \brief Handler for mouse move events
	virtual void mouseMoveEvent(QMouseEvent* e);

	//! \brief Handler for key press events
	virtual void keyPressEvent(QKeyEvent* event);

	//! \brief Handler for mouse double click events
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

	//! \brief Handler for mouse press events
	virtual void mousePressEvent(QMouseEvent* event);

	//! \brief Handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent* event);

	//! \brief Handler for context menu events
	virtual void contextMenuEvent(QContextMenuEvent* event);

	//! \brief The point where mouse was clicked
	QPoint pressedPoint_;

	//! \brief Action to add new item
	QAction addAction_;

	//! \brief Action to remove item
	QAction removeAction_;

	//! \brief Action to copy item
	QAction copyAction_;

	//! \brief Action to paste item
	QAction pasteAction_;

	//! \brief Action to clear selected cells
	QAction clearAction_;

	//! \brief Action to import a csv-file
	QAction importAction_;

	//! \brief Action to export a csv-file
	QAction exportAction_;

	//! \brief Specified if the items can be imported/exported to a csv file.
	bool impExportable_;

protected slots:

	//! \brief Handler for add action
	virtual void onAddAction();

	//! \brief Handler for remove action
	virtual void onRemoveAction();

	//! \brief Handler for copy action.
	virtual void onCopyAction();

	//! \brief Handler for paste action
	virtual void onPasteAction();

	//! \brief Handler for cell clear action
	virtual void onClearAction(); 

private:
	//! \brief No copying
	EditableTableView(const EditableTableView& other);

	//! \brief No assignment
	EditableTableView& operator=(const EditableTableView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

	//! \brief The default path to use for import/export csv
	QString defImportExportPath_;

	//! \brief Specifies if the items in the view can be dragged from position to another.
	bool itemsDraggable_;
};

#endif // EDITABLETABLEVIEW_H
