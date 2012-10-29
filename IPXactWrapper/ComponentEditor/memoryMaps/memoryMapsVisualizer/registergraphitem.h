/* 
 *  	Created on: 24.4.2012
 *      Author: Antti Kamppi
 * 		filename: registergraphitem.h
 *		Project: Kactus 2
 */

#ifndef REGISTERGRAPHITEM_H
#define REGISTERGRAPHITEM_H

#include <IPXactWrapper/ComponentEditor/visualization/memoryvisualizationitem.h>
#include <models/register.h>

#include <QSharedPointer>

/*! \brief The graphical item that represents one register.
 *
 */
class RegisterGraphItem : public MemoryVisualizationItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param reg Pointer to the register that this graph item visualizes.
	 * \param parent Pointer to the parent of this graph item.
	 *
	*/
	RegisterGraphItem(QSharedPointer<Register> reg,
		QGraphicsItem* parent);
	
	//! \brief The destructor
	virtual ~RegisterGraphItem();

	//! \brief Refresh the item and sub-items.
	virtual void refresh();

	/*! \brief Get the offset of the item. 
	 *
	 * \return int The offset of the item from the parent item's base address.
	*/
	virtual int getOffset() const;

	/*! \brief Get the bit width of the item.
	 * 
	 * \return The bit width of the item.
	*/
	virtual int getBitWidth() const;

private:
	
	//! \brief No copying
	RegisterGraphItem(const RegisterGraphItem& other);

	//! \brief No assignment
	RegisterGraphItem& operator=(const RegisterGraphItem& other);

	//! \brief Pointer to the register being visualized.
	QSharedPointer<Register> register_;
};

#endif // REGISTERGRAPHITEM_H
