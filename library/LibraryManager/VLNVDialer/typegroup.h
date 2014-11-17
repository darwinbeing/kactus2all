/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: typegroup.h
 *		Project: Kactus 2
 */

#ifndef TYPEGROUP_H
#define TYPEGROUP_H

#include <common/utils.h>

#include <QGroupBox>
#include <QCheckBox>

/*! \brief Contains the check boxes to set the type options for VLNVDialer.
 *
 */
class TypeGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	TypeGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~TypeGroup();

    /*!
     *  Sets new type options.
     *
     *      @param [in] options   The options to set.
     */
    void setTypes(Utils::TypeOptions options);

    /*!
     *  Gets the selected type options.
     *
     *      @return Selected type options.
     */
    Utils::TypeOptions getTypes() const;

signals:

	//! \brief Emitted when type settings change
	void optionsChanged(const Utils::TypeOptions& options);

private slots:

	//! \brief Handler for componentBox state changes.
    void onComponentChange(bool checked);

	//! \brief Handler for busApiBox state changes.
    void onBusChange(bool checked);

	//! \brief Handler for advancedBox state changes.
    void onAdvancedChange(bool checked);

private:
	//! \brief No copying
	TypeGroup(const TypeGroup& other);

	//! \brief No assignment
	TypeGroup& operator=(const TypeGroup& other);

	//! \brief Check box to select components in/out of search results.
	QCheckBox componentBox_;

	//! \brief Check box to select buses/APIs in/out of search results.
	QCheckBox busApiComBox_;

	//! \brief Check box to select other IP-Xact types in/out of search results.
	QCheckBox advancedBox_;

	//! \brief Contains the current search settings for type.
	Utils::TypeOptions options_;
};

#endif // TYPEGROUP_H
