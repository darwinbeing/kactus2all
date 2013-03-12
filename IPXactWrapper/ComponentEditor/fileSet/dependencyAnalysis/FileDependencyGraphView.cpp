//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.01.2013
//
// Description:
// Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------

#include "FileDependencyGraphView.h"

#include "FileDependencyModel.h"
#include "FileDependencyDelegate.h"
#include "FileDependencyItem.h"

#include <models/FileDependency.h>

#include <QHeaderView>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QDebug>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphView::FileDependencyGraphView(QWidget* parent)
    : QTreeView(parent),
      model_(0),
      columns_(),
      maxVisibleGraphColumns_(0),
      scrollIndex_(0),
      selectedDependency_(0),
      drawingDependency_(false),
      filters_(255)
{
    setUniformRowHeights(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setItemDelegate(new FileDependencyDelegate(this));

    connect(header(), SIGNAL(sectionResized(int, int, int)),
            this, SLOT(onSectionResized()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphView::~FileDependencyGraphView()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setModel()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setModel(QAbstractItemModel* model)
{
    // Disconnect the previous model.
    if (model_ != 0)
    {
        model_->disconnect(this);
    }

    if (model != 0)
    {
        FileDependencyModel* depModel = dynamic_cast<FileDependencyModel*>(model);
        Q_ASSERT(depModel != 0);

        connect(depModel, SIGNAL(dependencyAdded(FileDependency*)),
                this, SLOT(onDependencyAdded(FileDependency*)), Qt::UniqueConnection);
        connect(depModel, SIGNAL(dependencyChanged(FileDependency*)),
                this, SLOT(onDependencyChanged(FileDependency*)), Qt::UniqueConnection);
        connect(depModel, SIGNAL(modelReset()),
                this, SLOT(onModelReset()), Qt::UniqueConnection);
        model_ = depModel;
    }
    else
    {
        model_ = 0;
    }

    QTreeView::setModel(model);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onDependencyAdded()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependencyAdded(FileDependency* dependency)
{
    FileDependencyItem* fromItem = dependency->getFileItem1();
    FileDependencyItem* toItem = dependency->getFileItem2(); 

//     if (fromItem == 0 || toItem == 0)
//     {
//         fromItem = model_->findFileItem(dependency->getFile1());
//         toItem = model_->findFileItem(dependency->getFile2());
//         dependency->setItemPointers(fromItem, toItem);
//     }

    if (fromItem != 0 && toItem != 0)
    {
        // Determine the y coordinates for the dependency.
        int fromY = 0;
        int toY = 0;

        getVisualRowY(model_->getItemIndex(fromItem, 0), fromY);
        getVisualRowY(model_->getItemIndex(toItem, 0), toY);

        // The user may be scrolling the view so scroll bar position must be taken into account.
        int vOffset = verticalOffset();
        fromY += vOffset;
        toY += vOffset;

        GraphDependency graphDep(dependency, fromY, toY);

        // Find the first column from the left which has space for the dependency.
        GraphColumn* selColumn = 0;
        int x = GRAPH_MARGIN - scrollIndex_ * GRAPH_SPACING;

        for (int i = 0; i < columns_.size(); ++i)
        {
            GraphColumn* column = &columns_[i];

            if (hasSpace(*column, graphDep))
            {
                selColumn = column;
                break;
            }

            x += GRAPH_SPACING;
        }

        // If no free column was found, create a new one.
        if (selColumn == 0)
        {
            columns_.append(GraphColumn());
            selColumn = &columns_.back();

            onSectionResized();
        }
        
        selColumn->dependencies.append(graphDep);

        // Repaint only the region of the new dependency.
        int columnOffset = columnViewportPosition(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);
        viewport()->repaint(QRect(columnOffset + x - ARROW_WIDTH, qMin(fromY, toY) - POINTER_OFFSET,
                                  2 * ARROW_WIDTH, qAbs(toY - fromY) + 2 * POINTER_OFFSET));
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onDependencyChanged()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependencyChanged(FileDependency* dependency)
{
    // TODO: Repaint only the area of the dependency.
    viewport()->repaint();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::paintEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::paintEvent(QPaintEvent* event)
{
    QTreeView::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();

    // Draw the dependency graph. 
    drawDependencyGraph(painter, event->rect());

    // Draw the manual dependency arrow if a dependency is being created.
    if (drawingDependency_)
    {
        drawManualCreationArrow(painter);
    }

    painter.restore();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::mousePressEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::mousePressEvent(QMouseEvent* event)
{
    int column = columnAt(event->x());

    // Check if the user pressed over the dependencies column.
    if (column == FILE_DEPENDENCY_COLUMN_DEPENDENCIES)
    {
        if (event->button() == Qt::LeftButton)
        {
            // Search for a dependency under the cursor.
            FileDependency* dependency = findDependencyAt(event->pos());

            if (dependency != selectedDependency_)
            {
                selectedDependency_ = dependency;
                viewport()->repaint();

                emit selectionChanged(selectedDependency_);
            }
        }
    }
    // Otherwise check if the user pressed over the manual creation column.
    else if (column == FILE_DEPENDENCY_COLUMN_CREATE)
    {
        // TODO[Tommi]
        // Use indexAt() to retrieve the model index.
        // Use modelIndex.internalPointer() to retrieve the FileDependencyItem* pointer.
        // Use getRowY(modelIndex) function to retrieve the center y coordinate for the row specified by the model index.
    }
    else
    {
        QTreeView::mousePressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::mouseMoveEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::mouseMoveEvent(QMouseEvent* event)
{
    QTreeView::mousePressEvent(event);

    // Same helper functions apply here as well.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawArrow(QPainter& painter, int x, int fromY, int toY,
                                          QColor const& color, bool bidirectional)
{
    painter.setPen(QPen(color, 2));
    painter.setBrush(QBrush(color));
    painter.drawLine(x, fromY, x, toY);

    if (bidirectional)
    {
        if (fromY < toY)
        {
            QPoint pointsFrom[3] = {
                QPoint(x - ARROW_WIDTH, fromY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, fromY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x, fromY - POINTER_OFFSET)
            };

            QPoint pointTo[3] = {
                QPoint(x - ARROW_WIDTH, toY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, toY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x, toY + POINTER_OFFSET)
            };

            painter.drawPolygon(pointsFrom, 3);
            painter.drawPolygon(pointTo, 3);
        }
        else
        {
            QPoint pointsFrom[3] = {
                QPoint(x - ARROW_WIDTH, fromY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, fromY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x, fromY + POINTER_OFFSET)
            };

            QPoint pointTo[3] = {
                QPoint(x - ARROW_WIDTH, toY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, toY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x, toY - POINTER_OFFSET)
            };

            painter.drawPolygon(pointsFrom, 3);
            painter.drawPolygon(pointTo, 3);
        }
    }
    else
    {
        painter.drawPie(x - DOT_RADIUS, fromY - DOT_RADIUS, 2 * DOT_RADIUS, 2 * DOT_RADIUS, 0, 5760);

        if (fromY < toY)
        {
            QPoint points[3] = {
                QPoint(x - ARROW_WIDTH, toY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, toY + POINTER_OFFSET - ARROW_HEIGHT),
                QPoint(x, toY + POINTER_OFFSET)
            };

            painter.drawPolygon(points, 3);
        }
        else
        {
            QPoint points[3] = {
                QPoint(x - ARROW_WIDTH, toY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x + ARROW_WIDTH, toY - POINTER_OFFSET + ARROW_HEIGHT),
                QPoint(x, toY - POINTER_OFFSET)
            };

            painter.drawPolygon(points, 3);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setFilters()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setFilters(DependencyFilters filters)
{
    // Save filters and apply.
    filters_ = filters;
    applyFilters();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getFilters()
//-----------------------------------------------------------------------------
FileDependencyGraphView::DependencyFilters FileDependencyGraphView::getFilters() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::applyFilters()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::applyFilters()
{
    // TODO: Possible to make only after the data structure is fully ready.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getRowY()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::getVisualRowY(QModelIndex const& index, int& y) const
{
    QRect rect = visualRect(index);
    
    if (rect.isNull())
    {
        return false;
    }

    y = rect.center().y();
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::hasSpace()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::hasSpace(GraphColumn const& column, GraphDependency const& dependency) const
{
    int minY = qMin(dependency.fromY, dependency.toY) - SAFE_MARGIN;
    int maxY = qMax(dependency.fromY, dependency.toY) + SAFE_MARGIN;

    foreach (GraphDependency const& dep, column.dependencies)
    {
        if (!(maxY < qMin(dep.fromY, dep.toY) || minY > qMax(dep.fromY, dep.toY)))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawDependencies()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawDependencyGraph(QPainter& painter, QRect const& rect)
{
    int columnOffset = columnViewportPosition(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);
    int width = columnWidth(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);
    int x = GRAPH_MARGIN;
    
    for (int i = scrollIndex_; i < qMin(columns_.size(), scrollIndex_ + maxVisibleGraphColumns_); ++i)
    {
        GraphColumn const& column = columns_[i];

        foreach (GraphDependency const& dep, column.dependencies)
        {
            int fromY = 0;
            int toY = 0;
            
            if (getCoordinates(dep, fromY, toY))
            {
                // Cull arrows that are not inside the view rectangle.
                QRect arrowRect(columnOffset + x - ARROW_WIDTH,
                                qMin(fromY, toY) - POINTER_OFFSET,
                                2 * ARROW_WIDTH,
                                qAbs(toY - fromY) + 2 * POINTER_OFFSET);

                if (arrowRect.intersects(rect))
                {
                    // Choose color for the arrow based on the dependency information
                    // and the selected dependency.
                    QColor color = Qt::black;

                    if (dep.dependency == selectedDependency_)
                    {
                        color = Qt::blue;
                    }
                    else if (dep.dependency->isManual())
                    {
                        color = Qt::magenta;
                    }

                    drawArrow(painter, columnOffset + x, fromY, toY, color, dep.dependency->isBidirectional());
                }
            }
        }

        x += GRAPH_SPACING;
    }

    // Draw coverage of the dependencies that are out of sight.
    painter.setPen(QPen(QColor(0, 158, 255), 2));
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Left side coverage.
    x = columnOffset + 2;

    for (int i = 0; i < scrollIndex_; ++i)
    {
        GraphColumn const& column = columns_[i];

        foreach (GraphDependency const& dep, column.dependencies)
        {
            // Determine the y coordinates for the dependency.
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY))
            {
                painter.drawLine(x, fromY, x, toY);
            }
        }
    }

    // Right side coverage.
    x = columnOffset + width - 2;

    for (int i = scrollIndex_ + maxVisibleGraphColumns_; i < columns_.size(); ++i)
    {
        GraphColumn const& column = columns_[i];

        foreach (GraphDependency const& dep, column.dependencies)
        {
            // Determine the y coordinates for the dependency.
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY))
            {
                painter.drawLine(x, fromY, x, toY);
            }
        }
    }

//     Left side coverage.
//     int covTop = 0;
//     int covBottom = 0;
//     computeGraphCoverage(0, scrollIndex_ - 1, 0, viewport()->height(), covTop, covBottom);
// 
//     if (covTop < covBottom)
//     {
//         painter.drawLine(columnOffset + 2, covTop, columnOffset + 2, covBottom);
//     }
// 
//     // Right side coverage.
//     computeGraphCoverage(scrollIndex_ + maxVisibleGraphColumns_, columns_.size() - 1,
//                          0, viewport()->height(), covTop, covBottom);
// 
//     if (covTop < covBottom)
//     {
//         painter.drawLine(columnOffset + width - 2, covTop, columnOffset + width - 2, covBottom);
//     }

    painter.setRenderHint(QPainter::Antialiasing);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawRow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
                                      QModelIndex const& index) const
{
    // Handle base drawing.
    QTreeView::drawRow(painter, option, index);

    // Draw a line below the row.
    QRect rowRect = visualRect(index);

    painter->save();
    painter->setPen(QPen(QColor(200, 200, 200), 0));
    painter->drawLine(option.rect.left(), rowRect.bottom(), option.rect.right(), rowRect.bottom());
    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onModelReset()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onModelReset()
{
    columns_.clear();
    selectedDependency_ = 0;

    emit graphColumnScollMaximumChanged(0);
    emit selectionChanged(0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawManualCreationArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawManualCreationArrow(QPainter& painter)
{
    // TODO[Tommi]: Draw the arrow based on the movements processed in the mouseMoveEvent().
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::setGraphColumnScrollIndex()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setGraphColumnScrollIndex(int index)
{
    scrollIndex_ = index;
    viewport()->repaint();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onSectionResized()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onSectionResized()
{
    int width = columnWidth(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);
    width -= 2 * GRAPH_MARGIN;

    if (width <= 0)
    {
        maxVisibleGraphColumns_ = 0;
    }
    else
    {
        maxVisibleGraphColumns_ = 1 + width / GRAPH_SPACING;
    }

    emit graphColumnScollMaximumChanged(qMax<int>(0, columns_.size() - maxVisibleGraphColumns_));
    emit dependencyColumnPositionChanged(columnViewportPosition(FILE_DEPENDENCY_COLUMN_DEPENDENCIES));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::findDependencyAt()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyGraphView::findDependencyAt(QPoint const& pt) const
{
    int x = columnViewportPosition(FILE_DEPENDENCY_COLUMN_DEPENDENCIES) + GRAPH_MARGIN;

    for (int i = scrollIndex_; i < columns_.size(); ++i)
    {
        GraphColumn const& column = columns_[i];

        // Check if the column contains the x coordinate.
        if (qAbs(x - pt.x()) <= ARROW_WIDTH + SELECTION_MARGIN)
        {
            // Go through all dependencies in the column until we find a match.
            foreach (GraphDependency const& dep, column.dependencies)
            {
                // Determine the y coordinates for the dependency.
                int fromY = 0;
                int toY = 0;

                if (getCoordinates(dep, fromY, toY))
                {
                    // Interval intersection test.
                    if (pt.y() >= qMin(fromY, toY) - SELECTION_MARGIN &&
                        pt.y() <= qMax(fromY, toY) + SELECTION_MARGIN)
                    {
                        return dep.dependency;
                    }
                }
            }

            break;
        }

        x += GRAPH_SPACING;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::computeGraphCoverage()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::computeGraphCoverage(int startColumn, int endColumn, int top, int bottom,
                                                   int& covTop, int& covBottom) const
{
    covTop = INT_MAX;
    covBottom = INT_MIN;

    for (int i = startColumn; i <= endColumn; ++i)
    {
        GraphColumn const& column = columns_[i];

        foreach (GraphDependency const& dep, column.dependencies)
        {
            // Determine the visual y coordinates for the dependency and check if the
            // dependency is visible at all.
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY))
            {
                // Check if the dependency intersects the query area (top-bottom).
                int minY = qMin(fromY, toY);
                int maxY = qMax(fromY, toY);

                if (maxY <= top || minY >= bottom)
                {
                    continue;
                }

                // Update coverage variables.
                covTop = qMin(covTop, qMax(top, minY));
                covBottom = qMax(covBottom, qMin(bottom, maxY));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::getCoordinates()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::getCoordinates(GraphDependency const &dep, int& fromY, int& toY) const
{
    FileDependencyItem* fromItem = dep.dependency->getFileItem1();
    FileDependencyItem* toItem = dep.dependency->getFileItem2();
    Q_ASSERT(fromItem != 0);
    Q_ASSERT(toItem != 0);

    // Determine the y coordinates for the dependency.
    // If the item is not visible, reroute the dependency to the missing item's parent.
    if (!getVisualRowY(model_->getItemIndex(fromItem, 0), fromY))
    {
        fromItem = fromItem->getParent();

        if (!getVisualRowY(model_->getItemIndex(fromItem, 0), fromY))
        {
            return false;
        }
    }

    if (!getVisualRowY(model_->getItemIndex(toItem, 0), toY))
    {
        toItem = toItem->getParent();

        if (!getVisualRowY(model_->getItemIndex(toItem, 0), toY))
        {
            return false;
        }
    }

    // The items can be same when both items are not visible and have a same parent.
    // In that case, the dependency is invisible.
    if (fromItem == toItem)
    {
        return false;
    }

    return true;
}
