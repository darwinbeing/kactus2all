//-----------------------------------------------------------------------------
// File: VerilogGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2014
//
// Description:
// Plugin for structural verilog generation.
//-----------------------------------------------------------------------------

#include "VerilogGeneratorPlugin.h"

#include "VerilogGenerator/VerilogGenerator.h"

#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfigurationDialog.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::VerilogGeneratorPlugin()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0), outputFile_(),
    configuration_(new GeneratorConfiguration())
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::~VerilogGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getName() const
{
    static QString name(tr("Verilog Generator"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getVersion() const
{
    static QString version(tr("1.1"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getDescription() const
{
    static QString description(tr("Generates a top-level Verilog module for a HW design or component."));
    return description;
}
//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getVendor() const
{
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getLicenceHolder() const
{
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VerilogGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon VerilogGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/verilogGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes) const
{
    QSharedPointer<const Component> targetComponent = libComp.dynamicCast<const Component>();
    
    return targetComponent && targetComponent->getComponentImplementation() == KactusAttribute::KTS_HW;    
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    utility_ = utility;
    topComponent_ = libComp.dynamicCast<Component>();

    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    if (couldConfigure(findPossibleViewNames(libComp, libDes, libDesConf)))
    {
        QSharedPointer<GeneratorConfiguration> configuration = getConfiguration();

        outputFile_ = configuration->getOutputPath();

        utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));
        
        VerilogGenerator generator(utility->getLibraryInterface());
        generator.parse(topComponent_, configuration_->getActiveViewName(), libDes.dynamicCast<Design>());
        generator.generate(outputFile_);

        utility_->printInfo(tr("Finished writing file %1.").arg(outputFile_));

        if (configuration->getSaveToFileset())
        {          
            QString viewName = configuration->getActiveViewName();
            addGeneratedFileToFileSet(viewName);
            addRTLViewToTopComponent(viewName);
            saveChanges();
        }

        utility_->printInfo(tr("Generation complete."));
    }    
    else
    {
        utility_->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findPossibleViewNames()
//-----------------------------------------------------------------------------
QStringList VerilogGeneratorPlugin::findPossibleViewNames(QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDes, QSharedPointer<LibraryComponent> libDesConf) const
{
    QSharedPointer<Component> topComponent = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration> designConfig = libDesConf.dynamicCast<DesignConfiguration>();

    QStringList viewNames;
    if (designConfig && libDes && designConfig->getDesignRef() == *libDes->getVlnv())
    {        
        viewNames = findReferencingViews(topComponent, *designConfig->getVlnv());
    }
    else if (libDes)
    {
        viewNames = findReferencingViews(topComponent, *libDes->getVlnv());
    }
    else
    {
        viewNames = topComponent->getFlatViews();
    }

    return viewNames;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::couldConfigure()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::couldConfigure(QStringList const& possibleViewNames) const
{
    if (!possibleViewNames.isEmpty())
    {
        configuration_->setActiveView(possibleViewNames.first());
    }

    configuration_->setOutputPath(defaultOutputPath());
    configuration_->setSaveToFileset(outputFileAndViewShouldBeAddedToTopComponent());

    GeneratorConfigurationDialog dialog(configuration_, utility_->getParentWidget());
    dialog.setViewNames(possibleViewNames);
    return dialog.exec() == QDialog::Accepted;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<GeneratorConfiguration> VerilogGeneratorPlugin::getConfiguration()
{
    return configuration_;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::defaultOutputPath()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::defaultOutputPath() const
{
    QString suggestedFile = "";

    QString topComponentPath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
    QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
    suggestedFile = xmlDir + "/" + topComponent_->getVlnv()->getName() + ".v";

    return suggestedFile;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::fileShouldBeAddedToFileset()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::outputFileAndViewShouldBeAddedToTopComponent() const
{
    QString filePath = relativePathFromXmlToFile(outputFile_);
    return !topComponent_->hasFile(filePath);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::relativePathForFile()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::relativePathFromXmlToFile(QString const& filePath) const
{
    QString xmlPath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
    return General::getRelativePath(xmlPath, filePath);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::addFileToFileSet()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::addGeneratedFileToFileSet(QString const& activeViewName) const
{
    QString filePath = relativePathFromXmlToFile(outputFile_);

    QSettings settings;
    QSharedPointer<FileSet> fileSet = topComponent_->getFileSet(fileSetNameForActiveView(activeViewName));
    fileSet->addFile(filePath, settings);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::fileSetNameForActiveView()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::fileSetNameForActiveView(QString const& activeViewName) const
{
    View* activeView = topComponent_->findView(activeViewName);
    if (topComponent_->hasView(activeViewName) && activeView->isHierarchical())
    {
        return activeViewName + "_verilogSource";
    }
    else
    {
        return "verilogSource";
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::addRTLViewToTopComponent()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::addRTLViewToTopComponent(QString const& activeViewName) const
{    
    View* rtlView = 0;

    View* activeView = topComponent_->findView(activeViewName);
    if (topComponent_->hasView(activeViewName))
    {    
        if (activeView->isHierarchical())
        {
            QString structuralViewName = activeViewName + "_verilog";    
            rtlView = new View();    
            rtlView->setName(structuralViewName);   
            topComponent_->addView(rtlView);

            activeView->setTopLevelView(structuralViewName);
        }
        else
        {
            rtlView = activeView;
        }
    }
    else
    {
        rtlView = new View();
        rtlView->setName("rtl");
        topComponent_->addView(rtlView);
    }

 
    rtlView->setLanguage("verilog");
    rtlView->setEnvIdentifiers(QStringList("verilog:Kactus2:"));
    rtlView->setModelName(topComponent_->getVlnv()->getName());    

    QStringList fileSetRefs;
    fileSetRefs << fileSetNameForActiveView(activeViewName);        
    rtlView->setFileSetRefs(fileSetRefs);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::saveChanges()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::saveChanges() const
{
    QString component = topComponent_->getVlnv()->toString();

    bool saveSucceeded = utility_->getLibraryInterface()->writeModelToFile(topComponent_);            
    if (saveSucceeded)
    {
        utility_->printInfo(tr("Saved changes to component %1.").arg(component));
    }    
    else
    {
        QString savePath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
        utility_->printError(tr("Could not write component %1 to file %2.").arg(component, savePath));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::findReferencingViews()
//-----------------------------------------------------------------------------
QStringList VerilogGeneratorPlugin::findReferencingViews(QSharedPointer<Component> containingComponent, 
    VLNV targetReference) const
{
    QStringList hierViews;
    foreach(QSharedPointer<View> view, containingComponent->getViews())
    {
        if (view->getHierarchyRef() == targetReference)
        {
            if (!view->getTopLevelView().isEmpty())
            {
                hierViews.append(view->getTopLevelView());
            }
            else
            {
                hierViews.append(view->getName());
            }
        }
    }

    return hierViews;
}
