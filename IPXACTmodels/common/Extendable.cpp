//-----------------------------------------------------------------------------
// File: Extendable.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.08.2015
//
// Description:
// Base class for all IP-XACT implementations with vendor extensions.
//-----------------------------------------------------------------------------

#include "Extendable.h"

#include <IPXACTmodels/VendorExtension.h>


//-----------------------------------------------------------------------------
// Function: Extendable::~Extendable()
//-----------------------------------------------------------------------------
Extendable::~Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: Extendable::getVendorExtensions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<VendorExtension> > > Extendable::getVendorExtensions() const
{
    return vendorExtensions_;
}

//-----------------------------------------------------------------------------
// Function: Extendable::Extendable()
//-----------------------------------------------------------------------------
Extendable::Extendable() :
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{

}

//-----------------------------------------------------------------------------
// Function: Extendable::Extendable()
//-----------------------------------------------------------------------------
Extendable::Extendable(Extendable const& other) : 
vendorExtensions_(new QList<QSharedPointer<VendorExtension> >())
{
    copyVendorExtensions(other);
}

//-----------------------------------------------------------------------------
// Function: Extendable::operator=()
//-----------------------------------------------------------------------------
Extendable& Extendable::operator=(Extendable const& other)
{
    if (this != &other)
    {
        vendorExtensions_->clear();
        copyVendorExtensions(other);
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::copyVendorExtensions()
//-----------------------------------------------------------------------------
void Extendable::copyVendorExtensions(Extendable const& other)
{
    foreach (QSharedPointer<VendorExtension> extension, *other.vendorExtensions_)
    {
        vendorExtensions_->append(QSharedPointer<VendorExtension>(extension->clone()));
    }
}