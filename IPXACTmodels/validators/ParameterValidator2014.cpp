//-----------------------------------------------------------------------------
// File: ParameterValidator2014.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "ParameterValidator2014.h"

#include <IPXACTmodels/parameter.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::ParameterValidator2014(QSharedPointer<ExpressionParser> expressionParser) : 
ParameterValidator(), expressionParser_(expressionParser)
{

}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::~SystemVerilogValidator()
//-----------------------------------------------------------------------------
ParameterValidator2014::~ParameterValidator2014()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::validate()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::validate(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return hasValidName(parameter) &&
        hasValidValue(parameter, availableChoices) &&
        hasValidMinimumValue(parameter) &&
        hasValidMaximumValue(parameter) &&
        hasValidChoice(parameter, availableChoices) &&
        hasValidResolve(parameter) &&
        hasValidValueId(parameter);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValue(Parameter const* parameter, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    return !parameter->getValue().isEmpty() &&
        hasValidValueForType(parameter) &&
        !valueIsLessThanMinimum(parameter) &&
        !valueIsGreaterThanMaximum(parameter) &&
        hasValidValueForChoice(parameter, availableChoices);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidType(Parameter const* parameter) const
{
    QString type = parameter->getType();

    return type.isEmpty() || type == "bit" || type == "byte" || type == "shortint" ||
        type == "int" || type == "longint" || type == "shortreal" || type == "real" || 
        type == "string";
}

//-----------------------------------------------------------------------------
// Function: SystemVerilogValidator::hasValidValueForType()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(QString const& value, QString const& type) const
{
    if (type.isEmpty())
    {
        return true;
    }

    if (!expressionParser_->isValidExpression(value))
    {
        return false;
    }

    bool canConvert = false;
    QString solvedValue = expressionParser_->parseExpression(value);

    if (type == "bit")
    {
        QRegularExpression bitExpression("^([01]|[1-9]+[0-9]*'([bB][01]+|[hH][0-9a-fA-F]+))$");
        return bitExpression.match(value).hasMatch() || bitExpression.match(solvedValue).hasMatch();
    }
    else if (type == "byte")
    {
        solvedValue.toShort(&canConvert);
        return canConvert && -128 <= solvedValue.toShort() && solvedValue.toShort() <= 127;
    }
    else if (type == "shortint")
    {
        solvedValue.toShort(&canConvert);
    }
    else if (type == "int")
    {
        solvedValue.toInt(&canConvert);
    }
    else if (type == "longint")
    {
        if (solvedValue.startsWith("-"))
        {
            solvedValue.toLongLong(&canConvert);
        }
        else
        {
            solvedValue.toULongLong(&canConvert);
        }
    }
    else if (type == "shortreal")
    {
        solvedValue.toFloat(&canConvert);
    }
    else if (type == "real")
    {
        solvedValue.toDouble(&canConvert);
    }
    else if (type == "string")
    {
        QRegularExpression stringExpression("^\\s*\".*\"\\s*$");
        return stringExpression.match(value).hasMatch();
    }

    return canConvert;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidValueForFormat()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidValueForType(Parameter const* parameter) const
{
    return hasValidValueForType(parameter->getValue(), parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMinimumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMinimumValue(Parameter const* parameter) const
{
    QString minimumValue = parameter->getMinimumValue();

    if (!shouldCompareValueAndBoundary(minimumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(minimumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::hasValidMaximumValue()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::hasValidMaximumValue(Parameter const* parameter) const
{
    QString maximumValue = parameter->getMaximumValue();

    if (!shouldCompareValueAndBoundary(maximumValue, parameter->getType()))
    {
        return true;
    }

    return hasValidValueForType(maximumValue, parameter->getType());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsLessThanMinimum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsLessThanMinimum(Parameter const* parameter) const
{
    QString minimum = parameter->getMinimumValue();
    QString type = parameter->getType();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(minimum, type) && valueOf(value, type) < valueOf(minimum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueIsGreaterThanMaximum()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::valueIsGreaterThanMaximum(Parameter const* parameter) const
{
    QString maximum = parameter->getMaximumValue();
    QString type = parameter->getType();
    QString value = parameter->getValue();

    return shouldCompareValueAndBoundary(maximum, type) && valueOf(value, type) > valueOf(maximum, type);
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsIn()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsIn(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList errors;
    errors.append(findErrorsInName(parameter, context));
    errors.append(findErrorsInValue(parameter, context, availableChoices));
    errors.append(findErrorsInType(parameter, context));
    errors.append(findErrorsInMinimumValue(parameter, context));
    errors.append(findErrorsInMaximumValue(parameter, context));
    errors.append(findErrorsInChoice(parameter, context, availableChoices));
    errors.append(findErrorsInResolve(parameter, context));

    return errors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::shouldCompareValueAndBoundary()
//-----------------------------------------------------------------------------
bool ParameterValidator2014::shouldCompareValueAndBoundary(QString const& boundaryValue, QString const& type) const
{
     return expressionParser_->isValidExpression(boundaryValue) && 
         (type != "bit" && type != "string" && !type.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::valueOf()
//-----------------------------------------------------------------------------
qreal ParameterValidator2014::valueOf(QString const& value, QString const& type) const
{
    if (type == "real" || type == "shortreal")
    {
        return expressionParser_->parseExpression(value).toDouble();
    }
    else
    {
        return expressionParser_->parseExpression(value).toLongLong();
    }
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInType()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInType(Parameter const* parameter, QString const& context) const
{
    QStringList typeErrors;

    if (!hasValidType(parameter))
    {
        typeErrors.append(QObject::tr("Invalid type %1 specified for %2 %3 within %4").arg(
            parameter->getType(), parameter->elementName(), parameter->getName(), context));
    }

    return typeErrors;
}

//-----------------------------------------------------------------------------
// Function: ParameterValidator2014::findErrorsInValue()
//-----------------------------------------------------------------------------
QStringList ParameterValidator2014::findErrorsInValue(Parameter const* parameter, QString const& context, 
    QSharedPointer<QList<QSharedPointer<Choice> > > availableChoices) const
{
    QStringList valueErrors;

    if (parameter->getValue().isEmpty())
    {
        valueErrors.append(QObject::tr("No value specified for %1 %2 within %3").arg(
            parameter->elementName(), parameter->getName(), context));
    }
    else
    {
        if (!hasValidValueForType(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 is not valid for type %2 in %3 %4 within %5").arg(
                parameter->getValue(), parameter->getType(), parameter->elementName(), 
                parameter->getName(), context));
        }

        if (valueIsLessThanMinimum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates minimum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getMinimumValue(), 
                parameter->elementName(), parameter->getName(), context));
        }

        if (valueIsGreaterThanMaximum(parameter))
        {
            valueErrors.append(QObject::tr("Value %1 violates maximum value %2 in %3 %4 within %5"
                ).arg(parameter->getValue(), parameter->getMaximumValue(), 
                parameter->elementName(), parameter->getName(), context));
        }

        if (!hasValidValueForChoice(parameter, availableChoices))
        {           
            valueErrors.append(QObject::tr("Value %1 references unknown enumeration for choice "
                "%2 in %3 %4 within %5").arg(parameter->getValue(), parameter->getChoiceRef(), 
                parameter->elementName(), parameter->getName(), context));
        }
    }

    return valueErrors;
}
