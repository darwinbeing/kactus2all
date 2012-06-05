/* 
 *
 *  Created on: 7.2.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildcommand.h
 */

#ifndef FILEBUILDCOMMAND_H
#define FILEBUILDCOMMAND_H

#include "targetnameedit.h"

#include <common/widgets/attributeBox/attributebox.h>
#include <models/buildcommand.h>
#include <models/file.h>

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QFileInfo>

/*! \brief FileBuildCommand is a widget to edit File's buildCommand
 * 
 */
class FileBuildCommand : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param file Pointer to the file that is being edited.
	 * \param baseLocation Location in the file system that is used as base for
	 * relative file paths.
	 *
	*/
	FileBuildCommand(QWidget *parent, 
		File* file,
		const QFileInfo& baseLocation);

	//! \brief The destructor
	virtual ~FileBuildCommand();

	/*! \brief Apply the changes from editor to model.
	 *
	*/
	void apply();

	/*! \brief Restore the changes from the model to the editor.
	 *
	*/
	void restore();

signals:

	//! \brief Emitted when contents of the widget change
	void contentChanged();

private:

	//! \brief No copying
	FileBuildCommand(const FileBuildCommand& other);

	//! No assignment
	FileBuildCommand& operator=(const FileBuildCommand& other);

	//! \brief set up the command editor
	void setupCommand();

	//! \brief Set up the flags editor
	void setupFlags();

	//! \brief Set up the target editor
	void setupTarget();

	//! \brief Pointer to the file's buildCommand
	BuildCommand* buildCommand_;

	//! \brief Editor to set file's build command
	QLineEdit command_;

	//! \brief Editor to set build command's flags
	QLineEdit flags_;

	//! \brief Editor to set build command's replaceDefaultFlags setting
	QCheckBox replaceDefault_;

	//! \brief Editor to set build command's target file.
	TargetNameEdit target_;

	//! \brief the layout for the widget
	QGridLayout layout_;
};

#endif // FILEBUILDCOMMAND_H
