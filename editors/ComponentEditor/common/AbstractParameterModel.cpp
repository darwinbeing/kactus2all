//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#include "AbstractParameterModel.h"

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/Enumeration.h>

#include <IPXACTmodels/validators/ParameterValidator2014.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::AbstractParameterModel(QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<ExpressionParser> expressionParser, QObject *parent): 
QAbstractTableModel(parent), choices_(choices), expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::~AbstractParameterModel()
//-----------------------------------------------------------------------------
AbstractParameterModel::~AbstractParameterModel()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::data( QModelIndex const& index, int role /*= Qt::DisplayRole */ ) const 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return QVariant();
    }

    QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());
    
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == nameColumn())
        {
            return parameter->getName();
        }
        else if (index.column() == displayNameColumn())
        {
            return parameter->getDisplayName();
        }
        else if (index.column() == typeColumn())
        {
            return parameter->getType();
        }
        else if (index.column() == bitWidthColumn())
        {
            return parameter->getBitWidth();
        }
        else if (index.column() == minimumColumn())
        {
            return parameter->getMinimumValue();
        }
        else if (index.column() == maximumColumn())
        {
            return parameter->getMaximumValue();
        }
        else if (index.column() == choiceColumn())
        {
            return parameter->getChoiceRef();
        }
        else if (index.column() == valueColumn())
        {
            if (role == Qt::EditRole)
            {
                return parameter->getValue();
            }
            else
            {
                return evaluateValueFor(parameter);
            }
        }
        else if (index.column() == resolveColumn())
        {
            return parameter->getValueResolve();
        }
        else if (index.column() == arraySizeColumn())
        {
            return parameter->getAttribute("arraySize");
        }
        else if (index.column() == arrayOffsetColumn())
        {
            return parameter->getAttribute("arrayOffset");
        }
        else if (index.column() == descriptionColumn())
        {
            return parameter->getDescription();
        }
        else if (index.column() == valueIdColumn())
        {
            return parameter->getValueId();
        }

        else 
        {
            return QVariant();
        }
    }
    else if (Qt::BackgroundRole == role) 
    {
        if (index.column() == nameColumn() ||
            index.column() == valueColumn()) 
        {
            return QColor("lemonChiffon");
        }
        else if ((index.column() == minimumColumn() || index.column() == maximumColumn()) &&
            (parameter->getType() == "bit" || parameter->getType() == "string" || parameter->getType().isEmpty()))
        {
            return QColor("whiteSmoke");
        }
        else
        {
            return QColor("white");
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        if (validateColumnForParameter(index.column(), parameter))
        {
            return QColor("black");
        }
        else 
        {
            return QColor("red");
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        if (index.column() == valueColumn())
        {
            return parameter->getValue();
        }
        else
        {
            return QVariant();
        }
    }
	else // if unsupported role
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractParameterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole) 
    {
        QString mathSymbolFunction(0x0192);

        if (section == nameColumn())
        {
            return tr("Name");
        }
        else if (section == displayNameColumn())
        {
            return tr("Display\nname");
        }
        else if (section == typeColumn())
        {
            return tr("Type");   
        }
        else if (section == bitWidthColumn())
        {
            return QString(tr("Bit width, ") + mathSymbolFunction + "(x)");
        }
        else if (section == minimumColumn())
        {
            return tr("Min");
        }
        else if (section == maximumColumn())
        {
            return tr("Max");
        }
        else if (section == choiceColumn())
        {
            return tr("Choice");
        }     
        else if (section == valueColumn())
        {
            return QString(tr("Value, ") + mathSymbolFunction + "(x)");
        }  
        else if (section == resolveColumn())
        {     
            return tr("Resolve");
        }  
        else if (section == arraySizeColumn())
        {
            return QString(tr("Array\nsize, ") + mathSymbolFunction + "(x)");
        }  
        else if (section == arrayOffsetColumn())
        {
            return QString(tr("Array\noffset, ") + mathSymbolFunction + "(x)");
        } 
        else if (section == descriptionColumn())
        { 
            return tr("Description");
        }
        else if (section == valueIdColumn())
        {
            return tr("Value ID");
        }
        else
        {
            return QVariant();
        }
    }
    // if unsupported role
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::setData()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::setData(QModelIndex const& index, const QVariant& value, int role /*= Qt::EditRole */) 
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
    {
		return false;
    }

    if (role == Qt::EditRole)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(index.row());

        if (index.column() == nameColumn())
        {
            parameter->setName(value.toString());
        }
        else if (index.column() == displayNameColumn())
        {
            parameter->setDisplayName(value.toString());
        }
        else if (index.column() == typeColumn())
        {
            parameter->setType(value.toString());
        }
        else if (index.column() == bitWidthColumn())
        {
            parameter->setBitWidth(value.toString());
        }
        else if (index.column() == minimumColumn())
        {
            parameter->setMinimumValue(value.toString());
        }
        else if (index.column() == maximumColumn())
        {
            parameter->setMaximumValue(value.toString());
        }
        else if (index.column() == choiceColumn())
        {
            parameter->setChoiceRef(value.toString());
        }
        else if (index.column() == valueColumn())
        {
            parameter->setValue(value.toString());
        }
        else if (index.column() == resolveColumn())
        {
            parameter->setValueResolve(value.toString());
        }
        else if (index.column() == arraySizeColumn())
        {
            parameter->setAttribute("arraySize", value.toString());
        }
        else if (index.column() == arrayOffsetColumn())
        {
            parameter->setAttribute("arrayOffset", value.toString());
        }
        else if (index.column() == descriptionColumn())
        {
            parameter->setDescription(value.toString());
        }
        else if (index.column() == valueIdColumn())
        {
            parameter->setValueId(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        emit contentChanged();
        return true;
    }
    else // is unsupported role
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractParameterModel::flags(QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid() const
{
    ParameterValidator validator;

	// check all parameters
	for (int i = 0; i < rowCount(); i++)
	{
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        if (!validator.validate(parameter.data(), choices_)) 
        {
            return false;
        }
	}
	
	// all parameters are valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::isValid()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::isValid(QStringList& errorList, QString const& parentIdentifier) const
{
    bool valid = true;

    ParameterValidator validator;
    for (int i = 0; i < rowCount(); i++)
    {
        QSharedPointer<Parameter> parameter = getParameterOnRow(i);

        errorList.append(validator.findErrorsIn(parameter.data(), parentIdentifier, choices_));

        // if one parameter is invalid, model is invalid.
        if (!validator.validate(parameter.data(), choices_))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::evaluateValueFor()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::evaluateValueFor(QSharedPointer<Parameter> parameter) const
{
    if (!parameter->getChoiceRef().isEmpty())
    {
        QSharedPointer<Choice> choice = findChoice(parameter->getChoiceRef());
        return findDisplayValueForEnumeration(choice, parameter->getValue());
    }
    else if (expressionParser_->isValidExpression(parameter->getValue()))
    {
        return expressionParser_->parseExpression(parameter->getValue());
    }
    else
    {
        return "n/a";
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::findChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> AbstractParameterModel::findChoice(QString const& choiceName) const
{
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
// Function: AbstractParameterModel::findDisplayValueForEnumeration()
//-----------------------------------------------------------------------------
QString AbstractParameterModel::findDisplayValueForEnumeration(QSharedPointer<Choice> choice,
    QString const& enumerationValue) const
{
    foreach (QSharedPointer<Enumeration> enumeration, *choice->enumerations())
    {
        if (enumeration->getValue() == enumerationValue && !enumeration->getText().isEmpty())
        {
            return enumeration->getText();
        }
    }

    return enumerationValue;
}

//-----------------------------------------------------------------------------
// Function: AbstractParameterModel::validateColumnForParameter()
//-----------------------------------------------------------------------------
bool AbstractParameterModel::validateColumnForParameter(int column, QSharedPointer<Parameter> parameter) const
{
    ParameterValidator2014 validator(expressionParser_);

    if (column == nameColumn())
    {
        return validator.hasValidName(parameter.data());
    }
    else if (column == typeColumn())
    {
        return validator.hasValidValueForType(parameter->getValue(), parameter->getType());
    }
    else if (column == bitWidthColumn())
    {
        return true;
    }
    else if (column == minimumColumn())
    {
        return validator.hasValidMinimumValue(parameter.data()) && 
            !validator.valueIsLessThanMinimum(parameter.data());
    }
    else if (column == maximumColumn())
    {
        return validator.hasValidMaximumValue(parameter.data()) && 
            !validator.valueIsGreaterThanMaximum(parameter.data());
    }
    else if (column == choiceColumn())
    {
        return validator.hasValidChoice(parameter.data(), choices_) &&
            validator.hasValidValueForChoice(parameter.data(), choices_);
    }
    else if (column == valueColumn())
    {
        return validator.hasValidValue(parameter.data(), choices_);
    }
    else if (column == resolveColumn())
    {
        return validator.hasValidResolve(parameter.data());
    }

    return true;
}

