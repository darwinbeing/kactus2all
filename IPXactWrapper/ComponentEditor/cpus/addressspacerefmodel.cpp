/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacerefmodel.cpp
 *		Project: Kactus 2
 */

#include "addressspacerefmodel.h"

#include <QColor>

AddressSpaceRefModel::AddressSpaceRefModel( QObject *parent,
										   QSharedPointer<Component> component,
										   const QStringList& items /*= QStringList()*/ ):
ListManagerModel(parent, items),
component_(component) {

	Q_ASSERT(component);
}

AddressSpaceRefModel::~AddressSpaceRefModel() {
}

QVariant AddressSpaceRefModel::data( const QModelIndex& index,
									int role /*= Qt::DisplayRole*/ ) const {

	// nothing for invalid indexes
	if (!index.isValid()) {
		return QVariant();
	}

	// if there are no real items then display the help text
	else if (index.row() == 0 && items_.isEmpty()) {
		switch (role) {
			case Qt::DisplayRole: {
				return tr("Double click to add new item.");
								  }
			case Qt::ForegroundRole: {
				return QColor("silver");
									 }
			default: {
				return QVariant();
					 }
		}
	}

	// if index.row() is invalid
	else if (index.row() < 0 || index.row() >= items_.size()) {
		return QVariant();
	}

	// return data for display role
	if (role == Qt::DisplayRole) {
		return items_.at(index.row());
	}
	else if (role == Qt::ForegroundRole) {

		// if component contains the named file set.
		if (component_->hasAddressSpace(items_.at(index.row()))) {
			return QColor("black");
		}

		// if the file set does not exist.
		else {
			return QColor("red");
		}
	}
	// if unsupported role
	else {
		return QVariant();
	}
}
