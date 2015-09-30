//-----------------------------------------------------------------------------
// File: tst_CPUWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for class CPUWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/CPUWriter.h>
#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/GenericVendorExtension.h>

#include <QtTest>

class tst_CPUWriter : public QObject
{
    Q_OBJECT

public:
	tst_CPUWriter();

private slots:
	void init();
	void cleanup();

	void testWriteCPUNameGroup();
	void testWriteIsPresent();
	void testWriteParameter();
	void testWriteAddressSpaces();
	void testWriteVendorExtension();

private:

	QSharedPointer<Cpu> testCPU_;
};

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::tst_CPUWriter()
//-----------------------------------------------------------------------------
tst_CPUWriter::tst_CPUWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::init()
//-----------------------------------------------------------------------------
void tst_CPUWriter::init()
{
	testCPU_ = QSharedPointer<Cpu>(new Cpu());
	testCPU_->setName("testCpu");
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_CPUWriter::cleanup()
{
	testCPU_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteCPUNameGroup()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteCPUNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QString expectedOutput(
		"<ipxact:cpu>"
		"<ipxact:name>testCpu</ipxact:name>"
		"</ipxact:cpu>"
		);

	CPUWriter cpuWriter;
	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);
	QCOMPARE(output, expectedOutput);

	expectedOutput.clear();
	output.clear();

	testCPU_->setDisplayName("testDisplay");
	expectedOutput = 
		"<ipxact:cpu>"
		"<ipxact:name>testCpu</ipxact:name>"
		"<ipxact:displayName>testDisplay</ipxact:displayName>"
		"</ipxact:cpu>"
		;

	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);
	QCOMPARE(output, expectedOutput);

	output.clear();
	expectedOutput.clear();

	testCPU_->setDescription("testDescription");
	expectedOutput = 
		"<ipxact:cpu>"
		"<ipxact:name>testCpu</ipxact:name>"
		"<ipxact:displayName>testDisplay</ipxact:displayName>"
		"<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:cpu>"
		;

	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteIsPresent()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setIsPresent("4-3");

	QString expectedOutput(
		"<ipxact:cpu>"
		"<ipxact:name>testCpu</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
		"</ipxact:cpu>"
		);

	CPUWriter cpuWriter;
	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteParameter()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteParameter()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setIsPresent("4-3");

	QSharedPointer<Parameter> para1(new Parameter());
	para1->setName("Esko");
	para1->setValueId("joq");
	para1->setValue("5");
	para1->setType("shortint");
	para1->setAttribute("prompt", "Parm 1");
	para1->setAttribute("resolve", "user");
	para1->setDescription("First generator parameter.");

	QList<QSharedPointer<Parameter> >& parameters = testCPU_->getParameters();
	parameters.append(para1);

	QString expectedOutput(
		"<ipxact:cpu>"
		"<ipxact:name>testCpu</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		" resolve=\"user\" type=\"shortint\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:cpu>"
		);

	CPUWriter cpuWriter;
	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteAddressSpaces()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteAddressSpaces()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setName("joqCPU");

	QStringList addressSpaces;
	addressSpaces.append("default_interconnect");
	testCPU_->setAddressSpaceRefs(addressSpaces);

	QString expectedOutput(
		"<ipxact:cpu>"
		"<ipxact:name>joqCPU</ipxact:name>"
		"<ipxact:addressSpaceRef addressSpaceRef=\"default_interconnect\"/>"
		"</ipxact:cpu>"
		);

	CPUWriter cpuWriter;
	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testWriteVendorExtension()
//-----------------------------------------------------------------------------
void tst_CPUWriter::testWriteVendorExtension()
{
	QDomDocument document;
	QDomElement extensionNode = document.createElement("ulina");
	extensionNode.setAttribute("kolina", "eaa");
	extensionNode.appendChild(document.createTextNode("testValue"));

	QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

	testCPU_->getVendorExtensions()->append(testExtension);

	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testCPU_->setName("joq");

	QString expectedOutput(
		"<ipxact:cpu>"
		"<ipxact:name>joq</ipxact:name>"
		"<ipxact:vendorExtensions>"
		"<ulina kolina=\"eaa\">"
		"testValue"
		"</ulina>"
		"</ipxact:vendorExtensions>"
		"</ipxact:cpu>"
		);

	CPUWriter cpuWriter;
	cpuWriter.writeCPU(xmlStreamWriter, testCPU_);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_CPUWriter)

#include "tst_CPUWriter.moc"