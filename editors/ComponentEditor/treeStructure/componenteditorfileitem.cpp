//-----------------------------------------------------------------------------
// File: componenteditorfileitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single file in component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorfileitem.h"

#include <editors/ComponentEditor/fileSet/file/fileeditor.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QDesktopServices>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QProcess>
#include <QSettings>
#include <QStringList>
#include <QUrl>

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::ComponentEditorFileItem()
//-----------------------------------------------------------------------------
ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
editAction_(new QAction(tr("Edit"), this)),
editWithAction_(new QAction(tr("Edit/Run with..."), this)),
runAction_(new QAction(tr("Run"), this)),
openContainingFolderAction_(new QAction(tr("Open Containing Folder"), this) )
{
    connect(editAction_, SIGNAL(triggered(bool)), this, SLOT(openItem()), Qt::UniqueConnection);
    connect(editWithAction_, SIGNAL(triggered(bool)), this, SLOT(openWith()), Qt::UniqueConnection);
	connect(runAction_, SIGNAL(triggered(bool)), this, SLOT(run()), Qt::UniqueConnection);
	connect(openContainingFolderAction_, SIGNAL(triggered(bool)),
		this, SLOT(onOpenContainingFolder()), Qt::UniqueConnection);
}
//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::~ComponentEditorFileItem()
//-----------------------------------------------------------------------------
ComponentEditorFileItem::~ComponentEditorFileItem()
{

}
//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::text() const
{
    QFileInfo fileQfi = QFileInfo(file_->getName());
    return fileQfi.fileName();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::isValid() const
{
	// if the file is not valid
	if (!file_->isValid(true)) {
		return false;
	}

	QString filePath = file_->getName();

    // get the path to the xml file
    QString basePath = libHandler_->getPath(*component_->getVlnv());

    QString absPath;

    // if the path is relative then create absolute path
    QFileInfo originalInfo(filePath);
    if (originalInfo.isRelative())
    {
        absPath = General::getAbsolutePath(basePath, filePath);
    }
    // if the reference is directly absolute
    else
    {
        absPath = filePath;
    }

    // check if the file exists in the file system
    QFileInfo fileInfo(absPath);
    return fileInfo.exists();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFileItem::editor()
{
	if (!editor_) {
		editor_ = new FileEditor(libHandler_, component_, file_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));
        connect(editor_, SIGNAL(editFile()), this, SLOT(openItem()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(runFile()), this, SLOT(run()), Qt::UniqueConnection);
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::getTooltip() const
{
	return tr("Specifies a single file associated with the file set");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::canBeOpened() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::openItem()
{
    if (useKactusCSourceEditor())
    {
        emit openCSource(fileAbsolutePath(), component_);
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileAbsolutePath()));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::openWith()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::openWith()
{
    QString applicationPath = QFileDialog::getOpenFileName(0, tr("Select Application"));

    if (QFileInfo(applicationPath).isExecutable())
    {
        runInApplication(applicationPath);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::run()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::run()
{
    if (runExecutableSet())
    {
        runInApplication(executablePath()); 
    }
    else
    {
        emit errorMessage(tr("No executable set for file type(s)."));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::actions()
//-----------------------------------------------------------------------------
QList<QAction*> ComponentEditorFileItem::actions() const
{
    QList<QAction*> actionList;
    actionList.append(editAction_);
    actionList.append(editWithAction_);
	actionList.append(runAction_);
	actionList.append(openContainingFolderAction_);

    runAction_->setEnabled(runExecutableSet());

    return actionList;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::onEditorChanged() {
	
	// on file also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::absolutePath()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::fileAbsolutePath() const
{
    const QString relPath = file_->getName();
    const QString xmlPath = libHandler_->getPath(*component_->getVlnv());

    return General::getAbsolutePath(xmlPath, relPath);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::runInApplication()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::runInApplication(QString const& applicationPath)
{
    QStringList arguments(fileAbsolutePath());
    QProcess::startDetached(applicationPath, arguments);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::useKactusCSourceEditor()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::useKactusCSourceEditor() const
{
	QSettings settings;

	foreach(QString fileType, file_->getAllFileTypes())
	{
		QString key = "FileTypes/" + fileType + "/EditInKactus";

		if ( settings.value(key).toBool() )
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::runExecutableSet()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::runExecutableSet() const
{
    return !executablePath().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::executablePath()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::executablePath() const
{
    QSettings settings;

    foreach(QString fileType, file_->getAllFileTypes())
    {
        QString key = "FileTypes/" + fileType + "/Executable";
        QString executableName = settings.value(key).toString();

        executableName = resolveEnvironmentVariables(executableName);

        if (QFileInfo(executableName).isExecutable())
        {
            return executableName;
        }
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::resolveEnvironmentVariables()
//-----------------------------------------------------------------------------
QString ComponentEditorFileItem::resolveEnvironmentVariables(QString const& text) const
{
    QRegExp environmental("(\\$\\(?\\w+\\)?)");
    QString parsed = text;
    QSettings settings;

    parsed.indexOf(environmental);
   
    int variableCount = environmental.captureCount();
    for (int i = 1; i <= variableCount; i++)            //!< i = 0 contains the whole text, skip it.
    {
        QString variable = environmental.cap(i);
        QString variableName = variable;
        variableName.remove("$");
        
        // Remove enclosing parathesis.
        if (variableName.startsWith("("))
        {
            variableName = variableName.mid(1);
            variableName.chop(1);
        }

        QString key = "K2Variables/" + variableName + "/value";
        QString variableValue = settings.value(key).toString();

        parsed.replace(variable, variableValue);
    }

    return parsed;
}

//-----------------------------------------------------------------------------
// Function: HierarchyView::onOpenContainingFolder()
//-----------------------------------------------------------------------------
void ComponentEditorFileItem::onOpenContainingFolder()
{
	QString path = QFileInfo(fileAbsolutePath()).absolutePath();

	// Open the folder in the operating system's default file browser.
	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}