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
        DESCRIPTION,
        VALUEID,
        TYPE,
        VALUE,
        CHOICE,
        MINIMUM,
        MAXIMUM,
        RESOLVE,
        BITWIDTH,
        ARRAY_SIZE,
        ARRAY_OFFSET,
        COLUMN_COUNT,
    };
}
