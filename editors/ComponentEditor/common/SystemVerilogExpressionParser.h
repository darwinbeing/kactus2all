//-----------------------------------------------------------------------------
// File: SystemVerilogExpressionParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVERILOGEXPRESSIONPARSER_H
#define SYSTEMVERILOGEXPRESSIONPARSER_H

#include <QString>

//-----------------------------------------------------------------------------
//! Parser for SystemVerilog expressions.
//-----------------------------------------------------------------------------
class SystemVerilogExpressionParser 
{
public:

	//! The constructor.
	SystemVerilogExpressionParser();

	//! The destructor.
	~SystemVerilogExpressionParser();

    /*!
     *  Parses an expression to decimal number.
     *
     *      @param [in] expression   The expression to parse.
     *
     *      @return The decimal value of the constant.
     */
    QString parseExpression(QString const& expression) const;

    /*!
     *  Parses a constant number to decimal number.
     *
     *      @param [in] constantNumber   The constant to parse.
     *
     *      @return The decimal value of the constant.
     */
    int parseConstant(QString const& constantNumber) const;

private:

	// Disable copying.
	SystemVerilogExpressionParser(SystemVerilogExpressionParser const& rhs);
	SystemVerilogExpressionParser& operator=(SystemVerilogExpressionParser const& rhs);

    /*!
     *  Checks if the given expression is not valid for parsing.
     *
     *      @param [in] expression   The expression to check.
     *
     *      @return True, if the expression is not in valid format, otherwise false.
     */
    bool isNotValidExpression(QString const& expression) const;

    /*!
     *  Splits the given expression to string list with terms and operations as separate items.
     *
     *      @param [in] expression   The expression to split.
     *
     *      @return The separated list.
     */
    QStringList toStringList(QString const& expression) const;

    /*!
     *  Splits the given operand to string list with terms and parentheses as separate items.
     *
     *      @param [in] operand   The operand to split.
     *
     *      @return The separated list.
     */
    QStringList parseLiteralAndParentheses(QString const& operand) const;

    /*!
     *  Solves expressions in parentheses in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the expression in parenthesis has been replaced with the solved result
     *              and without the parentheses.
     */
    QStringList solveExpressionsInParentheses(QStringList const& equation) const;

    /*!
     *  Finds the matching end parenthesis in the given equation for the opening parenthesis in given position.
     *
     *      @param [in] equation            The equation to search for ending parenthesis.
     *      @param [in] parenthesesStart    The position of the opening parenthesis.
     *
     *      @return The position of the ending parenthesis.
     */
    int findMatchingEndParenthesis(QStringList const& equation, int parenthesesStart) const;

    /*!
     *  Solves multiply and division operations in a given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the terms and operators have been replaced with the result.
     */
    QStringList solveMultiplyAndDivide(QStringList const& equation) const;

    /*!
     *  Solves a binary operation.
     *
     *      @param [in] firstTerm   The first term of the operation.
     *      @param [in] operation   The operation to solve.
     *      @param [in] secondTerm  The second term of the operation.
     *
     *      @return The result of the operation.
     */
    QString solve(int firstTerm, QString const& operation, int secondTerm) const;

    /*!
     *  Solves addition and subtraction operations in given equation.
     *
     *      @param [in] equation   The equation to solve.
     *
     *      @return Equation where the terms and operators have been replaced with the result.
     */
    QStringList solveAdditionAndSubtraction(QStringList const& equation) const;

    /*!
     *  Converts the base format to the base number e.g. h to 16.
     *
     *      @param [in] baseFormat   The format to convert.
     *
     *      @return The base number for the format.
     */
    int baseForFormat(QString const& baseFormat) const;

};

#endif // SYSTEMVERILOGEXPRESSIONPARSER_H
