//-----------------------------------------------------------------------------
// File: McapiCodeGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 27.6.2012
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#include "MCAPICodeGenerator.h"

#include "MCAPISettingsWidget.h"

#include <QtPlugin>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>

#include <editors/CSourceEditor/CSourceWriter.h>
#include <editors/CSourceEditor/CSourceTextEdit.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/fileset.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::MCAPICodeGenerator() : utility_(0)
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::~MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::~MCAPICodeGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getName()
//----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getName() const
{
    static QString name("MCAPI Code Generator");
    return name;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getVersion()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getVersion() const
{
    static QString version("1.0");
    return version;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getDescription()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getDescription() const
{
    static QString desc("Generates MCAPI code templates based on the metadata.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getVendor()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getLicence()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MCAPICodeGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MCAPICodeGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes ) const {

	// MCAPI code generator is only run for SW component editor
	/*if (libDesConf) {
		return false;
	}*/
	
    QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    return libDes != 0 && desgConf != 0 && desgConf->getDesignConfigImplementation() == KactusAttribute::KTS_SYS;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::runGenerator()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::runGenerator( IPluginUtility* utility, 
	QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> /*libDesConf*/,
    QSharedPointer<LibraryComponent> libDes)
{
    utility_ = utility;

	QSharedPointer<Design const> design = libDes.dynamicCast<Design const>();

	foreach ( SWInstance instance, design->getSWInstances() )
	{
		VLNV instanceVLNV = instance.getComponentRef();

		QSharedPointer<LibraryComponent> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
		QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

		// Check if can generate the component, return if cannot
		if ( canGenerateMCAPIComponent(instanceComp) )
		{
			QString dir = QFileInfo(utility->getLibraryInterface()->getPath(instanceVLNV)).absolutePath();

			generateMCAPIForComponent(dir, instanceComp);

			addGeneratedMCAPIToFileset(instanceComp);

			utility_->getLibraryInterface()->writeModelToFile(instanceComp);
		}
	}
}

//-----------------------------------------------------------------------------
// Function: generateHeader()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateHeader(QString const& filename, QSharedPointer<Component> component)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeLine("#ifndef KTSMCAPICODE_H");
    writer.writeLine("#define KTSMCAPICODE_H");
    writer.writeEmptyLine();

    writer.writeInclude("mcapi.h");
    writer.writeEmptyLine();

    writer.writeHeaderComment("Node data.");
    writer.writeEmptyLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");
    
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine("extern mcapi_endpoint_t " + comIf->getName() + ";");

        QString handleName = comIf->getPropertyValues().value("handle_name");

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("extern mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("extern mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("extern mcapi_sclchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("extern mcapi_sclchan_recv_hndl_t " + handleName + ";");
            }
        }

        writer.writeEmptyLine();
    }

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        QString remoteEndpointName = comIf->getPropertyValues().value("remote_endpoint_name", "");
        writer.writeLine("extern mcapi_endpoint_t " + remoteEndpointName + ";");
    }

    writer.writeEmptyLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("extern mcapi_status_t status;");
    writer.writeEmptyLine();

    writer.writeHeaderComment("Functions.");
    writer.writeEmptyLine();

    // Write the initialization function prototype.
    writer.writeLine("/*");
    writer.writeLine(" *  Initializes the MCAPI system and endpoints.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int initializeMCAPI();");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Creates and opens all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int createConnections();");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Closes all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int closeConnections();");
    writer.writeEmptyLine();

    writer.writeLine("#endif // KTSMCAPICODE_H");
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: generateSource()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateSource(QString const& filename, QSharedPointer<Component> component)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeInclude("ktsmcapicode.h", true);
    writer.writeEmptyLine();
    writer.writeHeaderComment("Constants.");
    writer.writeEmptyLine();

    // Write local port IDs.
    writer.writeLine("// Local port IDs.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }
    
        writer.writeLine("const mcapi_port_t " + comIf->getName().toUpper() + "_PORT = " +
                         comIf->getPropertyValues().value("port_id") + ";");
    }

    writer.writeEmptyLine();

    writer.writeHeaderComment("Global variables.");
    writer.writeEmptyLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }
    
        writer.writeLine("mcapi_endpoint_t " + comIf->getName() + ";");

        QString handleName = comIf->getPropertyValues().value("handle_name");

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_sclchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("mcapi_sclchan_recv_hndl_t " + handleName + ";");
            }
        }

        writer.writeEmptyLine();
    }

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        QString remoteEndpointName = comIf->getPropertyValues().value("remote_endpoint_name", "");
        writer.writeLine("mcapi_endpoint_t " + remoteEndpointName + ";");
    }

    writer.writeEmptyLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("mcapi_status_t status;");
    writer.writeEmptyLine();

    generateInitializeMCAPIFunc(writer, component);
    generateCreateConnectionsFunc(writer, component);
    generateCloseConnectionsFunc(writer, component);

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMainTemplate()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMainTemplate(QString const& filename, QSharedPointer<Component> /*component*/)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write includes.
    writer.writeInclude("stdlib.h");
    writer.writeInclude("stdio.h");
    writer.writeEmptyLine();

    writer.writeLine("// This header includes the Kactus2 generated MCAPI code.");
    writer.writeInclude("ktsmcapicode.h");
    writer.writeEmptyLine();

    writer.writeLine("int main(int argc, char* argv[])");
    writer.beginBlock();
    writer.writeLine("// Initialize MCAPI.");

    writer.writeLine("if (initializeMCAPI() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeLine("if (createConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeLine("// TODO: Write your application code here.");
    writer.writeEmptyLine();
    writer.writeLine("// Close connections and finalize MCAPI before exiting.");
    writer.writeLine("if (closeConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();
    writer.writeLine("mcapi_finalize(&status);");
    writer.writeLine("return EXIT_SUCCESS;");

    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::createIndentString()
//-----------------------------------------------------------------------------
QString MCAPICodeGenerator::createIndentString()
{
    QSettings settings;

    // Read indentation settings.
    IndentStyle style = static_cast<IndentStyle>(settings.value("Editor/IndentStyle",
                                                                INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("Editor/IndentWidth", 4).toInt();

    QString indent;

    if (style == INDENT_STYLE_SPACES)
    {
        indent.fill(' ', width);
    }
    else
    {
        indent = "\t";
    }

    return indent;
}

//-----------------------------------------------------------------------------
// Function: generateCreateConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateCreateConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: createConnections()");
    writer.writeLine("int createConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeEmptyLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                // Sender is responsible of calling the connect function.
                writer.writeLine("mcapi_connect_pktchan_i(" + comIf->getName() + ", " +
                                 comIf->getPropertyValues().value("remote_endpoint_name") + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");

                writer.writeLine("mcapi_open_pktchan_send_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
            else
            {
                writer.writeLine("mcapi_open_pktchan_recv_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                // Sender is responsible of calling the connect function.
                writer.writeLine("mcapi_connect_sclchan_i(" + comIf->getName() + ", " +
                    comIf->getPropertyValues().value("remote_endpoint_name") + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");

                writer.writeLine("mcapi_open_sclchan_send_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
            else
            {
                writer.writeLine("mcapi_open_sclchan_recv_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeEmptyLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateCloseConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: closeConnections()");
    writer.writeLine("int closeConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeEmptyLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_pktchan_send_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeEmptyLine();
            }
            else
            {
                writer.writeLine("mcapi_pktchan_recv_close_i(" + comIf->getPropertyValues().value("handle_name") +
                    ", &request, &status);");
                writer.writeEmptyLine();
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_sclchan_send_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeEmptyLine();
            }
            else
            {
                writer.writeLine("mcapi_sclchan_recv_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeEmptyLine();
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateInitializeMCAPIFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateInitializeMCAPIFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the initialization function.
    writer.writeHeaderComment("Function: initializeMCAPI()");
    writer.writeLine("int initializeMCAPI()");

    writer.beginBlock();
    writer.writeLine("mcapi_version_t version;");
    writer.writeEmptyLine();
    writer.writeLine("// Initialize MCAPI implementation.");
    writer.writeLine("mcapi_initialize(LOCAL_NODE_ID, &version, &status);");
    writer.writeEmptyLine();

    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeEmptyLine();

    // Write function calls to create local endpoints.
    writer.writeLine("// Create local endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine(comIf->getName() + " = mcapi_create_endpoint(" +
                         comIf->getName().toUpper() + "_PORT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }

    // Write function calls to retrieve remote endpoints.
    writer.writeLine("// Retrieve remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine(comIf->getPropertyValues().value("remote_endpoint_name") +
                         " = mcapi_get_endpoint(" +
                         comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_NODE" +
                         ", " +
                         comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_PORT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeWaitCall()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeWaitCall(CSourceWriter& writer, QString const& requestName,
                                       QString const& sizeName)
{
    writer.writeLine("if (mcapi_wait(&" + requestName + ", &" + sizeName + ", &status, MCAPI_INFINITE) == MCAPI_FALSE)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeStatusCheck()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeStatusCheck(CSourceWriter &writer)
{
    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MCAPICodeGenerator::getIcon() const
{
    return QIcon(":icons/mcapi-generator.png");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MCAPICodeGenerator::getProgramRequirements() {
	return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::canGenerateMCAPIComponent()
//-----------------------------------------------------------------------------
bool MCAPICodeGenerator::canGenerateMCAPIComponent(QSharedPointer<Component> component)
{
	// Will not generate a null component.
	if ( component == 0 )
	{
		return false;
	}

	// Errors found in interfaces.
	QStringList errorList;
	// Must have at least one MCAPI interface to generate.
	bool hasMcapi = false;

	// Go over each com interface and check for that each required property is set.
	foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
	{
		// Check API support for generated interface
		if (comIf->getComType().getName().toLower() == "mcapi" )
		{
			hasMcapi = true;
		}

		QSharedPointer<LibraryComponent> libCom = utility_->getLibraryInterface()->getModel(comIf->getComType());
		QSharedPointer<ComDefinition> comDef = libCom.dynamicCast<ComDefinition>();

		checkRequiredPropertiesSet(comDef, comIf, errorList);
	}

	// If errors exist, print about it and return false.
	if ( hasMcapi && !errorList.isEmpty())
	{
		foreach (QString const& msg, errorList)
		{
			utility_->printError(msg);
		}

		QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
			tr("The component contained %1 error(s). MCAPI code was not generated.").arg(errorList.size()),
			QMessageBox::Ok, utility_->getParentWidget());
		msgBox.setDetailedText(tr("The following error(s) were found: \n* ") + errorList.join("\n* "));

		msgBox.exec();
	}

	// If no errors, we can generate the component.
	return errorList.isEmpty() && hasMcapi;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMCAPIForComponent()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMCAPIForComponent(QString dir, QSharedPointer<Component> component)
{
	// Create the template only if it does not exist.
	if (!QFileInfo(dir + "/ktsmcapicode.h").exists())
	{
		generateMainTemplate(dir + "/main.c", component);
	}

	// Update the ktsmcapicode module.
	generateHeader(dir + "/ktsmcapicode.h", component);
	generateSource(dir + "/ktsmcapicode.c", component);
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::addGeneratedMCAPIToFileset()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::addGeneratedMCAPIToFileset(QSharedPointer<Component> component)
{
	// Add the files to the component metadata.
	QSharedPointer<FileSet> fileSet = component->getFileSet("cSources");

	QSettings settings;

	if (fileSet == 0)
	{
		fileSet = QSharedPointer<FileSet>(new FileSet("cSources", "sourceFiles"));
		component->addFileSet(fileSet);
	}

	if (!fileSet->contains("ktsmcapicode.h"))
	{
		fileSet->addFile("ktsmcapicode.h", settings);
	}

	if (!fileSet->contains("ktsmcapicode.c"))
	{
		fileSet->addFile("ktsmcapicode.c", settings);
	}

	if (!fileSet->contains("main.c"))
	{
		fileSet->addFile("main.c", settings);
	}
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkRequiredPropertiesSet()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::checkRequiredPropertiesSet(QSharedPointer<ComDefinition> comDef,
	QSharedPointer<ComInterface> comIf, QStringList &errorList)
{
	foreach ( QSharedPointer<ComProperty> property, comDef->getProperties() )
	{
		if ( property->isRequired() && comIf->getPropertyValues().value(property->getName()).isEmpty() )
		{
			errorList.append(tr("Property %1 of COM interface '%2' is not set").arg(property->getName(),
				comIf->getName()));
		}
	}
}
