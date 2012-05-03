//-----------------------------------------------------------------------------
// File: PlatformComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�
// Date: 26.9.2011
//
// Description:
// Graphics item for representing SW platform components.
//-----------------------------------------------------------------------------

#ifndef PLATFORMCOMPONENTITEM_H
#define PLATFORMCOMPONENTITEM_H

#include "../SystemDesign/SWComponentItem.h"

#include <common/graphicsItems/GraphicsItemTypes.h>

//-----------------------------------------------------------------------------
//! PlatformComponentItem class.
//-----------------------------------------------------------------------------
class PlatformComponentItem : public SWComponentItem
{
    Q_OBJECT 

public:
    enum { Type = GFX_TYPE_PLATFORM_COMPONENT };

    /*!
     *  Constructor.
     */
    PlatformComponentItem(QSharedPointer<Component> component,
                          QString const& instanceName = QString("unnamed"),
                          QString const& displayName = QString(),
                          QString const& description = QString(),
                          QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                          QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~PlatformComponentItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    int type() const { return Type; }

private:
    // Disable copying.
    PlatformComponentItem(PlatformComponentItem const& rhs);
    PlatformComponentItem& operator=(PlatformComponentItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Icon for hierarchical platform components.
    QGraphicsPixmapItem* hierIcon_;

    //! Icon for imported SW components.
    QGraphicsPixmapItem* importedIcon_;
};

//-----------------------------------------------------------------------------

#endif // PLATFORMCOMPONENTITEM_H
