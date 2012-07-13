/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: swviewsdelegate.cpp
 *		Project: Kactus 2
 */

#include "SystemViewsDelegate.h"

#include <QLineEdit>

SystemViewsDelegate::SystemViewsDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

SystemViewsDelegate::~SystemViewsDelegate() {
}

QWidget* SystemViewsDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
								 }
		case HIER_REF_COLUMN: {
			Q_ASSERT(false);
			return NULL;
							  }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void SystemViewsDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
								 }
		case HIER_REF_COLUMN: {
			Q_ASSERT(false);
			break;
							  }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void SystemViewsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case NAME_COLUMN:
		case DISPLAY_NAME_COLUMN:
		case DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
								 }
		case HIER_REF_COLUMN: {
			Q_ASSERT(false);
			break;
							  }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void SystemViewsDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
