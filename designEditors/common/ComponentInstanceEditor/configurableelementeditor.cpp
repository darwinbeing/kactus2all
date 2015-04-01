/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.cpp
 *		Project: Kactus 2
 */

#include "configurableelementeditor.h"
#include "configurableelementdelegate.h"
#include "ConfigurableElementsColumns.h"
#include "ConfigurableElementsFilter.h"

#include <IPXACTmodels/component.h>

#include <designEditors/HWDesign/HWComponentItem.h>

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSharedPointer>
#include <QAbstractItemDelegate>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(QSharedPointer<ListParameterFinder> listFinder,
                                                  QSharedPointer<ParameterFinder> parameterFinder, 
                                                  QSharedPointer<ExpressionFormatter> configurableElementFormatter,
                                                  QSharedPointer<ExpressionFormatter> componentInstanceFormatter,
                                                  QSharedPointer<ExpressionParser> expressionParser,
                                                  QSharedPointer<ExpressionParser> instanceParser,
                                                  QAbstractItemModel* completionModel,
                                                  QWidget *parent):
QGroupBox(tr("Configurable element values"), parent),
view_(this),
model_(parameterFinder, listFinder, configurableElementFormatter, componentInstanceFormatter, expressionParser,
       instanceParser, this),
delegate_()
{
    ConfigurableElementsFilter* filter (new ConfigurableElementsFilter(this));
	filter->setSourceModel(&model_);
    filter->setSortCaseSensitivity(Qt::CaseInsensitive);
	view_.setModel(filter);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(completionModel);

	// set options for the view
	view_.setSortingEnabled(true);
    view_.setSelectionBehavior(QAbstractItemView::SelectItems);
    view_.setSelectionMode(QAbstractItemView::SingleSelection);

    delegate_ = QSharedPointer<ConfigurableElementDelegate> (new ConfigurableElementDelegate(parameterCompleter,
        parameterFinder, configurableElementFormatter, this));

    view_.setItemDelegate(delegate_.data());

    view_.setAlternatingRowColors(false);
    view_.setColumnHidden(ConfigurableElementsColumns::CHOICE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::ARRAY_SIZE, true);
    view_.setColumnHidden(ConfigurableElementsColumns::TYPE, true);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
        filter, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
    
    connect(filter, SIGNAL(removeItem(const QModelIndex&)),
        &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::~ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::~ConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setComponent(ComponentItem* component) 
{
	model_.setComponent(component);
    delegate_->setChoices(component->componentModel()->getChoices());

    view_.expandAll();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::clear() 
{
	model_.clear();
}

//-----------------------------------------------------------------------------
// Function: configurableelementeditor::setDesignConfigurationToModel()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setDesignConfigurationToModel(
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    model_.setDesignConfigurationModel(designConfiguration);
}
