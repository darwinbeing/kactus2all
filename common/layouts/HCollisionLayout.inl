//-----------------------------------------------------------------------------
// File: HCollisionLayout.inl
//-----------------------------------------------------------------------------
// Project: Endpoint Designer
// Author: Joni-Matti M��tt�
// Date: 22.4.2011
//
// Description:
// Vertical collision-based layout.
//-----------------------------------------------------------------------------

namespace HCollisionLayout
{
    //-----------------------------------------------------------------------------
    // Function: updateItemMove()
    //-----------------------------------------------------------------------------
    template <class T>
    void updateItemMove(QList<T*>& items, T* item, qreal minX, qreal spacing)
    {
        item->setY(std::max(minX - item->boundingRect().left(), item->y()));

        // Remove the moved item from the list.
        items.removeAt(items.indexOf(item));

        int index = 0;
        qreal x = minX;

        // Phase 1. Skip over the items that are fully above the moved item.
        for (index = 0; index < items.size(); ++index)
        {
            T* curItem = items.at(index);

            if (curItem->x() + curItem->boundingRect().right() + spacing >
                item->x() + item->boundingRect().left())
            {
                break;
            }

            x = curItem->x() + curItem->boundingRect().right() + spacing;
        }

        // Phase 2. Move the first colliding item above the moved item if it is possible
        // and the moved item is at least slightly over the colliding item.
        if (index < items.size())
        {
            T* curItem = items.at(index);

            if (item->x() < curItem->x() && curItem->x() + curItem->boundingRect().left() <
                item->x() + item->boundingRect().right() + spacing &&
                x + curItem->boundingRect().width() + spacing <= item->x() + item->boundingRect().left() &&
                item->x() + item->boundingRect().left() - spacing - curItem->boundingRect().right() >= minX)
            {
                curItem->setY(item->x() + item->boundingRect().left() - spacing -
                    curItem->boundingRect().right());

                ++index;
            }
        }

        // Phase 3. Put the moved item to its correct place.
        items.insert(index, item);

        // Phase 4. Move the rest of the items below the moved item so that they do not collide.
        while (index < items.size() - 1)
        {
            T* item1 = items.at(index);
            T* item2 = items.at(index + 1);

            if (item1->x() + item1->boundingRect().right() + spacing >
                item2->x() + item2->boundingRect().left())
            {
                item2->setY(item1->x() + item1->boundingRect().right() + spacing -
                    item2->boundingRect().left());
            }

            ++index;
        }
    }

    //-----------------------------------------------------------------------------
    // Function: setItemPos()
    //-----------------------------------------------------------------------------
    template <class T>
    void setItemPos(QList<T*> const& /*items*/, T* item, qreal y, qreal /*minX*/, qreal /*spacing*/)
    {
        item->setY(y);
    }
}
