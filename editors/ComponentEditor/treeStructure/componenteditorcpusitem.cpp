//-----------------------------------------------------------------------------
// File: componenteditorcpusitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author:Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The Cpus-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorcpusitem.h"

#include <editors/ComponentEditor/cpus/cpuseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/cpu.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::ComponentEditorCpusItem()
//-----------------------------------------------------------------------------
ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
    LibraryInterface* libHandler,
    QSharedPointer<Component> component,
    ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
    cpus_(component->getCpus())
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::~ComponentEditorCpusItem()
//-----------------------------------------------------------------------------
ComponentEditorCpusItem::~ComponentEditorCpusItem()
{
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorCpusItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!cpus_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorCpusItem::text() const
{
	return tr("Cpus");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorCpusItem::editor()
{
	if (!editor_)
    {
		editor_ = new CpusEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorCpusItem::getTooltip() const
{
	return tr("Contains the programmable cores of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorCpusItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorCpusItem::isValid() const
{
	/*QStringList addrSpaceNames = component_->getAddressSpaceNames();

	foreach (QSharedPointer<Cpu> cpu, *cpus_) 
    {
		if (!cpu->isValid(addrSpaceNames, component_->getChoices()))
        {
			return false;
		}
	}*/

	// all cpus were valid
	return true;
}
