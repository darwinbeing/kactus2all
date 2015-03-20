//-----------------------------------------------------------------------------
// File: parametergroupbox.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 23.2.2012
//
// Description:
// Group box that can be used to edit list of parameters.
//-----------------------------------------------------------------------------

#include "parametergroupbox.h"

#include <common/views/EditableTableView/ColumnFreezableTable.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/parameters/ParameterColumns.h>
#include <editors/ComponentEditor/parameters/ParameterDelegate.h>
#include <editors/ComponentEditor/parameters/ParameterEditorHeaderView.h>
#include <editors/ComponentEditor/parameters/parametersmodel.h>

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>

#include <IPXACTmodels/component.h>

#include <QVBoxLayout>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::ParameterGroupBox()
//-----------------------------------------------------------------------------
ParameterGroupBox::ParameterGroupBox(QSharedPointer<QList<QSharedPointer<Parameter> > > parameters,
                                     QSharedPointer<Component> component,
                                     QSharedPointer<ParameterFinder> parameterFinder,
                                     QSharedPointer<ExpressionFormatter> expressionFormatter,
									 QWidget *parent):
QGroupBox(tr("Parameters"), parent),
view_(0), 
model_(0)
{
    QSharedPointer<EditableTableView> parametersView(new EditableTableView(this));
    parametersView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    parametersView->verticalHeader()->show();

    view_ = new ColumnFreezableTable(1, parametersView, this);

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));
    model_ = new ParametersModel(parameters, component->getChoices(), expressionParser, parameterFinder, 
        expressionFormatter, this);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(errorMessage(const QString&)),
        this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(model_, SIGNAL(noticeMessage(const QString&)),
        this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)), 
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    ParameterEditorHeaderView* parameterHorizontalHeader = new ParameterEditorHeaderView(Qt::Horizontal, this);
    view_->setHorizontalHeader(parameterHorizontalHeader);
    view_->horizontalHeader()->setSectionsClickable(true);
    view_->horizontalHeader()->setStretchLastSection(true);

	// set view to be sortable
	view_->setSortingEnabled(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(parameterFinder, this);
    parameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(parameterModel);

    view_->setDelegate(new ParameterDelegate(component->getChoices(), parameterCompleter, parameterFinder,
        expressionFormatter, this));

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    //! The proxy that is used to do the sorting of parameters.
    QSortFilterProxyModel* sortingProxy = new QSortFilterProxyModel(this);

	// set source model for proxy
	sortingProxy->setSourceModel(model_);
	// set proxy to be the source for the view
	view_->setModel(sortingProxy);

	// sort the view
	view_->sortByColumn(0, Qt::AscendingOrder);

    view_->setColumnHidden(ParameterColumns::SOURCEIDS, true);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::~ParameterGroupBox()
//-----------------------------------------------------------------------------
ParameterGroupBox::~ParameterGroupBox()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::isValid()
//-----------------------------------------------------------------------------
bool ParameterGroupBox::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::isValid()
//-----------------------------------------------------------------------------
bool ParameterGroupBox::isValid(QStringList& errorList, const QString& parentIdentifier) const
{
    return model_->isValid(errorList, parentIdentifier);
}

//-----------------------------------------------------------------------------
// Function: ParameterGroupBox::refresh()
//-----------------------------------------------------------------------------
void ParameterGroupBox::refresh() 
{
	view_->update();
}
