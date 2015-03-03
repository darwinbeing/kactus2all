/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressblockdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <IPXACTmodels/validators/BinaryValidator.h>

#include <QLineEdit>
#include <QIntValidator>

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::AddressBlockDelegate()
//-----------------------------------------------------------------------------
AddressBlockDelegate::AddressBlockDelegate(QCompleter* parameterNameCompleter, 
    QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
ExpressionDelegate(parameterNameCompleter, parameterFinder, parent) 
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::~AddressBlockDelegate()
//-----------------------------------------------------------------------------
AddressBlockDelegate::~AddressBlockDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AddressBlockDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        return new BooleanComboBox(parent);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        return new AccessComboBox(parent);
    }
    else if (index.column() == AddressBlockColumns::RESET_VALUE || index.column() == AddressBlockColumns::RESET_MASK)
    {
        QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);

        if (lineEditor)
        {
            QModelIndex modelIndex (index.sibling(index.row(), AddressBlockColumns::REGISTER_SIZE));
            QString sizeExpression = modelIndex.data(Qt::ToolTipRole).toString();

            lineEditor->setValidator(new BinaryValidator(sizeExpression, lineEditor));
        }

        return editor;
    }
    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = index.model()->data(index, Qt::DisplayRole).toBool();
        boolBox->setCurrentValue(value);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = General::str2Access(index.model()->data(
            index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
        accessBox->setCurrentValue(access);
    }
    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::setModelData()
//-----------------------------------------------------------------------------
void AddressBlockDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == AddressBlockColumns::VOLATILE)
    {
        BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
        Q_ASSERT(boolBox);

        bool value = boolBox->getCurrentValue();
        model->setData(index, value, Qt::EditRole);
    }
    else if (index.column() == AddressBlockColumns::REGISTER_ACCESS)
    {
        AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
        Q_ASSERT(accessBox);

        General::Access access = accessBox->getCurrentValue();
        model->setData(index, General::access2Str(access), Qt::EditRole);
    }
    else 
    {
        ExpressionDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressBlockDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool AddressBlockDelegate::columnAcceptsExpression(int column) const
{
    return column == AddressBlockColumns::REGISTER_DIMENSION ||
        column == AddressBlockColumns::REGISTER_SIZE ||
        column == AddressBlockColumns::REGISTER_OFFSET;
}