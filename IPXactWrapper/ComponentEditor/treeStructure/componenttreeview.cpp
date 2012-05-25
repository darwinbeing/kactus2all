/* 
 *
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeview.h"

#include "componenttreeitem.h"

#include <LibraryManager/libraryinterface.h>
#include <models/generaldeclarations.h>

#include <QEvent>
#include <QCursor>
#include <QApplication>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

ComponentTreeView::ComponentTreeView(LibraryInterface* handler,
									 const VLNV& compVLNV,
									 QWidget *parent):
QTreeView(parent),
pressedPoint_(),
locked_(true),
fileOpenAction_(tr("Open"), this),
handler_(handler),
componentVLNV_(compVLNV) {

	// can be used in debugging to identify the object
	setObjectName(tr("ComponentTreeView"));

	// header is not shown 
	setHeaderHidden(true);

	setIndentation(20);

	// only one item can be selected at a time
	setSelectionMode(QAbstractItemView::SingleSelection);

	setSelectionBehavior(QAbstractItemView::SelectItems);

	connect(&fileOpenAction_, SIGNAL(triggered()),
		this, SLOT(onFileOpen()), Qt::UniqueConnection);
	fileOpenAction_.setToolTip(tr("Open the file in operating system's default editor."));
	fileOpenAction_.setStatusTip(tr("Open the file in operating system's default editor."));
}

ComponentTreeView::~ComponentTreeView() {
}

void ComponentTreeView::currentChanged(const QModelIndex & current, 
									   const QModelIndex & previous ) 
{
	// call the base class implementation
	QTreeView::currentChanged(current, previous);

	// inform that the current index has changed
	emit activated(current);
}

void ComponentTreeView::keyPressEvent( QKeyEvent* event ) {
	QTreeView::keyPressEvent(event);
}

void ComponentTreeView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();
	QTreeView::mousePressEvent(event);
}

void ComponentTreeView::mouseReleaseEvent( QMouseEvent* event ) {

	// calculate the distance of the drag
	//int distance = (event->pos() - pressedPoint_).manhattanLength();

	// make sure widget is not locked
	if (!locked_) {

		QModelIndex pressedIndex = indexAt(pressedPoint_);

		QModelIndex releaseIndex = indexAt(event->pos());

		// if either index is invalid or indexes are the same
		if (!releaseIndex.isValid() || !pressedIndex.isValid() || 
			(releaseIndex == pressedIndex)) {
			
			// do the normal release event functionality
			QTreeView::mouseReleaseEvent(event);

			// restore the normal cursor
			setCursor(QCursor(Qt::ArrowCursor));
			return;
		}

		QModelIndex parent1 = pressedIndex.model()->parent(pressedIndex);
		QModelIndex parent2 = releaseIndex.model()->parent(releaseIndex);

		if (parent1.isValid() && parent1 == parent2) {
			emit moveItem(pressedIndex, releaseIndex);

			QModelIndex newIndex = indexAt(event->pos());
			setCurrentIndex(newIndex);
			emit activated(newIndex);
		}
	}

	// restore the normal cursor
	setCursor(QCursor(Qt::ArrowCursor));

	QTreeView::mouseReleaseEvent(event);
}

void ComponentTreeView::mouseMoveEvent( QMouseEvent* event ) {

	// if not dragging an item or item is locked
	if (event->buttons() == Qt::NoButton || locked_)
		QTreeView::mouseMoveEvent(event);
	
	// if dragging item
	else {
		QModelIndex startIndex = indexAt(pressedPoint_);
		QModelIndex thisIndex = indexAt(event->pos());

		// if either index is invalid then dragging is not possible
		if (!startIndex.isValid() || !thisIndex.isValid()) {
			setCursor(QCursor(Qt::ForbiddenCursor));
			return;
		}

		QModelIndex parent1 = startIndex.model()->parent(startIndex);
		QModelIndex parent2 = thisIndex.model()->parent(thisIndex);

		// if both have same parent
		if (parent1 == parent2) {
			setCursor(QCursor(Qt::ClosedHandCursor));
		}
		else {
			setCursor(QCursor(Qt::ForbiddenCursor));
		}
	}
}

void ComponentTreeView::setLocked( bool locked ) {
	locked_ = locked;
}

void ComponentTreeView::selectItem( const QModelIndex& index ) {
	selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void ComponentTreeView::contextMenuEvent( QContextMenuEvent* event ) {
	QModelIndex index = indexAt(event->pos());
	
	// save the position where click occurred
	pressedPoint_ = event->pos();

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(index.internalPointer());

	if (item->type() == ComponentTreeItem::FILE) {
		QMenu menu(this);
		menu.addAction(&fileOpenAction_);
		menu.exec(event->globalPos());
	}

	event->accept();
}

void ComponentTreeView::mouseDoubleClickEvent( QMouseEvent* event ) {
	QModelIndex index = indexAt(event->pos());

	// save the position where click occurred
	pressedPoint_ = event->pos();

	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(index.internalPointer());

	if (item->type() == ComponentTreeItem::FILE) {
		onFileOpen();
	}

	event->accept();
}


void ComponentTreeView::onFileOpen() {
	const QString xmlPath = handler_->getPath(componentVLNV_);
	
	QModelIndex index = indexAt(pressedPoint_);
	ComponentTreeItem* item = static_cast<ComponentTreeItem*>(index.internalPointer());
	Q_ASSERT(item->type() == ComponentTreeItem::FILE);

	// get the relative path of the file
	const QString relFilePath = item->text();

	// get the absolute file path to the file
	const QString absolutePath = General::getAbsolutePath(xmlPath, relFilePath);

	// open the file in operating system's default editor
	QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
}


