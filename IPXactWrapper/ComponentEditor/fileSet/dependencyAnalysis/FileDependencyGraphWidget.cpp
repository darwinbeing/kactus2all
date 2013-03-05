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

#include "FileDependencyGraphWidget.h"

#include "FileDependencyModel.h"
#include "FileDependencyDelegate.h"
#include "FileDependencyItem.h"

#include <models/FileDependency.h>

#include <QHeaderView>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::FileDependencyGraphWidget(QWidget* parent)
    : QTreeView(parent),
      model_(0),
      columns_(),
      scrollIndex_(0),
      drawingDependency_(false)
{
    setItemDelegate(new FileDependencyDelegate(this));
    columns_.append(GraphColumn());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::~FileDependencyGraphWidget()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setModel()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::setModel(QAbstractItemModel* model)
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
void FileDependencyGraphWidget::onDependencyAdded(FileDependency* dependency)
{
    FileDependencyItem* fromItem = model_->findFileItem(dependency->getFile1());
    FileDependencyItem* toItem = model_->findFileItem(dependency->getFile2());

    if (fromItem != 0 && toItem != 0)
    {
        // Determine the y coordinates for the dependency.
        int fromY = getRowY(model_->getItemIndex(fromItem, 0));
        int toY = getRowY(model_->getItemIndex(toItem, 0));

        GraphDependency graphDep(dependency, fromY, toY);

        // Find the first column from the left which has space for the dependency.
        GraphColumn* selColumn = 0;
        int x = GRAPH_MARGIN;

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
        }
        
        selColumn->dependencies.append(graphDep);

        // TODO: Repaint only the region of the new dependency.
        //repaint(QRect(x - ARROW_WIDTH, qMin(fromY, toY), x + ARROW_WIDTH, qMax(fromY, toY)));

        // Current implementation adds each dependency to its own column.
        // TODO: Change this to use all available space in the left-most possible column.
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::paintEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::paintEvent(QPaintEvent* event)
{
    QTreeView::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();

    // Draw the dependency graph. 
    drawDependencyGraph(painter);

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
void FileDependencyGraphWidget::mousePressEvent(QMouseEvent* event)
{
    QTreeView::mousePressEvent(event);

    // TODO[Tommi]
    // Use columnViewportPosition() and columnWidth() functions to check
    // if the user pressed the mouse button inside the creation column.

    // Use indexAt() to retrieve the model index.
    // Use modelIndex.internalPointer() to retrieve the FileDependencyItem* pointer.
    // Use getRowY(modelIndex) function to retrieve the center y coordinate for the row specified by the model index.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::mouseMoveEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::mouseMoveEvent(QMouseEvent* event)
{
    QTreeView::mousePressEvent(event);

    // Same helper functions apply here as well.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::drawArrow(QPainter& painter, int x, int fromY, int toY,
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
void FileDependencyGraphWidget::setFilters(DependencyFilters filters)
{
    // TODO: Save filters variable before applying.
    applyFilters();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getFilters()
//-----------------------------------------------------------------------------
FileDependencyGraphWidget::DependencyFilters FileDependencyGraphWidget::getFilters() const
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::applyFilters()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::applyFilters()
{
    // TODO: Possible to make only after the data structure is fully ready.
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getRowY()
//-----------------------------------------------------------------------------
int FileDependencyGraphWidget::getRowY(QModelIndex const& index) const
{
    return visualRect(index).center().y();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::hasSpace()
//-----------------------------------------------------------------------------
bool FileDependencyGraphWidget::hasSpace(GraphColumn const& column, GraphDependency const& dependency) const
{
    int minY = qMin(dependency.fromY, dependency.toY);
    int maxY = qMax(dependency.fromY, dependency.toY);

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
void FileDependencyGraphWidget::drawDependencyGraph(QPainter& painter)
{
    int columnOffset = columnViewportPosition(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);
    int x = GRAPH_MARGIN;
    int width = columnWidth(FILE_DEPENDENCY_COLUMN_DEPENDENCIES);

    for (int i = scrollIndex_; i < columns_.size(); ++i)
    {
        GraphColumn const& column = columns_[i];

        foreach (GraphDependency const& dependency, column.dependencies)
        {
            QColor color = Qt::black;

            if (dependency.dependency->isManual())
            {
                color = Qt::magenta;
            }

            drawArrow(painter, columnOffset + x, dependency.fromY, dependency.toY, color,
                      dependency.dependency->isBidirectional());
        }

        x += GRAPH_SPACING;

        // Check if we're out of space.
        if (x + GRAPH_MARGIN >= width)
        {
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawRow()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
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
void FileDependencyGraphWidget::onModelReset()
{
    columns_.clear();
    columns_.append(GraphColumn());
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawManualCreationArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphWidget::drawManualCreationArrow(QPainter& painter)
{
    // TODO[Tommi]: Draw the arrow based on the movements processed in the mouseMoveEvent().
}
