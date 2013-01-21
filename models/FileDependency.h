//-----------------------------------------------------------------------------
// File: FileDependency.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// File dependency class.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCY_H
#define FILEDEPENDENCY_H

#include <QString>
#include <QDomNode>
#include <QObject>

//-----------------------------------------------------------------------------
//! File dependency class.
//-----------------------------------------------------------------------------
class KACTUS2_API FileDependency : public QObject
{
public:
    /*!
     *  Constructor.
     */
    FileDependency();

    /*!
     *  Copy constructor.
     */
    FileDependency(FileDependency const& rhs);

    /*!
     *  Constructor which reads the file dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    FileDependency(QDomNode& node);
    
    /*!
     *  Destructor.
     */
    ~FileDependency();

    /*!
     *  Sets the 'from' file.
     *
     *      @param [in] filename The name of the file.
     */
    void setFile1(QString const& filename);

    /*!
     *  Sets the 'to' file.
     *
     *      @param [in] filename The name of the file.
     */
    void setFile2(QString const& filename);

    /*!
     *  Sets the description.
     *
     *      @param [in] desc The description to set.
     */
    void setDescription(QString const& desc);

    /*!
     *  Sets the dependency locked/unlocked.
     *
     *      @param [in] locked If true, the dependency is set locked; false for unlocked.
     */
    void setLocked(bool locked);

    /*!
     *  Sets the dependency bidirectional.
     *
     *      @param [in] bidiretional If true, the dependency is set as bidirectional (two-way);
     *                               false for one-way dependency.
     */
    void setBidirectional(bool bidirectional);

    /*!
     *  Sets the dependency manual.
     *
     *      @param [in] manual If true, the dependency is treated as a manual dependency;
     *                         otherwise it is treated as an automatic (generated) one.
     */
    void setManual(bool manual);

    /*!
     *  Returns the name of the 'from' file.
     */
    QString const& getFile1() const;

    /*!
     *  Returns the name of the 'to' file.
     */
    QString const& getFile2() const;

    /*!
     *  Returns the description.
     */
    QString const& getDescription() const;

    /*!
     *  Returns true if the dependency is locked.
     */
    bool isLocked() const;

    /*!
     *  Returns true if the dependency is bidirectional.
     */
    bool isBidirectional() const;

    /*!
     *  Returns true if the dependency is manual.
     */
    bool isManual() const;

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // FILEDEPENDENCY_H
