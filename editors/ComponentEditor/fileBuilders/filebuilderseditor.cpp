/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: filebuilderseditor.cpp
 *		Project: Kactus 2
 */

#include "filebuilderseditor.h"
#include "filebuildersdelegate.h"

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::FileBuildersEditor()
//-----------------------------------------------------------------------------
FileBuildersEditor::FileBuildersEditor(QList<QSharedPointer<FileBuilder> >& fileBuilders,
									   QWidget* parent):
QGroupBox(tr("Default file build commands"), parent),
view_(this), 
model_(fileBuilders, this), 
proxy_(this)
{
	// set view to be sortable
	view_.setSortingEnabled(true);

	// items can not be dragged
	view_.setItemsDraggable(false);

	// set the delegate to provide editors
	view_.setItemDelegate(new FileBuildersDelegate(this));

	// set source model for proxy
	proxy_.setSourceModel(&model_);
	// set proxy to be the source for the view
	view_.setModel(&proxy_);

	// sort the view
	view_.sortByColumn(0, Qt::AscendingOrder);

	// create the layout, add widgets to it
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(&view_);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::~FileBuildersEditor()
//-----------------------------------------------------------------------------
FileBuildersEditor::~FileBuildersEditor()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::isValid()
//-----------------------------------------------------------------------------
bool FileBuildersEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: FileBuildersEditor::refresh()
//-----------------------------------------------------------------------------
void FileBuildersEditor::refresh()
{
	view_.update();
}
