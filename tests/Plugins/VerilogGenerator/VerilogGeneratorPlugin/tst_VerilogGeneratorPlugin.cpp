//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/veriloggeneratorplugin_global.h>
#include <Plugins/VerilogGenerator/VerilogGeneratorPlugin.h>

#include <Plugins/PluginSystem/PluginUtilityAdapter.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/FileSet.h>

#include <tests/MockObjects/LibraryMock.h>
#include "IPXACTmodels/Component/View.h"
#include "Plugins/common/NameGenerationPolicy.h"

class tst_VerilogGenerator : public VerilogGeneratorPlugin
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testFilesetIsCreatedWhenRunForComponent();
    void testFilesetIsCreatedWhenRunForDesign();
    void testFilesetIsCreatedWhenRunForDesign_data();
   
    void testRTLViewIsCreatedWhenRunForComponent();
   
    void testStructuralViewIsCreatedWhenRunForDesign();

    void testStructuralViewIsCreatedWhenRunForDesign_data();
    void testStructuralViewIsCreatedWhenRunWithDesignConfiguration();
    void testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data();

    void testTopLevelViewIsSetWhenMultipleFlatViews();

    void testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView();
    void testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView();

    void testConsecutiveRunsCreateOnlyOneView();

    void testFlatViewsArePossibleForTopComponent();
    void testRefenecedDesignViewIsPossible();
    void testRefenecedDesignConfigurationViewIsPossible();

protected:

    virtual bool couldConfigure(QStringList const& possibleViewNames) const;

    virtual QSharedPointer<GeneratorConfiguration> getConfiguration();

private:

   QSharedPointer<Component> createTestComponent();

   QSharedPointer<Design> createTestDesign() const;

   QSharedPointer<DesignConfiguration> createTestDesignConfig() const;

   void verifyRTLView(QSharedPointer<View> rtlView, QSharedPointer<ComponentInstantiation> cimp);

   void verifyHierarchicalView(QSharedPointer<View> hierView, QString const& viewName, QSharedPointer<ComponentInstantiation> cimp);    


    //! The test mock for library interface.
    LibraryMock library_;

    PluginUtilityAdapter utilityMock_;

    VerilogGeneratorPlugin plugin_;

    QWidget* activeWindow_;

    QSharedPointer<GeneratorConfiguration> configuration_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
tst_VerilogGenerator::tst_VerilogGenerator(): library_(this), 
    utilityMock_(&library_, 0, this), plugin_(), activeWindow_(0), configuration_(new GeneratorConfiguration())
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    library_.clear();
    configuration_ = QSharedPointer<GeneratorConfiguration>(new GeneratorConfiguration());
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    QFile::remove("test.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForComponent()
{
    QSharedPointer<Component> targetComponent = createTestComponent();
	targetComponent->getViews()->append(QSharedPointer<View>( new View("joq") ) );

    runGenerator(&utilityMock_, targetComponent);

    QVERIFY2(QFile::exists("test.v"), "No file created");
    QVERIFY2(targetComponent->hasFileSet("verilogSource"), "No file set created");
    QVERIFY2(targetComponent->getFileSet("verilogSource")->getFileNames().contains("test.v"), 
        "No file added to fileset");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    QSharedPointer<View> structuralView( new View(viewName) );
	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("test_design_insta") );
	structuralView->setDesignInstantiationRef(di->name());
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesign->getVlnv() ) );
	di->setDesignReference(cvr);
    targetComponent->getDesignInstantiations()->append(di);
    targetComponent->getViews()->append(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<Document>(), targetDesign);

    QVERIFY2(QFile::exists("test.v"), "No file created");
    QVERIFY2(targetComponent->hasFileSet(viewName + "_verilogSource"), "No file set created");
    QVERIFY2(targetComponent->getFileSet(viewName + "_verilogSource")->getFileNames().contains("test.v"), 
        "No file added to fileset");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFilesetIsCreatedWhenRunForDesign_data()
{
    QTest::addColumn<QString>("viewName");

    QTest::newRow("structural") << "structural";
    QTest::newRow("hierarchical") << "hierarchical";
    QTest::newRow("design") << "design";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponent()
{
	QSharedPointer<Component> targetComponent = createTestComponent();

    runGenerator(&utilityMock_, targetComponent);
    
    QVERIFY2(targetComponent->hasView("rtl"), "No rtl view created");

    QSharedPointer<View> rtlView = targetComponent->getModel()->findView("rtl");
    verifyRTLView(rtlView, targetComponent->getModel()->findComponentInstantiation(rtlView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

	QSharedPointer<View> structuralView( new View(viewName) );
	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("test_design_insta") );
	structuralView->setDesignInstantiationRef(di->name());
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesign->getVlnv() ) );
	di->setDesignReference(cvr);
	targetComponent->getDesignInstantiations()->append(di);
	targetComponent->getViews()->append(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<Document>(0), targetDesign);

    QVERIFY2(targetComponent->hasView( NameGenerationPolicy::verilogStructuralViewName(viewName)), "No implementation view created");

    QSharedPointer<View> verilogView = targetComponent->getModel()->findView(
		NameGenerationPolicy::verilogStructuralViewName( viewName ) );
    verifyHierarchicalView(verilogView, viewName,
		targetComponent->getModel()->findComponentInstantiation(verilogView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunForDesign_data()
{
    testFilesetIsCreatedWhenRunForDesign_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
{
    QFETCH(QString, viewName);

    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

    targetDesignConfig->setDesignRef(targetDesign->getVlnv());

    QSharedPointer<View> structuralView( new View(viewName) );
	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("test-desgcof-insta") );
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesignConfig->getVlnv() ) );
	disg->setDesignConfigurationReference(cvr);
    structuralView->setDesignConfigurationInstantiationRef(disg->name());
    targetComponent->getViews()->append(structuralView);
	targetComponent->getDesignConfigurationInstantiations()->append(disg);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

	QString verilogViewName = NameGenerationPolicy::verilogStructuralViewName( viewName );
    QVERIFY2(targetComponent->hasView( verilogViewName ), "No implementation view created");

    QSharedPointer<View> verilogView = targetComponent->getModel()->findView( verilogViewName );
    verifyHierarchicalView(verilogView, viewName,
		targetComponent->getModel()->findComponentInstantiation(verilogView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration_data()
{
    testFilesetIsCreatedWhenRunForDesign_data();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testStructuralViewIsCreatedWhenRunWithDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelViewIsSetWhenMultipleFlatViews()
{
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

    targetDesignConfig->setDesignRef(targetDesign->getVlnv());

	QSharedPointer<View> structuralView( new View("structural") );
	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("test-desgcof-insta") );
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesignConfig->getVlnv() ) );
	disg->setDesignConfigurationReference(cvr);
	structuralView->setDesignConfigurationInstantiationRef(disg->name());
	targetComponent->getViews()->append(structuralView);
	targetComponent->getDesignConfigurationInstantiations()->append(disg);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

	QString verilogViewName = NameGenerationPolicy::verilogStructuralViewName( structuralView->name() );
    QVERIFY2(targetComponent->hasView(verilogViewName), "No implementation view created");

    QSharedPointer<View> verilogView = targetComponent->getModel()->findView(verilogViewName);
    verifyHierarchicalView(verilogView, "structural", targetComponent->getModel()->
		findComponentInstantiation(verilogView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunWithInvalidHierarchicalView()
{
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<DesignConfiguration> targetDesignConfig = createTestDesignConfig();
    QSharedPointer<Design> targetDesign = createTestDesign();

	QSharedPointer<View> structuralView( new View("structural") );
	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("test-desgcof-insta") );
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( VLNV(VLNV::DESIGN, "", "", "", "") ) );
	disg->setDesignConfigurationReference(cvr);
	structuralView->setDesignConfigurationInstantiationRef(disg->name());
	targetComponent->getViews()->append(structuralView);

    runGenerator(&utilityMock_, targetComponent, targetDesignConfig, targetDesign);

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");

    QSharedPointer<View> verilogView = targetComponent->getModel()->findView("rtl");
	verifyRTLView(verilogView, targetComponent->getModel()->
		findComponentInstantiation(verilogView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRTLViewIsCreatedWhenRunForComponentWithHierarchicalView()
{    
    QSharedPointer<Component> targetComponent = createTestComponent();

	QSharedPointer<View> structuralView( new View("structural") );
	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("test-desgcof-insta") );
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( VLNV(VLNV::DESIGNCONFIGURATION, "TUT", "TestLib", "TestDesign", "1.0") ) );
	disg->setDesignConfigurationReference(cvr);
	structuralView->setDesignConfigurationInstantiationRef(disg->name());
	targetComponent->getViews()->append(structuralView);

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<Document>(0), QSharedPointer<Document>(0));

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");    

    QSharedPointer<View> verilogView = targetComponent->getModel()->findView("rtl");
	verifyRTLView(verilogView, targetComponent->getModel()->
		findComponentInstantiation(verilogView->getComponentInstantiationRef()));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveRunsCreateOnlyOneView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveRunsCreateOnlyOneView()
{    
    QSharedPointer<Component> targetComponent = createTestComponent();

    runGenerator(&utilityMock_, targetComponent, QSharedPointer<Document>(0), QSharedPointer<Document>(0));
    runGenerator(&utilityMock_, targetComponent, QSharedPointer<Document>(0), QSharedPointer<Document>(0));

    QVERIFY2(targetComponent->hasView("rtl"), "No implementation view created");    
    QCOMPARE(targetComponent->getViews()->size(), 1);    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFlatViewsArePossibleForTopComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFlatViewsArePossibleForTopComponent() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    QSharedPointer<View> firstView( new View("View1") );
    targetComponent->getViews()->append(firstView);

    QSharedPointer<View> secondView( new View("View2") );
    targetComponent->getViews()->append(secondView);

    QSharedPointer<View> hierView( new View("hierView") );
	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("test_design_insta") );
	hierView->setDesignInstantiationRef(di->name());
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesign->getVlnv() ) );
	di->setDesignReference(cvr);
	targetComponent->getDesignInstantiations()->append(di);
    targetComponent->getViews()->append(hierView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, QSharedPointer<Document>(0), 
        QSharedPointer<Document>(0));

    QCOMPARE(possibleViews.count(), 2);
    QVERIFY(possibleViews.contains("View1"));
    QVERIFY(possibleViews.contains("View2"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRefenecedDesignViewIsPossible()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRefenecedDesignViewIsPossible() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();

    QSharedPointer<View> firstView( new View("View1") );
    targetComponent->getViews()->append(firstView);

    QSharedPointer<View> secondView( new View("View2") );
    targetComponent->getViews()->append(secondView);

	QSharedPointer<View> hierView( new View("hierView") );
	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("test_design_insta") );
	hierView->setDesignInstantiationRef(di->name());
	QSharedPointer<ConfigurableVLNVReference> cvr( new ConfigurableVLNVReference( targetDesign->getVlnv() ) );
	di->setDesignReference(cvr);
	targetComponent->getDesignInstantiations()->append(di);
	targetComponent->getViews()->append(hierView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, targetDesign, 
        QSharedPointer<Document>(0));

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("hierView"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testRefenecedDesignConfigurationViewIsPossible()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testRefenecedDesignConfigurationViewIsPossible() 
{    
    QSharedPointer<Component> targetComponent = createTestComponent();
    QSharedPointer<Design> targetDesign = createTestDesign();
    QSharedPointer<DesignConfiguration> targetConfiguration = createTestDesignConfig();
    targetConfiguration->setDesignRef(targetDesign->getVlnv());

    QSharedPointer<View> flatView( new View("flat") );
    targetComponent->getViews()->append(flatView);

	QSharedPointer<View> designView( new View("designView") );
	QSharedPointer<DesignInstantiation> di( new DesignInstantiation("test_design_insta") );
	designView->setDesignInstantiationRef(di->name());
	QSharedPointer<ConfigurableVLNVReference> cvr1( new ConfigurableVLNVReference( targetDesign->getVlnv() ) );
	di->setDesignReference(cvr1);
	targetComponent->getDesignInstantiations()->append(di);
	targetComponent->getViews()->append(designView);

    QSharedPointer<View> designConfigView( new View("designConfigView") );
	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("test_design_conf_insta") );
	designConfigView->setDesignConfigurationInstantiationRef(disg->name());
	QSharedPointer<ConfigurableVLNVReference> cvr2( new ConfigurableVLNVReference( targetConfiguration->getVlnv() ) );
	disg->setDesignConfigurationReference(cvr2);
	targetComponent->getDesignConfigurationInstantiations()->append(disg);
	targetComponent->getViews()->append(designConfigView);

    QStringList possibleViews = findPossibleViewNames(targetComponent, targetDesign, targetConfiguration);

    QCOMPARE(possibleViews.count(), 1);
    QVERIFY(possibleViews.contains("designConfigView"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::couldConfigure()
//-----------------------------------------------------------------------------
bool tst_VerilogGenerator::couldConfigure(QStringList const& possibleViewNames) const
{
    configuration_->setActiveView("");
    if (!possibleViewNames.isEmpty())
    {
        configuration_->setActiveView(possibleViewNames.first());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::configure()
//-----------------------------------------------------------------------------
QSharedPointer<GeneratorConfiguration> tst_VerilogGenerator::getConfiguration()
{
    configuration_->setSaveToFileset(true);
    configuration_->setOutputPath("test.v");

    return configuration_;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_VerilogGenerator::createTestComponent()
{
    QSharedPointer<Component> component(new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "test","1.0")));
    library_.writeModelToFile(".", component);
    return component;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_VerilogGenerator::createTestDesign() const
{
    return QSharedPointer<Design>(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "TestDesign", "1.0")));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createTestDesignConfig()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> tst_VerilogGenerator::createTestDesignConfig() const
{
    return QSharedPointer<DesignConfiguration>(new DesignConfiguration(VLNV(VLNV::DESIGNCONFIGURATION, 
        "TUT", "TestLibrary", "TestDesignConf", "1.0")));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyRTLView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyRTLView(QSharedPointer<View> rtlView, QSharedPointer<ComponentInstantiation> cimp)
{
    QCOMPARE(cimp->getLanguage(), QString("verilog"));
    QCOMPARE(cimp->getFileSetReferences()->first(), QString("verilogSource"));
    QCOMPARE(rtlView->getEnvIdentifiers().first(), QString("verilog:Kactus2:"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyHierarchicalView()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyHierarchicalView(QSharedPointer<View> hierView, QString const& viewName,
	QSharedPointer<ComponentInstantiation> cimp)
{
    QCOMPARE(cimp->getLanguage(), QString("verilog"));
    QCOMPARE(cimp->getFileSetReferences()->first(), QString(viewName + "_verilogSource"));
    QCOMPARE(hierView->getEnvIdentifiers().first(), QString("verilog:Kactus2:"));
}

QTEST_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGeneratorPlugin.moc"
