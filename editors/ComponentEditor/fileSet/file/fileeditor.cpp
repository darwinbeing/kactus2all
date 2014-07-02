/* 
 *
 *  Created on: 5.2.2011
 *      Author: Antti Kamppi
 * 		filename: fileeditor.cpp
 */

#include "fileeditor.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

FileEditor::FileEditor( LibraryInterface* handler,
					   QSharedPointer<Component> component, 
					   QSharedPointer<File> file, 
					   QWidget *parent ):
ItemEditor(component, handler, parent),
tabs_(this), 
generalTab_(handler, component, file, &tabs_),
extraTab_(handler, component, file, &tabs_),
file_(file) {

	Q_ASSERT(handler);
	Q_ASSERT(component);
	Q_ASSERT(file);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&tabs_);
	layout->setContentsMargins(0, 0, 0, 0);

	tabs_.addTab(&generalTab_, tr("General settings"));
	tabs_.addTab(&extraTab_, tr("External dependencies and defines"));

	// connect the signals informing that widgets have changed their status
	connect(&generalTab_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalTab_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
    connect(&generalTab_, SIGNAL(editFile()), this, SIGNAL(editFile()), Qt::UniqueConnection);
    connect(&generalTab_, SIGNAL(runFile()), this, SIGNAL(runFile()), Qt::UniqueConnection);

	connect(&extraTab_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&extraTab_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
}

FileEditor::~FileEditor() {
}

bool FileEditor::isValid() const {

	// general tab contains the only mandatory fields.
	return generalTab_.isValid();
}

void FileEditor::refresh() {
	// fetch the data from the model
	generalTab_.refresh();
	extraTab_.refresh();
}
