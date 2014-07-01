/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORFILEITEM_H
#define COMPONENTEDITORFILEITEM_H

#include "componenteditoritem.h"
#include <IPXACTmodels/file.h>

#include <QRegExpValidator>

/*! \brief The item for a single file in component editor's navigation tree.
 *
 */
class ComponentEditorFileItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param file Pointer to the file being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileItem(QSharedPointer<File> file,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileItem();

	/*! \brief Get the font to be used for text of this item.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * Files can always be opened and this function returns true.
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

signals:

    //! Emitted when a file should be opened with a selected application.
    void openFile(QString const& fileAbsolutePath, QString const& applicationAbsolutePath);

public slots:
	
	//! Called to open the item in a default editor.
	virtual void openItem();

    //! Called to let the user choose an editor to open the item.
    virtual void openWith();

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	//! \brief No copying
	ComponentEditorFileItem(const ComponentEditorFileItem& other);

	//! \brief No assignment
	ComponentEditorFileItem& operator=(const ComponentEditorFileItem& other);
   
    //! Finds the absolute path of the file represented by the item.
    QString fileAbsolutePath() const;

    /*!
     *  Returns true if the item has a built-in editor available.
     */
    virtual bool useKactusCSourceEditor() const;

	//! \brief Pointer to the file being edited.
	QSharedPointer<File> file_;

	//! \brief Checks if the given string is a URL (invalids are allowed) or not.
	QRegExpValidator* urlTester_;

	//! \brief Validates the file path to check if they are valid URLs.
	QRegExpValidator* urlValidator_;

    //! Action to open the file for editing with default editor.
    QAction* editAction_;

    //! Action to open the file for editing with a chosen editor.
    QAction* editWithAction_;

};

#endif // COMPONENTEDITORFILEITEM_H
