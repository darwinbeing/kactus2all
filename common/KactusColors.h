//-----------------------------------------------------------------------------
// File: KactusColors.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 6.6.2012
//
// Description:
// Common colors used in Kactus2.
//-----------------------------------------------------------------------------

#ifndef KACTUSCOLORS_H
#define KACTUSCOLORS_H

#include <QColor>

namespace KactusColors
{
    //! Color used for selections in design diagrams.
    QColor const DIAGRAM_SELECTION = Qt::blue;

    //! Color used for broken/invalid connections in design diagrams.
    QColor const BROKEN_CONNECTION = Qt::red;

    //! Special color used for COM connections.
    QColor const COM_CONNECTION = QColor(49, 132, 61);//QColor(136, 206, 134);

    //! Color used for allowed visualization in design diagrams.
    QColor const DIAGRAM_ALLOWED_INTERFACE = Qt::magenta; //QColor(0, 220, 0);

    //! Colors used for component in different scenarios.
    QColor const MISSING_COMPONENT = QColor(0xe8, 0xc5, 0xc5);
    QColor const DRAFT_COMPONENT = QColor(217, 217, 217);
    QColor const HW_BUS_COMPONENT = QColor(0xce,0xdf,0xff);
    QColor const HW_COMPONENT = QColor(0xa5,0xc3,0xef);
    QColor const SW_COMPONENT = QColor(0xc5,0xff, 0xab);
}

//-----------------------------------------------------------------------------

#endif // KACTUSCOLORS_H
