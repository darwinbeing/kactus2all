//-----------------------------------------------------------------------------
// File: ParameterColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.11.2014
//
// Description:
// Common declarations for editing parameters.
//-----------------------------------------------------------------------------

namespace ParameterColumns
{
    //! The editable columns in the parameter editor.
    enum columns
    {
        NAME = 0,
        DISPLAY_NAME,
        TYPE,
        MINIMUM,
        MAXIMUM,
        CHOICE,
        VALUE,
        RESOLVE,
        BITWIDTH,
        ARRAY_SIZE,
        ARRAY_OFFSET,
        DESCRIPTION,
        COLUMN_COUNT
    };
}
