//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 19.01.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYINFOWIDGET_H
#define FILEDEPENDENCYINFOWIDGET_H

#include <QGroupBox>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QComboBox>

class FileDependency;

//-----------------------------------------------------------------------------
//! Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------
class FileDependencyInfoWidget : public QGroupBox
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyInfoWidget(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyInfoWidget();

    /*!
     *  Sets the given file dependency editable in the widget.
     *
     *      @param [in] dependency The dependency to set for editing.
     *
     *      @remarks If the dependency is null, all contents of the widgets are cleared and
     *               the widgets are set to disabled mode.
     */
    void setEditedDependency(QSharedPointer<FileDependency> dependency);

    /*!
     *  Returns the currently edited dependency.
     */
    QSharedPointer<FileDependency> getEditedDependency() const;

private:
    // Disable copying.
    FileDependencyInfoWidget(FileDependencyInfoWidget const& rhs);
    FileDependencyInfoWidget& operator=(FileDependencyInfoWidget const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for description.
    QPlainTextEdit descEdit_;

    //! Combo box for changing dependency direction.
    QComboBox directionCombo_;
    
    //! Check box for locked state.
    QCheckBox lockedCheck_;

    //! The currently edited dependency.
    QSharedPointer<FileDependency> dependency_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYINFOWIDGET_H
