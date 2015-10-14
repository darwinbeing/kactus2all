//-----------------------------------------------------------------------------
// File: tst_businterfaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 22.09.2015
//
// Description:
// Unit test for class businterfaceReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/businterfaceReader.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/VendorExtension.h>

#include <QtTest>

class tst_businterfaceReader : public QObject
{
    Q_OBJECT

public:
    tst_businterfaceReader();

private slots:

    void testReadSimplebusinterface();
	void testReadIsPresent();
	void testReadBusType();
	void testReadSlave();
	void testReadMaster();
	void testReadSystem();
	void testReadMirroredSlave();
	void testReadMirroredMaster();
	void testReadMirroredSystem();
	void testReadMonitor();
	void testReadAttributes();
	void testReadConnectionRequired();
	void testReadBitsInLau();
	void testReadBitSteering();
	void testReadEndianness();
	void testReadParameters();
	void testReadVendorExtensions();
	void testReadVendorExtensions2();
	void testReadAbstractionReference();
	void testReadPortMaps();
	void testReadPortMaps2();
};

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::tst_businterfaceReader()
//-----------------------------------------------------------------------------
tst_businterfaceReader::tst_businterfaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSimplebusinterface()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSimplebusinterface()
{
    QString documentContent(
        "<ipxact:busInterface>"
            "<ipxact:name>businterface</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:busInterface>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

    businterfaceReader businterfaceReader;
    QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->name(), QString("businterface"));
    QCOMPARE(testbusinterface->displayName(), QString("viewDisplay"));
    QCOMPARE(testbusinterface->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:busInterface>"
            "<ipxact:name>testbusinterface</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:busInterface>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

    businterfaceReader businterfaceReader;
    QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->name(), QString("testbusinterface"));
    QCOMPARE(testbusinterface->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBusType()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBusType()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:busType ipxact:vendor=\"joq.org\""
		"ipxact:library=\"busdef.clock\" ipxact:name=\"clock\" ipxact:version=\"1.0\"/>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);
	VLNV refVLNV = VLNV(VLNV::BUSDEFINITION,"joq.org","busdef.clock","clock","1.0");

	QCOMPARE(testbusinterface->getBusType(), refVLNV);
	QCOMPARE(testbusinterface->getBusType().getType(),VLNV::BUSDEFINITION);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSlave()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:slave>"
		"<ipxact:memoryMapRef memoryMapRef=\"ambaAHB\"/>"
		"<ipxact:transparentBridge masterRef=\"joq\">"
		"<ipxact:isPresent>poeoe</ipxact:isPresent>"
		"</ipxact:transparentBridge>"
		"<ipxact:fileSetRefGroup>"
		"<ipxact:group>joq</ipxact:group>"
		"<ipxact:fileSetRef>eka</ipxact:fileSetRef>"
		"<ipxact:fileSetRef>toka</ipxact:fileSetRef>"
		"</ipxact:fileSetRefGroup>"
		"</ipxact:slave>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSlave()->getMemoryMapRef(), QString("ambaAHB"));
	QSharedPointer<SlaveInterface::Bridge> bridge = testbusinterface->getSlave()->getBridges().first();
	QCOMPARE(bridge->masterRef_, QString("joq"));
	QCOMPARE(bridge->isPresent_, QString("poeoe"));

	QSharedPointer<SlaveInterface::FileSetRefGroup> refg = testbusinterface->getSlave()->getFileSetRefGroup().first();
	QCOMPARE(refg->group_, QString("joq"));
	QCOMPARE(refg->fileSetRefs_.size(), 2);
	QCOMPARE(refg->fileSetRefs_.first(), QString("eka"));
	QCOMPARE(refg->fileSetRefs_.last(), QString("toka"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMaster()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:master>"
		"<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		"</ipxact:master>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMaster()->getAddressSpaceRef(), QString("apb"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadSystem()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:system>"
		"<ipxact:group>esa</ipxact:group>"
		"</ipxact:system>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSystem(), QString("esa"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredSlave()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredSlave()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:mirroredSlave>"
		"<ipxact:baseAddresses>"
		"<ipxact:remapAddress state=\"decayed\" joku=\"jotain\">"
		"0x000000F0</ipxact:remapAddress>"
		"<ipxact:remapAddress state=\"uptodate\" yks=\"kaks\">"
		"0xBEEF</ipxact:remapAddress>"
	    "<ipxact:range>0x00010000</ipxact:range>"
	    "</ipxact:baseAddresses>"
	    "</ipxact:mirroredSlave>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().first()->remapAddress_, QString("0x000000F0"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().first()->state_, QString("decayed"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().first()->remapAttributes_.first(), QString("jotain"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().first()->remapAttributes_.firstKey(), QString("joku"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().last()->remapAddress_, QString("0xBEEF"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().last()->state_, QString("uptodate"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().last()->remapAttributes_.first(), QString("kaks"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRemapAddresses().last()->remapAttributes_.firstKey(), QString("yks"));
	QCOMPARE(testbusinterface->getMirroredSlave()->getRange(), QString("0x00010000"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredMaster()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredMaster()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:mirroredMaster>"
		"<ipxact:addressSpaceRef ipxact:addressSpaceRef=\"apb\"/>"
		"</ipxact:mirroredMaster>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMaster()->getAddressSpaceRef(), QString("apb"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMirroredSystem()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMirroredSystem()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:mirroredSystem>"
		"<ipxact:group>esa</ipxact:group>"
		"</ipxact:mirroredSystem>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getSystem(), QString("esa"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadMonitor()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadMonitor()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:monitor interfaceMode=\"system\">"
		"<ipxact:group>esa</ipxact:group>"
		"</ipxact:monitor>"
		"</ipxact:busInterface>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getMonitor()->group_, QString("esa"));
	QCOMPARE(testbusinterface->getMonitor()->interfaceMode_, General::SYSTEM);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadAttributes()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadAttributes()
{
	QString documentContent(
		"<ipxact:busInterface joku=\"jotain\">"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getAttributes().firstKey(), QString("joku"));
	QCOMPARE(testbusinterface->getAttributes().first(), QString("jotain"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadConnectionRequired()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadConnectionRequired()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:connectionRequired>true</ipxact:connectionRequired>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->getConnectionRequired(), QString("true"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBitsInLau()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBitsInLau()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:bitsInLau>1234</ipxact:bitsInLau>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

    QCOMPARE(testbusinterface->getBitsInLau(), QString("1234"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadBitSteering()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadBitSteering()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:bitSteering joku=\"jotain\">on</ipxact:bitSteering>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getBitSteering(), General::ON);
	QCOMPARE(testbusinterface->getBitSteeringAttributes().firstKey(), QString("joku"));
	QCOMPARE(testbusinterface->getBitSteeringAttributes().first(), QString("jotain"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadEndianness()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadEndianness()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:endianness>big</ipxact:endianness>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getEndianness(), General::BIG);
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		"type=\"shortint\" resolve=\"user\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"<ipxact:parameter parameterId=\"ev0\" prompt=\"Parm 1\""
		"type=\"shortint\" resolve=\"user\">"
		"<ipxact:name>Mikko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>1337</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getParameters()->size(), 2);
	QCOMPARE(testbusinterface->getParameters()->first()->name(), QString("Esko"));
	QCOMPARE(testbusinterface->getParameters()->first()->getValueId(), QString("joq"));
	QCOMPARE(testbusinterface->getParameters()->first()->getValue(), QString("5"));
	QCOMPARE(testbusinterface->getParameters()->last()->name(), QString("Mikko"));
	QCOMPARE(testbusinterface->getParameters()->last()->getValueId(), QString("ev0"));
	QCOMPARE(testbusinterface->getParameters()->last()->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadVendorExtensions()
{
	QString documentContent(
		"<ipxact:busInterface>"
		    "<ipxact:vendorExtensions>"
        		"<ulina kolina=\"eaa\"/>"
	        	"<kactus2:position x=\"1\" y=\"5\" />"
	    	"</ipxact:vendorExtensions>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getVendorExtensions()->last()->type(), QString("ulina"));
    QCOMPARE(testbusinterface->getVendorExtensions()->first()->type(), QString("kactus2:position"));
	QCOMPARE(testbusinterface->getDefaultPos().isNull(), false);
    QCOMPARE(testbusinterface->getDefaultPos().x(), qreal(1));
    QCOMPARE(testbusinterface->getDefaultPos().y(), qreal(5));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadVendorExtensions2()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadVendorExtensions2()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:vendorExtensions>"
    		    "<ulina kolina=\"eaa\"/>"
    	    	"<kactus2:position x=\"1\" y=\"5\" />"
	    	"</ipxact:vendorExtensions>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getVendorExtensions()->first()->type(), QString("kactus2:position"));
    QCOMPARE(testbusinterface->getDefaultPos().x(), qreal(1));
    QCOMPARE(testbusinterface->getDefaultPos().y(), qreal(5));
    QCOMPARE(testbusinterface->getVendorExtensions()->last()->type(), QString("ulina"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadAbstractionReference()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadAbstractionReference()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:abstractionTypes>"
		"<ipxact:abstractionType>"
		"<ipxact:viewRef>joku</ipxact:viewRef>"
		"<ipxact:abstractionRef vendor=\"accellera.org\" library=\"Sample\""
		"name=\"SampleAbstractionDefinition_TLM\" version=\"1.0\"/>"
		"</ipxact:abstractionType>"
		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->viewRef_, QString("joku"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->abstractionRef_->getVendor(), QString("accellera.org"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->abstractionRef_->getName(), QString("SampleAbstractionDefinition_TLM"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->abstractionRef_->getLibrary(), QString("Sample"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->abstractionRef_->getVersion(), QString("1.0"));
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadPortMaps()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadPortMaps()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:abstractionTypes>"
		"<ipxact:abstractionType>"
		"<ipxact:portMaps>"
		"<ipxact:portMap>"
		"<ipxact:logicalPort>"
		"<ipxact:name>CLK</ipxact:name>"
		"<ipxact:range>"
		"<ipxact:left>vasen</ipxact:left>"
		"<ipxact:right>oikea</ipxact:right>"
		"</ipxact:range>"
		"</ipxact:logicalPort>"
		"<ipxact:physicalPort>"
		"<ipxact:name>clk</ipxact:name>"
		"<ipxact:partSelect>"
		"<ipxact:range>"
		"<ipxact:left>vasen</ipxact:left>"
		"<ipxact:right>oikea</ipxact:right>"
		"</ipxact:range>"
		"<ipxact:indices>"
		"<ipxact:index>eka</ipxact:index>"
		"<ipxact:index>toka</ipxact:index>"
		"</ipxact:indices>"
		"</ipxact:partSelect>"
		"</ipxact:physicalPort>"
		"<ipxact:logicalTieOff>eoae</ipxact:logicalTieOff>"
		"</ipxact:portMap>"
		"</ipxact:portMaps>"
		"</ipxact:abstractionType>"
		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getLogicalPort()->name_, QString("CLK"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getLogicalPort()->range_->getLeft(), QString("vasen"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getLogicalPort()->range_->getRight(), QString("oikea"));

	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getPhysicalPort()->name_, QString("clk"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getPhysicalPort()->
		partSelect_->getLeftRange(), QString("vasen"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getPhysicalPort()->
		partSelect_->getRightRange(), QString("oikea"));

	QSharedPointer<QStringList > indices = testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->
		getPhysicalPort()->partSelect_->getIndices();

	QCOMPARE(indices->first(), QString("eka"));
	QCOMPARE(indices->last(), QString("toka"));

	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getLogicalTieOff(), QString("eoae") );
}

//-----------------------------------------------------------------------------
// Function: tst_businterfaceReader::testReadPortMaps2()
//-----------------------------------------------------------------------------
void tst_businterfaceReader::testReadPortMaps2()
{
	QString documentContent(
		"<ipxact:busInterface>"
		"<ipxact:abstractionTypes>"
		"<ipxact:abstractionType>"
		"<ipxact:portMaps>"
		"<ipxact:portMap>"
		"<ipxact:logicalPort>"
		"<ipxact:name>CLK</ipxact:name>"
		"</ipxact:logicalPort>"
		"<ipxact:physicalPort>"
		"<ipxact:name>clk</ipxact:name>"
		"</ipxact:physicalPort>"
		"</ipxact:portMap>"
		"</ipxact:portMaps>"
		"</ipxact:abstractionType>"
		"<ipxact:abstractionType>"
		"<ipxact:portMaps>"
		"<ipxact:portMap>"
		"<ipxact:logicalPort>"
		"<ipxact:name>joq</ipxact:name>"
		"</ipxact:logicalPort>"
		"<ipxact:physicalPort>"
		"<ipxact:name>jotain</ipxact:name>"
		"</ipxact:physicalPort>"
		"</ipxact:portMap>"
		"</ipxact:portMaps>"
		"</ipxact:abstractionType>"
		"</ipxact:abstractionTypes>"
		"</ipxact:busInterface>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode businterfaceNode = document.firstChildElement("ipxact:busInterface");

	businterfaceReader businterfaceReader;
	QSharedPointer<BusInterface> testbusinterface = businterfaceReader.createbusinterfaceFrom(businterfaceNode);

	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getLogicalPort()->name_, QString("CLK"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->first()->portMaps_->first()->getPhysicalPort()->name_, QString("clk"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->last()->portMaps_->first()->getLogicalPort()->name_, QString("joq"));
	QCOMPARE(testbusinterface->getAbstractionTypes()->last()->portMaps_->first()->getPhysicalPort()->name_, QString("jotain"));
}

QTEST_APPLESS_MAIN(tst_businterfaceReader)

#include "tst_businterfaceReader.moc"
