/* 
 *  	Created on: 10.12.2011
 *      Author: Antti Kamppi
 * 		filename: editabletableview.cpp
 *		Project: Kactus 2
 */

#include "editabletableview.h"

#include <QHeaderView>
#include <QMenu>
#include <QKeySequence>
#include <QClipboard>
#include <QApplication>
#include <QModelIndexList>

#include <QDebug>

EditableTableView::EditableTableView(QWidget *parent):
QTableView(parent),
addAction_(tr("Add row"), this),
removeAction_(tr("Remove row"), this),
copyAction_(tr("Copy"), this),
pasteAction_(tr("Paste"), this),
clearAction_(tr("Clear"), this),
itemsDraggable_(true) {

	// cells are resized to match contents 
	horizontalHeader()->setResizeMode(QHeaderView::Interactive);

	//last column is stretched take the available space in the widget
	horizontalHeader()->setStretchLastSection(true);

	// vertical headers are not visible
	verticalHeader()->hide();

	// set the height of a row to be smaller than default
	verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	// easies to see the different rows from one another
	setAlternatingRowColors(true);

	// words are wrapped in the cells to minimize space usage
	setWordWrap(true);

	// user can only select one item at a time
	setSelectionMode(QAbstractItemView::ContiguousSelection);

	setEditTriggers(QAbstractItemView::DoubleClicked |
		QAbstractItemView::SelectedClicked |
		QAbstractItemView::EditKeyPressed |
		QAbstractItemView::AnyKeyPressed);

	setToolTip(tr("Double click to add a new item."));
	
	setupActions();

	setMinimumHeight(MINIMUM_TABLE_HEIGHT);
}

EditableTableView::~EditableTableView() {
}

void EditableTableView::mouseMoveEvent( QMouseEvent* e ) {
	if (itemsDraggable_) {

		// if left mouse button was pressed 
		if (e->buttons() & Qt::LeftButton) {

			QModelIndex startIndex = indexAt(pressedPoint_);
			QModelIndex thisIndex = indexAt(e->pos());

			// if the item was dragged to new location
			if (startIndex.isValid() && startIndex != thisIndex) {
				setCursor(QCursor(Qt::ClosedHandCursor));

				emit moveItem(startIndex, thisIndex);

				// update the pressed point so the dragging works also
				// when moving further to another index
				pressedPoint_ = e->pos();
			}
		}
	}

	QTableView::mouseMoveEvent(e);
}

void EditableTableView::keyPressEvent( QKeyEvent* event ) {

	// call the base class implementation
	QTableView::keyPressEvent(event);

	if (event->matches(QKeySequence::Copy)) {
		onCopyAction();
	}
	if (event->matches(QKeySequence::Paste)) {
		onPasteAction();
	}
	if (event->matches(QKeySequence::Delete)) {
		onClearAction();
	}
	if (event->matches(QKeySequence::InsertLineSeparator)) {
		onAddAction();
	}
}

void EditableTableView::mouseDoubleClickEvent( QMouseEvent* event ) {
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid()) {
		emit addItem(index);
		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

void EditableTableView::mousePressEvent( QMouseEvent* event ) {
	pressedPoint_ = event->pos();

	// if user clicked area that has no item
	QModelIndex pressedIndex = indexAt(pressedPoint_);
	if (!pressedIndex.isValid()) {
		setCurrentIndex(pressedIndex);
	}

	QTableView::mousePressEvent(event);
}

void EditableTableView::mouseReleaseEvent( QMouseEvent* event ) {
	setCursor(QCursor(Qt::ArrowCursor));
	QTableView::mouseReleaseEvent(event);
}

void EditableTableView::contextMenuEvent( QContextMenuEvent* event ) {
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);
	menu.addAction(&addAction_);

	// if at least one valid item is selected
	if (index.isValid()) {
		menu.addAction(&removeAction_);
		menu.addAction(&clearAction_);
		menu.addAction(&copyAction_);
	}
	
	menu.addAction(&pasteAction_);
	menu.exec(event->globalPos());

	event->accept();
}

void EditableTableView::onAddAction() {

	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToAdd;
	int rowCount = 1;

	// if there were indexes selected
	if (!indexes.isEmpty()) {
		qSort(indexes);

		posToAdd = indexes.first();

		// count how many rows are to be added
		int previousRow = indexes.first().row();
		foreach (QModelIndex index, indexes) {
			
			if (index.row() != previousRow) {
				++rowCount;
			}

			previousRow = index.row();
		}
	}

	for (int i = 0; i < rowCount; ++i) {
		emit addItem(posToAdd);
	}
}

void EditableTableView::onRemoveAction() {
	QModelIndexList indexes = selectedIndexes();

	if (indexes.isEmpty()) {
		return;
	}

	qSort(indexes);
	
	// count how many rows the user wants to remove
	int previousRow = indexes.first().row();
	int rowCount = 1;
	foreach (QModelIndex index, indexes) {
		if (previousRow != index.row()) {
			++rowCount;
		}
		previousRow = index.row();
	}

	// remove as many rows as wanted
	for (int i = 0; i < rowCount; ++i) {
		emit removeItem(indexes.first());
	}

	clearSelection();
	setCurrentIndex(QModelIndex());
}

void EditableTableView::setupActions() {
	addAction_.setToolTip(tr("Add a new row to table"));
	addAction_.setStatusTip(tr("Add a new row to table"));
	connect(&addAction_, SIGNAL(triggered()),
		this, SLOT(onAddAction()), Qt::UniqueConnection);
	addAction_.setShortcut(QKeySequence::InsertLineSeparator);

	removeAction_.setToolTip(tr("Remove a row from the table"));
	removeAction_.setStatusTip(tr("Remove a row from the table"));
	connect(&removeAction_, SIGNAL(triggered()),
		this, SLOT(onRemoveAction()), Qt::UniqueConnection);

	copyAction_.setToolTip(tr("Copy a row from the table"));
	copyAction_.setStatusTip(tr("Copy a row from the table"));
	connect(&copyAction_, SIGNAL(triggered()),
		this, SLOT(onCopyAction()), Qt::UniqueConnection);
	copyAction_.setShortcut(QKeySequence::Copy);

	pasteAction_.setToolTip(tr("Paste a row to the table"));
	pasteAction_.setStatusTip(tr("Paste a row to the table"));
	connect(&pasteAction_, SIGNAL(triggered()),
		this, SLOT(onPasteAction()), Qt::UniqueConnection);
	pasteAction_.setShortcut(QKeySequence::Paste);

	clearAction_.setToolTip(tr("Clear the contents of a cell"));
	clearAction_.setStatusTip(tr("Clear the contents of a cell"));
	connect(&clearAction_, SIGNAL(triggered()),
		this, SLOT(onClearAction()), Qt::UniqueConnection);
	clearAction_.setShortcut(QKeySequence::Delete);
}

void EditableTableView::setItemsDraggable( bool draggable ) {
	itemsDraggable_ = draggable;
}

void EditableTableView::onCopyAction() {

	// if nothing was selected then don't copy anything
	if (!currentIndex().isValid()) {
		return;
	}

	QModelIndexList indexes = selectedIndexes();
	qSort(indexes);

	QString copyText;

	for (int row = indexes.first().row(); row <= indexes.last().row(); ++row) {
		for (int column = indexes.first().column(); column <= indexes.last().column(); ++column) {

			QModelIndex itemToAdd = model()->index(row, column, QModelIndex());

			copyText.append(model()->data(itemToAdd, Qt::DisplayRole).toString());

			// separate columns with tab
			if (column < indexes.last().column()) {
				copyText.append("\t");
			}
			// for the last column don't add tab but new line instead
			else {
				copyText.append("\n");
			}
		}
	}

	QClipboard* clipBoard = QApplication::clipboard();

	clipBoard->setText(copyText);
}

void EditableTableView::onPasteAction() {

	// find the highest row to start adding to
	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToPaste;
	if (!indexes.isEmpty()) {
		qSort(indexes);
		posToPaste = indexes.first();
	}

	int startRow = posToPaste.row();
	int startCol = posToPaste.column();

	QClipboard* clipBoard = QApplication::clipboard();
	QString pasteText = clipBoard->text(QString("plain"));

	// split the string from clip board into rows
	QStringList rowsToAdd = pasteText.split("\n");
	foreach (QString row, rowsToAdd) {

		if (row.isEmpty()) {
			continue;
		}

		// new row starts always on same column
		int columnCounter = qMax(0, startCol);

		QModelIndex newRow = model()->index(startRow, columnCounter, QModelIndex());
		emit addItem(newRow);

		// split the row into columns
		QStringList columnsToAdd = row.split("\t");
		foreach (QString column, columnsToAdd) {

			if (column.isEmpty()) {
				continue;
			}

			QModelIndex itemToSet = model()->index(startRow, columnCounter, QModelIndex());
			// if the index is not valid then the data is written to last item
			if (!itemToSet.isValid()) {
				int lastRow = model()->rowCount(QModelIndex()) - 1;

				itemToSet = model()->index(lastRow, columnCounter, QModelIndex());
			}
			model()->setData(itemToSet, column, Qt::EditRole);
			++columnCounter;
		}
		
		if (startRow >= 0) {
			// when row is done then increase the row counter
			++startRow;
		}
	}
}

void EditableTableView::onClearAction() {
	
	QModelIndexList indexes = selectedIndexes();

	// clear the contents of each cell
	foreach (QModelIndex index, indexes) {
		model()->setData(index, QVariant(), Qt::EditRole);
	}
}
