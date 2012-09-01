//-----------------------------------------------------------------------------
// File: AddressEntry.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 01.09.2012
//
// Description:
// Address entry class.
//-----------------------------------------------------------------------------

#include "AddressEntry.h"

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designwidget/BusPortItem.h>

#include <models/businterface.h>
#include <models/slaveinterface.h>
#include <models/memorymap.h>

//-----------------------------------------------------------------------------
// Function: AddressEntry::AddressEntry()
//-----------------------------------------------------------------------------
AddressEntry::AddressEntry(ComponentItem* component, BusPortItem* port)
    : component_(component),
      port_(port),
      connectedPort_(0),
      range_(0),
      baseEndAddress_(0)
{
    if (port_->isConnected())
    {
        // Determine the connected port.
        GraphicsConnection const* conn = port_->getConnections().first();

        if (conn->endpoint1() != port_)
        {
            connectedPort_ = conn->endpoint1();
        }
        else
        {
            connectedPort_ = conn->endpoint2();
        }

        // Retrieve the size of the memory map.
        ComponentItem* connectedComp = connectedPort_->encompassingComp();

        if (connectedComp != 0)
        {
            QString mapName = connectedPort_->getBusInterface()->getSlave()->getMemoryMapRef();
            MemoryMap const* map = connectedComp->componentModel()->getMemoryMap(mapName);

            baseEndAddress_ = map->getLastAddress();
            range_ = baseEndAddress_ * map->getAddressUnitBits() / 8;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::setStartAddress()
//-----------------------------------------------------------------------------
void AddressEntry::setStartAddress(unsigned int startAddress)
{
    QMap<QString, QString> elements = component_->getConfigurableElements();
    elements.insert(port_->name() + "_start_addr", QString::number(startAddress));
    component_->setConfigurableElements(elements);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::setLocked()
//-----------------------------------------------------------------------------
void AddressEntry::setLocked(bool locked)
{
    QMap<QString, QString> elements = component_->getConfigurableElements();

    if (locked)
    {
        elements.insert(port_->name() + "_addr_locked", "true");
    }
    else
    {
        elements.insert(port_->name() + "_addr_locked", "false");
    }

    component_->setConfigurableElements(elements);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getStartAddress()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getStartAddress() const
{
    QMap<QString, QString> const& elements = component_->getConfigurableElements();
    return elements.value(port_->name() + "_start_addr", "0").toUInt();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getEndAddress()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getEndAddress() const
{
    return getStartAddress() + baseEndAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::isLocked()
//-----------------------------------------------------------------------------
bool AddressEntry::isLocked() const
{
    QMap<QString, QString> const& elements = component_->getConfigurableElements();
    return elements.value(port_->name() + "_addr_locked", "false") == "true";
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getRange()
//-----------------------------------------------------------------------------
unsigned int AddressEntry::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getInterfaceName()
//-----------------------------------------------------------------------------
QString AddressEntry::getInterfaceName() const
{
    return component_->name() + "." + port_->name();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::getMemoryMapName()
//-----------------------------------------------------------------------------
QString AddressEntry::getMemoryMapName() const
{
    if (connectedPort_ == 0)
    {
        return QObject::tr("unspecified");
    }

    return connectedPort_->encompassingComp()->name() + "." +
           connectedPort_->getBusInterface()->getSlave()->getMemoryMapRef();
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::isConnected()
//-----------------------------------------------------------------------------
bool AddressEntry::hasValidConnection() const
{
    return (connectedPort_ != 0);
}

//-----------------------------------------------------------------------------
// Function: AddressEntry::overlaps()
//-----------------------------------------------------------------------------
bool AddressEntry::overlaps(AddressEntry const& other) const
{
    return !(getEndAddress() < other.getStartAddress() || getStartAddress() > other.getEndAddress());
}
