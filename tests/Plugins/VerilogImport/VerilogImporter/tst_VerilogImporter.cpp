//-----------------------------------------------------------------------------
// File: tst_VerilogImporter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Unit test for class VerilogImporter.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>
#include <QTextCursor>
#include <QPlainTextEdit>

#include <Plugins/VerilogImport/VerilogImporter.h>

#include <IPXACTmodels/component.h>

#include <common/KactusColors.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

class tst_VerilogImporter : public QObject
{
    Q_OBJECT

public:
    tst_VerilogImporter();

private slots:
    void init();
    void testNothingIsParsedFromMalformedInput();
    void testNothingIsParsedFromMalformedInput_data();

    void testPortIsHighlighted();
    void testPortIsHighlighted_data();

private:

    void runParser(QString const& input);

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, 
        QColor const& highlightColor);
    void verifyDeclarationIsHighlighted(const int declarationStartIndex, 
        const int declarationLength, QColor const& expectedHighlight) const;
    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
        const int declarationLength, QColor const& highlightColor) const;

    QSharedPointer<Component> importComponent_;

    QPlainTextEdit displayEditor_;

    ImportHighlighter* highlighter_;
};

tst_VerilogImporter::tst_VerilogImporter(): importComponent_(new Component()), displayEditor_(), 
    highlighter_(new ImportHighlighter(&displayEditor_, this))
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::init()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput()
{
    QFETCH(QString, input);

    runParser(input);

    QCOMPARE(importComponent_->getPorts().count(), 0);
    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testNothingIsParsedFromMalformedInput_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty input") << "";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::runParser()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::runParser(QString const& input)
{
    displayEditor_.setPlainText(input);

    VerilogImporter parser;
    parser.setHighlighter(highlighter_);
    parser.runParser(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testPortIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, portDeclaration);

    runParser(fileContent);

    QVERIFY2(importComponent_->getPorts().count() != 0, "No ports parsed from input.");

    int begin = fileContent.indexOf(portDeclaration);

    QColor selectedColor = KactusColors::SW_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(begin, selectedColor);
    verifyDeclarationIsHighlighted(begin, portDeclaration.length(), selectedColor);
    verifyNotHighlightedAfterDeclartion(begin, portDeclaration.length(), selectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::testPortIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("portDeclaration");

    QTest::newRow("Simplest possible port declaration") <<
        "module test (\n"
        "        input clk\n"
        "   );\n"
        "endmodule"
        << "input clk";   

    QTest::newRow("Port declaration with description") <<
        "module test (\n"
        "        input clk // Clk from top\n"
        "   );\n"
        "endmodule"
        << "input clk // Clk from top";
    
    QTest::newRow("Port declaration followed by another port") <<
        "module TEST(\n"
        "        input clk, // Clk from top\n"
        "        input enable\n"
        "   );\n"
        "endmodule"
        << "input clk, // Clk from top";
    
    QTest::newRow("Port declaration with multiple ports") <<
        "module test (\n"
        "        input clk, enable //One bit ports\n"
        "   );\n"
        "endmodule"
        << "input clk, enable //One bit ports";
    
    QTest::newRow("Direction, type and name on separate lines") << 
        "module test (input\n"
        "reg\n"
        "clk // Clk from top.\n"
        ");\n"
        "endmodule;"
        << 
        "input\n"
        "reg\n"
        "clk // Clk from top.";
    
    QTest::newRow("Double comment line") << 
        "module test (\n"
        "    input clk,\n"
        "    // first comment.\n"
        "    //second comment.\n"
        "    // third comment.\n"
        "    input rst_n\n"
        "    );\n"
        "endmodule;"
        << 
        "input clk,";

    QTest::newRow("Simplest possible 1995-style port declaration") <<
        "module test (clk);"
        "   input clk;\n"
        "endmodule"
        << "input clk;";

    QTest::newRow("1995-style port declaration followed by another port") <<
        "module TEST(clk, enable);\n"
        "        input clk; // Clk from top\n"
        "        input enable;\n"
        "endmodule"
        << "input clk; // Clk from top";


    QTest::newRow("1995-stype port declarations with multiple ports") <<
        "module test (clk, enable);\n"
        "        input clk, enable; //One bit ports\n"
        "endmodule"
        << "input clk, enable; //One bit ports";

    QTest::newRow("1995-stype port with direction and name on separate lines") << 
        "module test (clk);\n"
        "input\n"        
        "clk ; // Clk from top.\n"
        "endmodule;"
        << 
        "input\n"        
        "clk ; // Clk from top.";
        
    QTest::newRow("1995-style ports with double comment line") << 
        "module test (clk, rst_n); \n"
        "    input clk;\n"
        "    // first comment.\n"
        "    //second comment.\n"
        "    // third comment.\n"
        "    input rst_n\n"
        "\n"
        "endmodule;"
        << 
        "input clk;";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, 
    QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    if (cursor.atBlockStart())
    {
        cursor.setPosition(declarationStartIndex - 1);
    }

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
    const int declarationLength, QColor const& expectedHighlight) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    for (int i = 1; i <= declarationLength; i++)
    {
        cursor.setPosition(declarationStartIndex + i);

        QColor appliedColor = cursor.charFormat().background().color();

        if (appliedColor != expectedHighlight)
        {
            QString correctlyHighligted(displayEditor_.toPlainText().mid(declarationStartIndex, i - 1));
            QString errorMessage("Highlight ended after '" + correctlyHighligted + "'");

            QFAIL(errorMessage.toLocal8Bit());           
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogImporter::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VerilogImporter::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex + declarationLength + 1);

    QColor appliedColor = cursor.charFormat().background().color();

    QVERIFY2(appliedColor != highlightColor, "Highlight applied after declaration.");
}

QTEST_MAIN(tst_VerilogImporter)

#include "tst_VerilogImporter.moc"
