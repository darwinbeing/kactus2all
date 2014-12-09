//-----------------------------------------------------------------------------
// File: ParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.11.2014
//
// Description:
// Delegate that provides widgets for editing parameters.
//-----------------------------------------------------------------------------

#include "ParameterDelegate.h"

#include "ParameterColumns.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>
#include <IPXACTmodels/StringPromtAtt.h>

#include <QComboBox>
#include <QLineEdit>
#include <QRegExpValidator>

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::ParameterDelegate()
//-----------------------------------------------------------------------------
ParameterDelegate::ParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject* parent):
QStyledItemDelegate(parent), choices_(choices)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::~ParameterDelegate()
//-----------------------------------------------------------------------------
ParameterDelegate::~ParameterDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index ) const
{
    if (index.column() == choiceColumn()) 
    {
        return createChoiceSelector(parent);
    }
    else if (isIndexForValueUsingChoice(index)) 
    {
        return createEnumerationSelector(parent, index);
    }
    else if (index.column() == formatColumn()) 
    {
        return createFormatSelector(parent);
    }
    else if (index.column() == bitwidthColumn()) 
    {
        return createNumberEditor(parent, option, index);
    }
    else if (index.column() == minimumColumn())
    {
        return createEditorUsingFormat(parent, option, index);
    }
    else if (index.column() == maximumColumn())
    {
        return createEditorUsingFormat(parent, option, index);
    }
    else if (index.column() == resolveColumn())
    {
        return createResolveSelector(parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    if (isIndexForValueUsingChoice(index)) 
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QComboBox* combo = qobject_cast<QComboBox*>(editor);

        int comboIndex = combo->findText(text, Qt::MatchEndsWith);
        if (comboIndex == -1)
        {
            comboIndex = combo->findText(text, Qt::MatchStartsWith);
        }
        combo->setCurrentIndex(comboIndex);
    }
    else if (combo) 
    {
		QString text = index.data(Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		
		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}
	else 
    {
        // use the line edit for other columns
        QString text = index.model()->data(index, Qt::DisplayRole).toString();
        QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
	}
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const 
{
    QComboBox* combo = qobject_cast<QComboBox*>(editor);

    if (isIndexForValueUsingChoice(index)) 
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        text = text.left(text.indexOf(':'));
        model->setData(index, text, Qt::EditRole);
    }
    else if (combo)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        if (text == "<none>")
        {
            text = "";
        }
        model->setData(index, text, Qt::EditRole);
    }
	else 
    {
        QStyledItemDelegate::setModelData(editor, model, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::choiceColumn() const
{
    return ParameterColumns::CHOICE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::formatColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::formatColumn() const
{
    return ParameterColumns::TYPE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::bitwidthColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::bitwidthColumn() const
{
    return ParameterColumns::BITWIDTH;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::minimumColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::minimumColumn() const
{
    return ParameterColumns::MINIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::maximumColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::maximumColumn() const
{
    return ParameterColumns::MAXIMUM;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::valueColumn() const
{
    return ParameterColumns::VALUE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::resolveColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::resolveColumn() const
{
    return ParameterColumns::RESOLVE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceSelected()
//-----------------------------------------------------------------------------
bool ParameterDelegate::isIndexForValueUsingChoice(QModelIndex const& index) const
{
    return index.column() == valueColumn() && !choiceNameOnRow(index).isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::choiceNameOnRow()
//-----------------------------------------------------------------------------
QString ParameterDelegate::choiceNameOnRow(QModelIndex const& index) const
{
    return index.sibling(index.row(), choiceColumn()).data().toString();
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> ParameterDelegate::findChoice(QModelIndex const& index) const
{
    QString choiceName = choiceNameOnRow(index);
    foreach (QSharedPointer<Choice> choice, *choices_)
    {
        if (choice->getName() == choiceName)
        {
            return choice;
        }
    }	
    
    return QSharedPointer<Choice>(new Choice(QDomNode()));
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::selectedFormat()
//-----------------------------------------------------------------------------
QString ParameterDelegate::formatOnRow(QModelIndex const &index) const
{
    return index.sibling(index.row(), formatColumn()).data().toString();
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createFormatEditor()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createFormatSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString(""));
    combo->addItem(QString("bit"));
    combo->addItem(QString("byte"));
    combo->addItem(QString("shortint"));
    combo->addItem(QString("int"));
    combo->addItem(QString("longint"));
    combo->addItem(QString("shortreal"));
    combo->addItem(QString("real"));
    combo->addItem(QString("string"));
    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createChoiceSelector()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createChoiceSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("<none>"));
    foreach (QSharedPointer<Choice> choice, *choices_)
    {
        combo->addItem(choice->getName());
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createEnumerationSelector()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createEnumerationSelector(QWidget* parent, QModelIndex const& index) const
{
    QComboBox* combo = new QComboBox(parent);

    QSharedPointer<Choice> selectedChoice = findChoice(index);
    foreach (QSharedPointer<Enumeration> enumeration, *selectedChoice->enumerations())
    {
        QString itemText = enumeration->getValue();
        if (!enumeration->getText().isEmpty())
        {
            itemText.append(":" + enumeration->getText());
        }
        combo->addItem(itemText);
    }

    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createValueEditorUsingFormat()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createEditorUsingFormat(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit)
    {
        lineEdit->setValidator(createValidatorForFormat(formatOnRow(index), lineEdit));
    }

    return editor;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createNumberEditor()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createNumberEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
    if (lineEdit)
    {
        lineEdit->setValidator(new QRegExpValidator(QRegExp("\\d*")));
    }

    return editor;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createResolveSelector()
//-----------------------------------------------------------------------------
QWidget* ParameterDelegate::createResolveSelector(QWidget* parent) const
{
    QComboBox* combo = new QComboBox(parent);
    combo->addItem(QString("immediate"));
    combo->addItem(QString("user"));
    combo->addItem(QString("generated"));
    return combo;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::createValidatorForFormat()
//-----------------------------------------------------------------------------
QValidator* ParameterDelegate::createValidatorForFormat(QString const& format, QWidget* parent) const
{
    if (format == "bool")
    {
        return new QRegExpValidator(QRegExp("(?:" + StringPromptAtt::VALID_BOOL_VALUE + ")?"), parent);
    }
    else if (format == "long")
    {
        return new QRegExpValidator(QRegExp("(?:" + StringPromptAtt::VALID_LONG_VALUE + ")?"), parent);
    }
    else if (format == "bitString")
    {
        return new QRegExpValidator(QRegExp("(?:" + StringPromptAtt::VALID_BITSTRING_VALUE + ")?"), parent);
    }
    else if (format == "float")
    {
         return new QRegExpValidator(QRegExp("(?:" + StringPromptAtt::VALID_FLOAT_VALUE + ")?"), parent);
    }
    else if (format == "string" || format.isEmpty())
    {
        return 0;
    }
    else
    {
        return new QRegExpValidator(QRegExp(), parent);
    }
}
