
#include "ChoicesEditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/choices/ChoicesModel.h>
#include <editors/ComponentEditor/choices/ChoicesDelegate.h>


//-----------------------------------------------------------------------------
// Function: ChoicesEditor::ChoicesEditor()
//-----------------------------------------------------------------------------
ChoicesEditor::ChoicesEditor( QSharedPointer<Component> component, 
					   QWidget* parent /*= 0*/ ):
ItemEditor(component, 0, parent), 
    view_(new EditableTableView(this)),
    model_(new ChoicesModel(component->getChoices(), this))
{
    ChoicesDelegate* delegate = new ChoicesDelegate(component->getChoices(), view_);
    view_->setItemDelegate(delegate);
    view_->setItemsDraggable(false);
    view_->setModel(model_);

    connect(model_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItem(const QModelIndex&)),
        model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(view_, SIGNAL(removeItem(const QModelIndex&)),
        model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(delegate, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::~ChoicesEditor()
//-----------------------------------------------------------------------------
ChoicesEditor::~ChoicesEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::isValid()
//-----------------------------------------------------------------------------
bool ChoicesEditor::isValid() const
{
    return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::refresh()
//-----------------------------------------------------------------------------
void ChoicesEditor::refresh()
{
    // TODO.
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::showEvent()
//-----------------------------------------------------------------------------
void ChoicesEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/choices.html");
}

//-----------------------------------------------------------------------------
// Function: ChoicesEditor::setupLayout()
//-----------------------------------------------------------------------------
void ChoicesEditor::setupLayout()
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Choices"), this);

    // create the layout, add widgets to it
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(view_, 1);
    layout->setContentsMargins(0, 0, 0, 0);
}