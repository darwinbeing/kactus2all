/* 
 *
 *  Created on: 15.10.2010
 *      Author: Antti Kamppi
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <common/Global.h>

#include <QXmlStreamWriter>
#include <QString>
#include <QDomNode>
#include <QStringList>

/*! \brief Equals the spirit:channel element in IP-Xact specification
 *
 * This struct contains a list of all the mirrored bus interfaces in the
 * containing component that belong to the same channel
 */
class KACTUS2_API Channel {

public:

	/*! \brief The constructor
	 *
	 * \param channelNode A reference to the QDomNode to parse the channel
	 * information from.
	 *
	 * Exception guarantee: basic
	 * \exception Parse_error Occurs when a mandatory element is missing in
	 * this class or one of it's member classes.
	 */
	Channel(QDomNode &channelNode);

	/*! \brief The default constructor
	 *
	 * Constructs an empty channel which is in invalid state.
	*/
	Channel();

	//! Copy constructor
	Channel(const Channel& other);

	//! Assignment operator
	Channel& operator=(const Channel& other);

	//! The destructor
	~Channel();

	/*! \brief Write the contents of the class using the writer.
	*
	* Uses the specified writer to write the class contents into file as valid
	* IP-Xact.
	*
	* \param writer A reference to a QXmlStreamWrite instance that is used to
	* write the document into file.
	*/
	void write(QXmlStreamWriter& writer);

	/*! \brief Check if the channel is in a valid state.
	 *
	 * \param interfaceNames Contains the names of the bus interfaces in the component.
	 * \param errorList The list to add the possible error messages to.
	 * \param parentIdentifier String from parent to help to identify the location of the error.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& interfaceNames,
		QStringList& errorList, 
		const QString& parentIdentifier) const;

	/*! \brief Check if the channel is in a valid state.
	 * 
	 * \param interfaceNames Contains the names of the bus interfaces in the component.
	 * 
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid(const QStringList& interfaceNames) const;

	/*! \brief Get the name of the channel
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Get the displayName of the channel.
	 *
	 * \return QString containing the name.
	 */
	QString getDisplayName() const;

	/*! \brief Get the description of the channel.
	 *
	 * \return QString containing the description.
	 */
	QString getDescription() const;

	/*! \brief Get the references to bus interfaces that belong to this channel.
	 *
	 * \return Reference to QStringList containing the interface references.
	*/
	const QStringList& getInterfaces() const;

	/*! \brief Set the interfaces that belong to this channel.
	 *
	 * \param interfaceNames QStringList containing the references to interfaces.
	 *
	*/
	void setInterfaces(const QStringList& interfaceNames);

	/*! \brief Set the name of the channel
	 *
	 * \param name The name to be set.
	 *
	*/
	void setName(const QString& name);

	/*! \brief Set the display name of the channel.
	 *
	 * \param name The display name to set.
	 *
	*/
	void setDisplayName(const QString& name);

	/*! \brief  Set the desciption of the channel.
	 *
	 * \param description The description to set.
	 *
	*/
	void setDescription(const QString& description);

private:

	//! \brief Identifies the containing element
	QString name_;

	//! \brief The displayName that can have a short descriptive name
	QString displayName_;

	//! \brief The textual description of the containing element.
	QString description_;

	//! \brief List of references to mirrored bus interfaces.
	QStringList busInterfaces_;

};

#endif /* CHANNEL_H_ */
