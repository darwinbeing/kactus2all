/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.h
 * 	Project:	Kactus 2
*/

#ifndef MEMORYMAPHEADERGENERATOR_H
#define MEMORYMAPHEADERGENERATOR_H


#include "memorymapheadergenerator_global.h"

#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include "globalheadersavemodel.h"
#include "systemheadersavemodel.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>

class IPluginUtility;

/*! \brief The plugin to generate C-headers of component memory maps.
 *
 */
class MEMORYMAPHEADERGENERATOR_EXPORT MemoryMapHeaderGenerator : public QObject, public IGeneratorPlugin {

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "kactus2.plugins.MemoryMapHeaderGenerator" FILE "headerGenerator.json")

	Q_INTERFACES(IPlugin)
	Q_INTERFACES(IGeneratorPlugin)

public:

	/*! \brief The constructor
	 *
	 * Method: 		MemoryMapHeaderGenerator
	 * Full name:	MemoryMapHeaderGenerator::MemoryMapHeaderGenerator
	 * Access:		public 
	 *
	*/
	MemoryMapHeaderGenerator();
	
	//! \brief The destructor
	virtual ~MemoryMapHeaderGenerator();

	//! \brief Returns the name of the plugin.
    virtual QString const& getName() const;

	//! \brief Returns the version of the plugin.
    virtual QString const& getVersion() const;

	//! \brief Returns the description of the plugin.
    virtual QString const& getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString const& getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString const& getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString const& getLicenceHolder() const;

	//! \brief Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*! \brief Checks whether the generator supports generation for the given library component.
     *
     * Method: 		checkGeneratorSupport
     * Full name:	MemoryMapHeaderGenerator::checkGeneratorSupport
     * Access:		virtual public 
     *
     * \param libComp The library component for which to check support.
     * \param libDesConf The design configuration object if the generator is run on a hierarchical component.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
     * \return True, if the generator supports the given component. Otherwise false.
    */
	 virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
		 QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(),
		 QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*! \brief Runs the generator.
     *
     * Method: 		runGenerator
     * Full name:	MemoryMapHeaderGenerator::runGenerator
     * Access:		virtual public 
     *
     * \param utility The plugin utility interface.
     * \param libComp The component for which the generator is run.
     * \param libDesConf The design configuration object if the generator is run on a hierarchical component.
     * \param libDes The design object if the generator is run on a hierarchical component.
     *
    */
	 virtual void runGenerator(IPluginUtility* utility, 
		 QSharedPointer<LibraryComponent> libComp, 
		 QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent>(), 
		 QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

	 //! \brief Returns the external program requirements of the plugin.
	 virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

private:

	//! \brief No copying
	MemoryMapHeaderGenerator(const MemoryMapHeaderGenerator& other);

	//! \brief No assignment
	MemoryMapHeaderGenerator& operator=(const MemoryMapHeaderGenerator& other);

	/*! \brief Generate headers for local memory maps of a component.
	 *
	 * Method: 		generateLocalMemMapHeaders
	 * Full name:	MemoryMapHeaderGenerator::generateLocalMemMapHeaders
	 * Access:		private 
	 *
	 * \param comp Pointer to the component which contains the local memory maps.
	 *
	*/
	void generateLocalMemMapHeaders( QSharedPointer<Component> comp );

	/*! \brief Generate global headers for the CPU instances of the design.
	 *
	 * Method: 		generateGlobalHeaders
	 * Full name:	MemoryMapHeaderGenerator::generateGlobalHeaders
	 * Access:		private 
	 *
	 * \param comp Pointer to the top hierarchical component to which the headers are added to.
	 * \param design Pointer to the design which instantiates the CPU instances.
	 *
	*/
	void generateGlobalHeaders(QSharedPointer<Component> comp, QSharedPointer<Design> design);

	/*! \brief Generate system headers which include the other generated codes for the HW platform.
	 *
	 * Method: 		generateSystemHeaders
	 * Full name:	MemoryMapHeaderGenerator::generateSystemHeaders
	 * Access:		private 
	 *
	 * \param comp Pointer to the component containing the system design.
	 * \param desConf Pointer to the configuration of the system design.
	 * \param design Pointer to the design which instantiates the components.
	 *
	*/
	void generateSystemHeaders(QSharedPointer<Component> comp, 
		QSharedPointer<DesignConfiguration> desConf,
		QSharedPointer<Design> design);

	/*! \brief Add a generated file to the file sets of a component.
	 *
	 * Method: 		addHeaderFile
	 * Full name:	MemoryMapHeaderGenerator::addHeaderFile
	 * Access:		private 
	 *
	 * \param component Pointer to the component where the file is added to.
	 * \param fileInfo Contains the info on the generated file.
	 * \param filesetName The name of the file set to add the header file to.
	 * \param swViewNames Contains the names of the sw views to add the file set reference to.
	 * \param instanceId The id of the instance which is used when generating global headers.
	 *
	*/
	void addHeaderFile(QSharedPointer<Component> component,
		const QFileInfo& fileInfo, 
		const QString& filesetName,
		const QStringList& swViewNames = QStringList(),
		const QString& instanceId = QString()) const;

	/*! \brief Parse the interface and depending on the type move forward in the connections.
	 *
	 * Method: 		parseInterface
	 * Full name:	MemoryMapHeaderGenerator::parseInterface
	 * Access:		private 
	 *
	 * \param offset The offset used as base for the addresses.
	 * \param stream The text stream where the address defines are written into. 
	 * \param interface Identifies the current interface to parse.
	 *
	*/
	void parseInterface(qint64 offset,
		QTextStream& stream,
		const Design::Interface& interface);

	/*! \brief Search the CPU instances and their containing components to find the generated global headers.
	 *
	 * Method: 		searchInstanceFiles
	 * Full name:	MemoryMapHeaderGenerator::searchInstanceFiles
	 * Access:		private 
	 *
	 * \param component Pointer to the hierarchical component to search.
	 * \param hwViewName The name of the hierarchical view on the component.
	 *
	*/
	void searchInstanceFiles(QSharedPointer<const Component> component, const QString& hwViewName);

	//! The plugin utility.
	IPluginUtility* utility_;

	//! \brief Pointer to the design being operated when creating global headers.
	QSharedPointer<Design> design_;

	//! \brief The list where all operated interfaces are added to when generating global headers.
	QList<Design::Interface> operatedInterfaces_;

	//! \brief Contains the settings for the system header generation.
	QList<SystemHeaderSaveModel::SysHeaderOptions> sysGenSettings_;

	//! \brief Used to save info on instances and their active views to be parsed.
	struct HierParsingInfo {
		
		//! \brief Identifies the component.
		QSharedPointer<const Component> comp_;

		//! \brief The view which is set for the component.
		QString activeView_;

		//! \brief The constructor.
		HierParsingInfo(QSharedPointer<const Component> comp, const QString& viewName);

		//! \brief Copy constructor. No deep copy needed for the pointer.
		HierParsingInfo(const HierParsingInfo& other);

		//! \brief Assignment operator. No deep copy needed for the pointer.
		HierParsingInfo& operator=(const HierParsingInfo& other);
	};
};

#endif // MEMORYMAPHEADERGENERATOR_H
