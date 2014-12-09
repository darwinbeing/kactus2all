//-----------------------------------------------------------------------------
// File: ModelParameterColumns.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 06.11.2014
//
// Description:
// Common declarations for editing model parameters.
//-----------------------------------------------------------------------------

namespace ModelParameterColumns
{
    //! The editable columns in the model parameter editor.
    enum columns
    {
        NAME = 0,
        DISPLAY_NAME,
        DATA_TYPE,
        USAGE_TYPE,
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
