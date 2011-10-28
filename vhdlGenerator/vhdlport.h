/* 
 *  	Created on: 28.10.2011
 *      Author: Antti Kamppi
 * 		filename: vhdlport.h
 *		Project: Kactus 2
 */

#ifndef VHDLPORT_H
#define VHDLPORT_H

#include <models/generaldeclarations.h>

#include "vhdlobject.h"

class Port;
class VhdlGenerator2;
class VhdlComponentDeclaration;

/*! \brief VhdlPort represents one port declaration in vhdl.
 * 
 * VhdlPort is used within top component entity and component declarations
 * to print the declaration for one port.
 */
class VhdlPort : public VhdlObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this port.
	 * \param port Pointer to the port that contains the details for this port.
	 *
	*/
	VhdlPort(VhdlGenerator2* parent, Port* port);

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this port.
	 * \param port Pointer to the port that contains the details for this port.
	 *
	*/
	VhdlPort(VhdlComponentDeclaration* parent, Port* port);
	
	//! \brief The destructor
	virtual ~VhdlPort();

	/*! \brief Write the contents of the port to the text stream.
	 *
	 * \param stream The text stream to write the port into.
	 *
	*/
	virtual void write(QTextStream& stream);

	/*! \brief Get the size of the port.
	 * 
	 * Size is calculated: left - right + 1
	 * 
	 * \return int The size of the port.
	*/
	virtual int size() const;

	/*! \brief Get the left bound of the port.
	 *
	 * \return int The left bound.
	*/
	virtual int left() const;

	/*! \brief Get the right bound of the port.
	 *
	 * \return int The right bound.
	*/
	virtual int right() const;

	/*! \brief Set the port to be commented out or not.
	 *
	 * \param commentOut If true then the port is commented out when written.
	 *
	*/
	virtual void setCommented(bool commentOut);

private:
	//! \brief No copying
	VhdlPort(const VhdlPort& other);

	//! \brief No assignment
	VhdlPort& operator=(const VhdlPort& other);

	//! \brief The direction of the port.
	General::Direction direction_;

	//! \brief The left bound of the port.
	int left_;

	//! \brief The right bound of the port.
	int right_;

	//! \brief If true then the port is commented out when printed to text stream
	bool commentOut_;
};

#endif // VHDLPORT_H
