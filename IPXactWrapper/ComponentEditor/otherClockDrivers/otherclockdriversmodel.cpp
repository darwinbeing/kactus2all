/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: otherclockdriversmodel.cpp
 */

#include "otherclockdriversmodel.h"

#include <models/component.h>
#include <models/generaldeclarations.h>
#include <models/otherclockdriver.h>

#include <QColor>

#include <QDebug>

OtherClockDriversModel::OtherClockDriversModel(QSharedPointer<Component> component, 
											   QObject *parent): 
QAbstractTableModel(parent),
component_(component), 
table_(component->getOtherClockDrivers()) {

	Q_ASSERT_X(component, "OtherClockDrivers constructor",
		"Null Component-pointer given as parameter");

}

OtherClockDriversModel::~OtherClockDriversModel() {
}

int OtherClockDriversModel::rowCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;

	return table_.size();
}

int OtherClockDriversModel::columnCount( const QModelIndex& parent /*= QModelIndex() */ ) const {

	if (parent.isValid())
		return 0;

	return 9;
}

QVariant OtherClockDriversModel::data( const QModelIndex& index, 
									  int role /*= Qt::DisplayRole */ ) const {
	

	if (!index.isValid())
		return QVariant();

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0:
				return table_.at(index.row())->getClockName();
			case 1:
				return table_.at(index.row())->getClockSource();
			case 2:
				return table_.at(index.row())->getClockPeriod()->value_;
			case 3:
				qDebug() << "Data period unit: " << table_.at(index.row())->getClockPeriod()->timeUnit_;
				return General::timeUnit2Str(
					table_.at(index.row())->getClockPeriod()->timeUnit_);
			case 4:
				return table_.at(index.row())->getClockPulseOffset()->value_;
			case 5:
				qDebug() << "Data pulse offset: " << table_.at(index.row())->getClockPulseOffset()->timeUnit_;
				return General::timeUnit2Str(
					table_.at(index.row())->getClockPulseOffset()->timeUnit_);
			case 6:
				return table_.at(index.row())->getClockPulseValue()->value_;
			case 7:
				return table_.at(index.row())->getClockPulseDuration()->value_;
			case 8:
				return General::timeUnit2Str(
					table_.at(index.row())->getClockPulseDuration()->timeUnit_);
			default:
				return QVariant();

		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case 0:
			case 2:
			case 4:
			case 6:
			case 7: {
				return QColor("LemonChiffon");
					}
			default:
				return QColor("white");
		}
	}
	else if (Qt::ForegroundRole == role) {
		if (table_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}

	// is unsupported role
	else {
		return QVariant();
	}
}

QVariant OtherClockDriversModel::headerData( int section, 
											Qt::Orientation orientation, 
											int role /*= Qt::DisplayRole */ ) const {
	
	if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (section) {
			case 0: 
				return tr("Clock\nname");
			case 1:
				return tr("Clock\nsource");
			case 2:
				return tr("Clock\nperiod");
			case 3:
				return tr("Period\nunit");
			case 4:
				return tr("Pulse\noffset");
			case 5:
				return tr("Offset\nunit");
			case 6:
				return tr("Pulse\nvalue");
			case 7:
				return tr("Pulse\nduration");
			case 8:
				return tr("Duration\nunit");
			default:
				return QVariant();

		}
	}

	// is unsupported role
	else {
		return QVariant();
	}
}

bool OtherClockDriversModel::setData( const QModelIndex& index, 
									 const QVariant& value, 
									 int role /*= Qt::EditRole */ ) {

	if (!index.isValid())
		return false;

	// if row is invalid
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {

		switch (index.column()) {
			case 0: {
				table_.value(index.row())->setClockName(value.toString());
				break;
					}
			case 1: {
				table_.value(index.row())->setClockSource(value.toString());
				break;
					}
			case 2: {
				table_.value(index.row())->getClockPeriod()->value_ = value.toDouble();
				break;
					}
			case 3: {
				qDebug() << "clock period unit: " << General::str2TimeUnit(value.toString(), General::NS);
				table_.value(index.row())->getClockPeriod()->timeUnit_ = 
					General::str2TimeUnit(value.toString(), General::NS);
				break;
					}
			case 4: {
				table_.value(index.row())->getClockPulseOffset()->value_ = value.toDouble();
				break;
					}
			case 5: {
				qDebug() << "Pulse offset unit: " << General::str2TimeUnit(value.toString(), General::NS);
				table_.value(index.row())->getClockPulseOffset()->timeUnit_ =
					General::str2TimeUnit(value.toString(), General::NS);
				break;
					}
			case 6: {
				table_.value(index.row())->getClockPulseValue()->value_ = value.toInt();
				break;
					}
			case 7: {
				table_.value(index.row())->getClockPulseDuration()->value_ = value.toDouble();
				break;
					}
			case 8: {
				qDebug() << "Pulse duration unit: " << General::str2TimeUnit(value.toString(), General::NS);
				table_.value(index.row())->getClockPulseDuration()->timeUnit_ =
					General::str2TimeUnit(value.toString(), General::NS);
				break;
					}
			default:
				return false;
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}

	// if unsupported role
	else {
		return false;
	}
}

Qt::ItemFlags OtherClockDriversModel::flags( const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool OtherClockDriversModel::isValid() const {

	// check all items in the model
	foreach (QSharedPointer<OtherClockDriver> driver, table_) {

		// if one item is not valid
		if (!driver->isValid())
			return false;
	}

	// all items were valid
	return true;
}

void OtherClockDriversModel::onRemoveRow( int row ) {
	// if row is invalid
	if (row < 0 || row >= table_.size())
		return;

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
	table_.removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void OtherClockDriversModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void OtherClockDriversModel::onAddRow() {
	beginInsertRows(QModelIndex(), table_.size(), table_.size());

	table_.append(QSharedPointer<OtherClockDriver>(new OtherClockDriver()));

	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void OtherClockDriversModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, QSharedPointer<OtherClockDriver>(new OtherClockDriver()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}
