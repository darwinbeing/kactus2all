/* 
 *
 *  Created on: 2.2.2011
 *      Author: Antti Kamppi
 * 		filename: itemeditor.cpp
 */

#include "itemeditor.h"
#include <models/component.h>
#include <LibraryManager/libraryinterface.h>

ItemEditor::ItemEditor( QSharedPointer<Component> component,
	LibraryInterface* handler,
	QWidget *parent): 
QWidget(parent), 
component_(component),
handler_(handler) {
}

ItemEditor::~ItemEditor() {
}

QSharedPointer<Component> ItemEditor::component() const {
	return component_;
}

//-----------------------------------------------------------------------------
// Function: confirmEditorChange()
//-----------------------------------------------------------------------------
bool ItemEditor::confirmEditorChange()
{
    // By default, we always allow the editor to be changed.
    return true;
}

LibraryInterface* ItemEditor::handler() const {
	return handler_;
}
