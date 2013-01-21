//-----------------------------------------------------------------------------
// File: FileDependencyModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// The data model for the file dependencies.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYMODEL_H
#define FILEDEPENDENCYMODEL_H

//-----------------------------------------------------------------------------
//! The data model for the file dependencies.
//-----------------------------------------------------------------------------
class FileDependencyModel
{
public:
    /*!
     *  Constructor.
     */
    FileDependencyModel();

    /*!
     *  Destructor.
     */
    ~FileDependencyModel();

private:
    // Disable copying.
    FileDependencyModel(FileDependencyModel const& rhs);
    FileDependencyModel& operator=(FileDependencyModel const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYMODEL_H
