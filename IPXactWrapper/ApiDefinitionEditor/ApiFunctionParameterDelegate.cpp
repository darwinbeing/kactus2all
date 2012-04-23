//-----------------------------------------------------------------------------
// File: ApiFunctionParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 18.4.2012
//
// Description:
// Delegate for editing API function parameters.
//-----------------------------------------------------------------------------

#include "ApiFunctionParameterDelegate.h"

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::ApiFunctionParameterDelegate()
//-----------------------------------------------------------------------------
ApiFunctionParameterDelegate::ApiFunctionParameterDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    builtInTypes_.append("char");
    builtInTypes_.append("char*");
    builtInTypes_.append("const char*");
    builtInTypes_.append("double");
    builtInTypes_.append("float");
    builtInTypes_.append("int");
    builtInTypes_.append("long");
    builtInTypes_.append("short");
    builtInTypes_.append("signed char");
    builtInTypes_.append("unsigned char");
    builtInTypes_.append("unsigned int");
    builtInTypes_.append("unsigned long");
    builtInTypes_.append("unsigned short");
    builtInTypes_.append("void");
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::~ApiFunctionParameterDelegate()
//-----------------------------------------------------------------------------
ApiFunctionParameterDelegate::~ApiFunctionParameterDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::updateDataTypes()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::updateDataTypes(QStringList const& dataTypes)
{
    customTypes_ = dataTypes;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ApiFunctionParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
                                                    QModelIndex const& index) const
{
    switch (index.column())
    {
    case API_FUNC_PARAM_COL_NAME:
    case API_FUNC_PARAM_COL_DESC:
        {
            QLineEdit* line = new QLineEdit(parent);
            connect(line, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return line;
        }

    case API_FUNC_PARAM_COL_TYPE:
        {
            QComboBox* box = new QComboBox(parent);
            box->setEditable(true);
            box->setInsertPolicy(QComboBox::InsertAlphabetically);

            box->addItems(getDataTypesList());
            return box;
        }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    switch (index.column())
    {
    case API_FUNC_PARAM_COL_TYPE:
        {
            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            box->setCurrentIndex(box->findText(text));
            return;
        }

    case API_FUNC_PARAM_COL_NAME:
    case API_FUNC_PARAM_COL_DESC:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            line->setText(text);
            return;
        }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    switch (index.column())
    {
    case API_FUNC_PARAM_COL_TYPE:
        {
            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = box->currentText();
            model->setData(index, text, Qt::EditRole);
            return;
        }

    case API_FUNC_PARAM_COL_NAME:
    case API_FUNC_PARAM_COL_DESC:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "ApiFunctionParameterDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = line->text();
            model->setData(index, text, Qt::EditRole);
            return;
        }
    default:
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ApiFunctionParameterDelegate::commitAndCloseEditor()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

    if (lineEdit != 0)
    {
        emit commitData(lineEdit);
        emit closeEditor(lineEdit);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameterDelegate::getDataTypesList()
//-----------------------------------------------------------------------------
QStringList ApiFunctionParameterDelegate::getDataTypesList() const
{
    QStringList list = builtInTypes_ + customTypes_;
    list.sort();
    list.removeDuplicates();
    return list;
}
