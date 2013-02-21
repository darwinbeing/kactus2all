/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.h
 * 	Project:	Kactus 2
*/

#ifndef MEMORYMAPHEADERGENERATOR_H
#define MEMORYMAPHEADERGENERATOR_H


#include "memorymapheadergenerator_global.h"

#include <PluginSystem/IGeneratorPlugin.h>
#include <models/component.h>
#include <models/librarycomponent.h>

#include <QObject>
#include <QString>
#include <QSharedPointer>

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
     *
     * \return True, if the generator supports the given component. Otherwise false.
    */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp) const;

    /*! \brief Runs the generator.
     *
     * Method: 		runGenerator
     * Full name:	MemoryMapHeaderGenerator::runGenerator
     * Access:		virtual public 
     *
     * \param utility The plugin utility interface.
     * \param libComp The component for which the generator is run.
     *
    */
    virtual void runGenerator(IPluginUtility* utility,
                              QSharedPointer<LibraryComponent> libComp);

private:

	//! \brief No copying
	MemoryMapHeaderGenerator(const MemoryMapHeaderGenerator& other);

	//! \brief No assignment
	MemoryMapHeaderGenerator& operator=(const MemoryMapHeaderGenerator& other);

	//! The plugin utility.
	IPluginUtility* utility_;
};

#endif // MEMORYMAPHEADERGENERATOR_H
