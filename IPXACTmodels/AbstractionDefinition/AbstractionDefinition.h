//-----------------------------------------------------------------------------
// File: AbstractionDefinition.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Implementation for ipxact:abstractionDefinition element.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITION_H
#define ABSTRACTIONDEFINITION_H

#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/common/Parameter.h>


#include <IPXACTmodels/vlnv.h>

#include <IPXACTmodels/ipxactmodels_global.h>
 
#include <QSharedPointer>

class PortAbstraction;

//-----------------------------------------------------------------------------
//! Implementation for ipxact:abstractionDefinition element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionDefinition: public Document
{
public:

	//! The default constructor.
	AbstractionDefinition();

	//! Copy constructor.
	AbstractionDefinition(AbstractionDefinition const& other);

	//! Assignment operator.
	AbstractionDefinition &operator=(AbstractionDefinition const& other);

	//! The destructor.
	virtual ~AbstractionDefinition();

	/*!
	 *  Creates a perfect copy of the Bus Definition.
	 *
	 *      @return Copy of the bus definition.
	 */
	virtual QSharedPointer<Document> clone() const;

	/*! Set the VLNV for the bus definition.
	*
	*      @param [in] vlnv     The vlnv to set
	*/
	virtual void setVlnv(const VLNV& vlnv);

	/*! Get the vlnv tag of the bus definition that this abstraction definition details.
	 * 
	 *      @return The VLNV this abstraction definition details.
	 */
	VLNV getBusType() const;

	/*! Set the bus definition detailed in this abstraction definition.
	 *
	 *      @param [in] targetBus   The vlnv of the target bus definition.
	 */
	void setBusType(VLNV const& targetBus);

	/*! Get the vlnv tag of the bus definition that this definition extends.
	 * 
	 *      @return The VLNV this bus extends or empty VLNV if this bus does not extend another.
	 */
	VLNV getExtends() const;

	/*! Set the bus to extend another.
	 *
	 *      @param [in] extends  The vlnv of the extended bus definition.
	 */
	void setExtends(VLNV const& extends);
    
	/*! Get the VLNVs that this busDefinition depends on.
	 *
	 *      @return The dependency VLNVs.
	 */
	virtual QList<VLNV> getDependentVLNVs() const;

	/*! Get the file dependencies of this busDefinition.
     *
	 *      @return The dependency files.
     *
     *      @remark This function never returns anything because busDefinition only has VLNV dependencies.
	 */
	virtual QStringList getDependentFiles() const;
    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > getLogicalPorts() const;
private:

    //! Specifies the bus definition this abstraction definition details.
    VLNV busType_;

	//! Specifies if this definition is an extension from another bus.
	VLNV extends_;

    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > logicalPorts_;
};

#endif /* ABSTRACTIONDEFINITION_H */
