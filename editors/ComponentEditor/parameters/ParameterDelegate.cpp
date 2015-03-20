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
#include <IPXACTmodels/validators/namevalidator.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/parameters/Array/ParameterArrayModel.h>
#include <editors/ComponentEditor/parameters/Array/ArrayDelegate.h>
#include <editors/ComponentEditor/parameters/Array/ArrayView.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QScrollArea>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::ParameterDelegate()
//-----------------------------------------------------------------------------
ParameterDelegate::ParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
    QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QObject* parent):
ExpressionDelegate(parameterCompleter, parameterFinder, parent),
choices_(choices),
expressionFormatter_(expressionFormatter)
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
    if (index.column() == nameColumn())
    {
        QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
        
        QLineEdit* lineEditor = qobject_cast<QLineEdit*>(editor);

        if (lineEditor)
        {
            lineEditor->setValidator(new NameValidator(lineEditor));
        }

        return editor;
    }

    if (index.column() == choiceColumn()) 
    {
        return createChoiceSelector(parent);
    }
    else if (index.column() == formatColumn()) 
    {
        return createFormatSelector(parent);
    }
    else if (index.column() == resolveColumn())
    {
        return createResolveSelector(parent);
    }
    else if (index.column() == usageCountColumn())
    {
        QModelIndex valueIdIndex = index.sibling(index.row(), sourceIdsColumn());
        QString targetId = valueIdIndex.data(Qt::DisplayRole).toString();
        emit(openReferenceTree(targetId));

        return 0;
    }
    
    else if (index.column() == valueColumn() && valueIsArray(index))
    {
        ArrayView* editor = new ArrayView(parent);

        QScrollArea* scrollingWidget = new QScrollArea(parent);
        scrollingWidget->setWidgetResizable(true);
        scrollingWidget->setWidget(editor);

        scrollingWidget->parent()->installEventFilter(editor);

        return scrollingWidget;
    }

    else if (isIndexForValueUsingChoice(index)) 
    {
        return createEnumerationSelector(parent, index);
    }

    else
    {
        return ExpressionDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const 
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        ArrayView* view = dynamic_cast<ArrayView*>(dynamic_cast<QScrollArea*>(editor)->widget());

        QModelIndex arraySizeIndex = index.sibling(index.row(), arraySizeColumn());
        int arraySize = arraySizeIndex.data(Qt::ToolTipRole).toInt();

        QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder_));

        QSharedPointer<Choice> selectedChoice = findChoice(index);

        ParameterArrayModel* model = new ParameterArrayModel(arraySize, expressionParser, parameterFinder_,
            expressionFormatter_, selectedChoice, view);

        QModelIndex valueIndex = index.sibling(index.row(), valueColumn());
        QString parameterValue = valueIndex.data(Qt::EditRole).toString();
        model->setArrayData(parameterValue);

        QModelIndex typeIndex = index.sibling(index.row(), formatColumn());
        QString parameterType = typeIndex.data(Qt::EditRole).toString();
        model->setParameterType(parameterType);

        view->setItemDelegate(new ArrayDelegate(parameterNameCompleter_, parameterFinder_, selectedChoice,
            this->parent()));

        view->setModel(model);
        view->setSortingEnabled(false);
        view->resizeColumnsToContents();

        connect(model, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
        connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);

        connect(view->itemDelegate(), SIGNAL(increaseReferences(QString)),
            this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
        connect(view->itemDelegate(), SIGNAL(decreaseReferences(QString)),
            this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    }

    else if (isIndexForValueUsingChoice(index)) 
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
    else if (qobject_cast<QComboBox*>(editor)) 
    {
		QString text = index.data(Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		
		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}
	else if (index.column() == usageCountColumn())
    {
        // Do nothing.
	}

    else
    {
        ExpressionDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::setModelData()
//-----------------------------------------------------------------------------
void ParameterDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const 
{
    if (index.column() == valueColumn() && valueIsArray(index))
    {
        QScrollArea* scrollWidget = qobject_cast<QScrollArea*>(editor);
        ArrayView* arrayTable = qobject_cast<ArrayView*>(scrollWidget->widget());
        ParameterArrayModel* arrayModel = qobject_cast<ParameterArrayModel*>(arrayTable->model());

        QString arrayValue = arrayModel->getArrayData();
        model->setData(index, arrayValue, Qt::EditRole);
    }

    else if (isIndexForValueUsingChoice(index)) 
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        QString text = combo->currentText();
        text = text.left(text.indexOf(':'));
        model->setData(index, text, Qt::EditRole);
    }
    else if (qobject_cast<QComboBox*>(editor))
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
        ExpressionDelegate::setModelData(editor, model, index);
	}
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::paint()
//-----------------------------------------------------------------------------
void ParameterDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    ExpressionDelegate::paint(painter, option, index);

    if (index.column() == maximumColumn() || index.column() == valueColumn() || 
        index.column() == arrayOffsetColumn())
    {
        QPen oldPen = painter->pen();
        QPen newPen(Qt::lightGray);
        newPen.setWidth(2);
        painter->setPen(newPen);
        painter->drawLine(option.rect.topRight() + QPoint(1,1), option.rect.bottomRight() + QPoint(1,1));
        painter->setPen(oldPen);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::nameColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::nameColumn() const
{
    return ParameterColumns::NAME;
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
// Function: ParameterDelegate::descriptionColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::descriptionColumn() const
{
    return ParameterColumns::DESCRIPTION;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::arraySizeColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::arraySizeColumn() const
{
    return ParameterColumns::ARRAY_SIZE;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::arrayOffsetColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::arrayOffsetColumn() const
{
    return ParameterColumns::ARRAY_OFFSET;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::usageCountColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::usageCountColumn() const
{
    return ParameterColumns::USAGE_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::valueIdColumn()
//-----------------------------------------------------------------------------
int ParameterDelegate::sourceIdsColumn() const
{
    return ParameterColumns::SOURCEIDS;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::columnAcceptsExpression()
//-----------------------------------------------------------------------------
bool ParameterDelegate::columnAcceptsExpression(int column) const
{
    return column == valueColumn() || column == bitwidthColumn() ||
        column == arraySizeColumn() || column == arrayOffsetColumn();
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

    return QSharedPointer<Choice>(new Choice());
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
// Function: ParameterDelegate::valueIsArray()
//-----------------------------------------------------------------------------
bool ParameterDelegate::valueIsArray(QModelIndex const& index) const
{
    QModelIndex valueIdIndex = index.sibling(index.row(), arraySizeColumn());
    bool arraySizeIsOk = true;
    int arraySize = valueIdIndex.data(Qt::ToolTipRole).toInt(&arraySizeIsOk);
    
    return arraySizeIsOk && arraySize > 0;
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::updateEditorGeometry()
//-----------------------------------------------------------------------------
void ParameterDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    ExpressionDelegate::updateEditorGeometry(editor, option, index);

    if (index.column() == valueColumn() && valueIsArray(index))
    {
        repositionAndResizeEditor(editor, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterDelegate::repositionAndResizeEditor()
//-----------------------------------------------------------------------------
void ParameterDelegate::repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QModelIndex valueIdIndex = index.sibling(index.row(), arraySizeColumn());
    int arraySize = valueIdIndex.data(Qt::ToolTipRole).toInt();
    int editorMinimumSize = 24 * (arraySize + 1);

    editor->setFixedWidth(300);

    const int PARENT_HEIGHT = editor->parentWidget()->height();
    const int AVAILABLE_HEIGHT_BELOW = PARENT_HEIGHT - option.rect.top();

    if (AVAILABLE_HEIGHT_BELOW > editorMinimumSize)
    {
        editor->move(option.rect.topLeft());
    }
    else
    {
        int editorNewY = PARENT_HEIGHT-editorMinimumSize;
        if (editorNewY <= 0)
        {
            editorNewY = 0;
        }

        editor->move(option.rect.left(), editorNewY);
    }

    if (editorMinimumSize > PARENT_HEIGHT)
    {
        editor->setFixedHeight(PARENT_HEIGHT);
    }
    else
    {
        editor->setFixedHeight(editorMinimumSize);
    }
}
