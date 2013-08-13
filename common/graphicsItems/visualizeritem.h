/* 
 *  	Created on: 15.10.2012
 *      Author: Antti Kamppi
 * 		filename: visualizeritem.h
 *		Project: Kactus 2
 */

#ifndef VISUALIZERITEM_H
#define VISUALIZERITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QRectF>
#include <QObject>

/*! \brief VisualizerItem is base class that provides interface to visualize IP-Xact models.
 *
 */
class VisualizerItem : public QObject, public QGraphicsRectItem {
	Q_OBJECT

public:

	enum NamePosition {
		NAME_INDENTED = 0,		//! The name label is indented.
		NAME_CENTERED			//! The name is centered in the middle of item.
	};

	//! \brief The minimum and maximum bounds for visualizer items.
	enum Bounds {
		ITEM_HEIGHT = 36,		//! The height of a single item
		MIN_WIDTH = 90,			//! The minimum width of an item
		MAX_WIDTH = 200,		//! The maximum width of a single item
		NAME_INDENTATION = 100, //! How much space the name leaves for left corners
        CORNER_INDENTATION = 4
	};

	//! \brief The font size to display text.
	enum FontSize {
		FONT_NAMESIZE = 10,		//! The size for name field
		FONT_CORNERSIZE = 10	//! The size for corner texts
	};

	/*! \brief The constructor.
	 *
	 * \param parent Pointer to the owner of this graphics item.
	 *
	*/
	VisualizerItem(QGraphicsItem* parent = 0);
	
	//! \brief The destructor
	virtual ~VisualizerItem();

	//! \brief Refresh the item and possible sub-items
	virtual void refresh() = 0;

	/*! \brief Get the rectangle reserved by this item and it's sub-items recursively.
	 *
	 * \return QRectF which contains the rectangles of this item and sub-items and their
	 * children recursively.
	*/
	virtual QRectF itemTotalRect() const;

	/*! \brief Get the displayed name of the object.
	 *
	 * \return QString containing the name
	*/
	virtual QString getName() const;

	/*! \brief Set the width for the item.
	 *
	 * \param width The new width of the item.
	 *
	*/
	virtual void setWidth(qreal width);

	/*! \brief Get the width of the item.
	 *
	 * This width is affected by the item's children so if children grow this 
	 * width grows accordingly.
	 *
	 * \return The width of the item and it's sub-items.
	*/
	virtual qreal itemTotalWidth() const;

	/*! \brief Set the name position of the item.
	 *
	 * \param namePos The position to be used.
	 *
	*/
	virtual void setNamePosition(const NamePosition namePos);

protected:

	/*! \brief Set the display name of the item.
	 *
	 * \param name The name to display.
	 *
	*/
	virtual void setName(const QString& name);

	/*! \brief Set text to the top left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftTopCorner(const QString& text);

	/*! \brief Set text to the bottom left corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setLeftBottomCorner(const QString& text);

	/*! \brief Set text to the top right corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setRightTopCorner(const QString& text);

	/*! \brief Set text to the bottom right corner.
	 *
	 * \param text The text to display in the corner.
	 *
	*/
	virtual void setRightBottomCorner(const QString& text);

	/*! \brief Get the minimum rectangle of the item.
	 *
	 * \return QRectF that limits the minimum size needed by this item.
	*/
	virtual QRectF minimumRect() const;

	/*! \brief Set new positions for child items.
	 * 
	 * The base class implementation only updates the position of the name and corner texts.
	*/
	virtual void reorganizeChildren();

private:
	
	//! \brief No copying
	VisualizerItem(const VisualizerItem& other);

	//! \brief No assignment
	VisualizerItem& operator=(const VisualizerItem& other);

	//! \brief Defines where the name label is positioned.
	NamePosition namePos_;

	//! \brief The label to display the name of the memory map.
	QGraphicsTextItem nameLabel_;

	//! \brief The label to display text in the left top corner.
	QGraphicsSimpleTextItem leftTopText_;

	//! \brief The label to display text in the left bottom corner.
	QGraphicsSimpleTextItem leftBottomText_;

	//! \brief The label to display text in the right top corner.
	QGraphicsSimpleTextItem rightTopText_;

	//! \brief The label to display text in the right bottom corner.
	QGraphicsSimpleTextItem rightBottomText_;
};

#endif // VISUALIZERITEM_H
