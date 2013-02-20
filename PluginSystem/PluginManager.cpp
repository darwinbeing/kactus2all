//-----------------------------------------------------------------------------
// File: PluginManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 27.6.2012
//
// Description:
// Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------

#include "PluginManager.h"

#include "IPlugin.h"

#include <QDir>

//-----------------------------------------------------------------------------
// Function: PluginManager::PluginManager()
//-----------------------------------------------------------------------------
PluginManager::PluginManager(QString const& pluginPath)
{
    QDir dir(pluginPath);

    foreach (QString const& filename, dir.entryList(QDir::Files))
    {
        QPluginLoader loader(dir.absoluteFilePath(filename));
        IPlugin* plugin = qobject_cast<IPlugin*>(loader.instance());

        if (plugin != 0)
        {
            plugins_.append(plugin);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PluginManager::~PluginManager()
//-----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
}

//-----------------------------------------------------------------------------
// Function: PluginManager::getPlugins()
//-----------------------------------------------------------------------------
QList<IPlugin*> const& PluginManager::getPlugins() const
{
    return plugins_;
}
