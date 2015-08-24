//-----------------------------------------------------------------------------
// File: Protocol.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// Implementation of ipxact:protocol.
//-----------------------------------------------------------------------------

#include "Protocol.h"

//-----------------------------------------------------------------------------
// Function: Protocol::Protocol()
//-----------------------------------------------------------------------------
Protocol::Protocol() : Extendable(),
protocolType_(),
    payloadName_(),
    payloadType_(),
    payloadExtension_(),
    mandatoryPayloadExtension_(false)
{

}

//-----------------------------------------------------------------------------
// Function: Protocol::Protocol()
//-----------------------------------------------------------------------------
Protocol::Protocol(Protocol const& other) : 
protocolType_(other.protocolType_),
    payloadName_(other.payloadName_),
    payloadType_(other.payloadType_),
    payloadExtension_(other.payloadExtension_),
    mandatoryPayloadExtension_(other.mandatoryPayloadExtension_)
{

}

//-----------------------------------------------------------------------------
// Function: Protocol::~Protocol()
//-----------------------------------------------------------------------------
Protocol::~Protocol()
{

}

//-----------------------------------------------------------------------------
// Function: Protocol::setType()
//-----------------------------------------------------------------------------
void Protocol::setProtocolType(QString const& type)
{
    protocolType_ = type;
}

//-----------------------------------------------------------------------------
// Function: Protocol::getType()
//-----------------------------------------------------------------------------
QString Protocol::getProtocolType() const
{
    return protocolType_;
}

//-----------------------------------------------------------------------------
// Function: Protocol::setPayloadName()
//-----------------------------------------------------------------------------
void Protocol::setPayloadName(QString const& payloadName)
{
    payloadName_ = payloadName;
}

//-----------------------------------------------------------------------------
// Function: Protocol::getPayloadName()
//-----------------------------------------------------------------------------
QString Protocol::getPayloadName() const
{
    return payloadName_;
}

//-----------------------------------------------------------------------------
// Function: Protocol::setPayloadType()
//-----------------------------------------------------------------------------
void Protocol::setPayloadType(QString const& payloadType)
{
    payloadType_ = payloadType;
}

//-----------------------------------------------------------------------------
// Function: Protocol::getPayloadType()
//-----------------------------------------------------------------------------
QString Protocol::getPayloadType() const
{
    return payloadType_;
}

//-----------------------------------------------------------------------------
// Function: Protocol::setPayloadExtension()
//-----------------------------------------------------------------------------
void Protocol::setPayloadExtension(QString const& extension, bool mandatory)
{
    payloadExtension_ = extension;
    mandatoryPayloadExtension_ = mandatory;
}

//-----------------------------------------------------------------------------
// Function: Protocol::getPayloadExtension()
//-----------------------------------------------------------------------------
QString Protocol::getPayloadExtension() const
{
    return payloadExtension_;
}

//-----------------------------------------------------------------------------
// Function: Protocol::hasMandatoryPayloadExtension()
//-----------------------------------------------------------------------------
bool Protocol::hasMandatoryPayloadExtension() const
{
    return mandatoryPayloadExtension_;
}

