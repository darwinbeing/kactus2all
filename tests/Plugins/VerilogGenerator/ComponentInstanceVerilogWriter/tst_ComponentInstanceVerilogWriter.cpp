//-----------------------------------------------------------------------------
// File: tst_ComponentInstanceVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Unit test for class ComponentInstanceVerilogWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/ComponentInstanceVerilogWriter/ComponentInstanceVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/vlnv.h>

#include <QSharedPointer>


class tst_ComponentInstanceVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_ComponentInstanceVerilogWriter();

private slots:
    void init();
    void cleanup();

    // Test cases:
    void testNullComponentAsConstructorParameter();
    void testNamedInstance();
    void testNamedInstance_data();
    void testDescriptionIsPrintedAboveInstance();
    void testDescriptionIsPrintedAboveInstance_data();    
    void testUnconnectedInstancePorts();
    void testFullyConnectedPorts();
    void testPartiallyConnectedPorts();
    void testDefaultPortValueIsUsedForUnconnectedInputPort();
private:

    //! The writer output.
    QString output_;

    //! The output stream for the writer.
    QTextStream outputStream_;

    //! The sorter for the instance ports.
    QSharedPointer<PortSorter> defaultSorter_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::tst_ComponentInstanceVerilogWriter()
//-----------------------------------------------------------------------------
tst_ComponentInstanceVerilogWriter::tst_ComponentInstanceVerilogWriter() : output_(), outputStream_(), 
    defaultSorter_(new InterfaceDirectionNameSorter())
{
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::init()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::init()
{    
    outputStream_.setString(&output_);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::cleanup()
{
    output_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNullComponentAsConstructorParameter()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNullComponentAsConstructorParameter()
{
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    ComponentInstance instance("compInstance", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, QSharedPointer<Component>(0), QSharedPointer<PortSorter>(0));
    writer.write(outputStream_);

    QCOMPARE(output_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNamedInstance()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNamedInstance()
{
    QFETCH(QString, referenceName);
    QFETCH(QString, instanceName);
    QFETCH(QString, expectedOutput);

    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", referenceName, "1.0");
    QSharedPointer<Component> component(new Component(instanceVLNV));
    ComponentInstance instance(instanceName, "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, component, QSharedPointer<PortSorter>(0));
    writer.write(outputStream_);

    QCOMPARE(output_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testNamedInstance_data()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testNamedInstance_data()
{
    QTest::addColumn<QString>("referenceName");
    QTest::addColumn<QString>("instanceName");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty") << "" << "" << " ();\n";
    QTest::newRow("empty reference") << "" << "instance" << " instance();\n";
    QTest::newRow("empty instance name") << "chip" << "" << "chip ();\n";
    QTest::newRow("common") << "TestComponent" << "instance1" << "TestComponent instance1();\n";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionIsPrintedAboveInstance()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testDescriptionIsPrintedAboveInstance()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);

    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));
    refComponent->setDescription(description);

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_);
    writer.write(outputStream_);

    QCOMPARE(output_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionIsPrintedAboveInstance_data()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testDescriptionIsPrintedAboveInstance_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty description") << "" << "TestComponent instance1();\n";
    QTest::newRow("one line description") << "Component description." << 
        "// Component description.\n"
        "TestComponent instance1();\n";

    QTest::newRow("multiline description") << 
        "Description on\n" 
        "multiple\n" 
        "lines." 
        << 
        "// Description on\n"
        "// multiple\n"
        "// lines.\n"
        "TestComponent instance1();\n";;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testUnconnectedInstancePorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testUnconnectedInstancePorts()
{
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));
    refComponent->addPort(QSharedPointer<Port>(new Port("a_in", General::IN, 0, 0, "", true)));
    refComponent->addPort(QSharedPointer<Port>(new Port("b_in", General::IN, 0, 0, "", true)));
    refComponent->addPort(QSharedPointer<Port>(new Port("c_out", General::OUT, 0, 0, "", true)));

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_);
    writer.write(outputStream_);

    QCOMPARE(output_, QString(
        "TestComponent instance1(\n"
        "    .a_in( ),\n"
        "    .b_in( ),\n"
        "    .c_out( ));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testFullyConnectedPorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testFullyConnectedPorts()
{
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));

    refComponent->addPort(QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true)));
    refComponent->addPort(QSharedPointer<Port>(new Port("rst_n", General::IN, 0, 0, "", true)));

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_);
    writer.assignPortForFullWidth("clk", "top_clk");
    writer.assignPortForFullWidth("rst_n", "rst");

    writer.write(outputStream_);

    QCOMPARE(output_, QString("TestComponent instance1(\n"
        "    .clk(top_clk),\n"
        "    .rst_n(rst));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testPartiallyConnectedPorts()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testPartiallyConnectedPorts()
{
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));

    refComponent->addPort(QSharedPointer<Port>(new Port("chip_select", General::IN, 0, 0, "", true)));
    refComponent->addPort(QSharedPointer<Port>(new Port("data", General::IN, 7, 0, "", true)));

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_);
    writer.assignPortForRange("chip_select", "top_select", 0, 0);
    writer.assignPortForRange("data", "top_data", 7, 0);    

    writer.write(outputStream_);

    QCOMPARE(output_, QString("TestComponent instance1(\n"
        "    .chip_select(top_select[0]),\n"
        "    .data(top_data[7:0]));\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDefaultPortValueIsUsedForUnconnectedInputPort()
//-----------------------------------------------------------------------------
void tst_ComponentInstanceVerilogWriter::testDefaultPortValueIsUsedForUnconnectedInputPort()
{
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));

    refComponent->addPort(QSharedPointer<Port>(new Port("a_in", General::IN, 0, 0, "'b0", true)));
    refComponent->addPort(QSharedPointer<Port>(new Port("b_out", General::OUT, 7, 0, "bDefault", true)));    
    refComponent->addPort(QSharedPointer<Port>(new Port("c_inout", General::INOUT, 0, 0, "'cDefault", true)));    

    ComponentInstance instance("instance1", "", "", instanceVLNV, QPointF(), "");

    ComponentInstanceVerilogWriter writer(instance, refComponent, defaultSorter_); 
    writer.write(outputStream_);

    QCOMPARE(output_, QString("TestComponent instance1(\n"
        "    .a_in('b0),\n"
        "    .b_out( ),\n"
        "    .c_inout( ));\n"));
}

QTEST_APPLESS_MAIN(tst_ComponentInstanceVerilogWriter)

#include "tst_ComponentInstanceVerilogWriter.moc"
