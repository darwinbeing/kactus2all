//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYINFOWIDGET_H
#define FILEDEPENDENCYINFOWIDGET_H

#include <QGroupBox>
#include <QPlainTextEdit>

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

private:
    // Disable copying.
    FileDependencyInfoWidget(FileDependencyInfoWidget const& rhs);
    FileDependencyInfoWidget& operator=(FileDependencyInfoWidget const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for description.
    QPlainTextEdit descEdit_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYINFOWIDGET_H
