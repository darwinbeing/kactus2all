//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYGRAPHWIDGET_H
#define FILEDEPENDENCYGRAPHWIDGET_H

#include <QTreeView>

class FileDependency;
class FileDependencyModel;

//-----------------------------------------------------------------------------
//! Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------
class FileDependencyGraphWidget : public QTreeView
{
    Q_OBJECT

public:
    typedef unsigned char DependencyFilters;

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyGraphWidget(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyGraphWidget();

    /*!
     *  Sets the item model.
     *
     *      @param [in] model The model to set.
     */
    virtual void setModel(QAbstractItemModel* model);

    /*!
     *  Sets the filters for the graph.
     */
    void setFilters(DependencyFilters filters);

    /*!
     *  Returns the currently set filters for the graph.
     */
    DependencyFilters getFilters() const;

protected:
    /*!
     *  Draws the dependency graph on top of the tree view.
     */
    virtual void paintEvent(QPaintEvent* event);

    /*!
     *  Draws a row.
     */
    virtual void drawRow(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Handles mouse press events and manual connection creation.
     */
    virtual void mousePressEvent(QMouseEvent* event);

    /*!
     *  Handles mouse move events and manual connection creation.
     */
    virtual void mouseMoveEvent(QMouseEvent* event);

private slots:
    /*!
     *  Called when a dependency has been added.
     */
    void onDependencyAdded(FileDependency* dependency);

    /*!
     *  Called when the model has been reset.
     */
    void onModelReset();

private:
    //-----------------------------------------------------------------------------
    //! Graph dependency structure.
    //-----------------------------------------------------------------------------
    struct GraphDependency
    {
        FileDependency* dependency;
        int fromY;
        int toY;

        /*!
         *  Constructor.
         */
        GraphDependency(FileDependency* dependency, int fromY, int toY)
            : dependency(dependency),
              fromY(fromY),
              toY(toY)
        {
        }
    };
    
    //-----------------------------------------------------------------------------
    //! Graph column structure.
    //-----------------------------------------------------------------------------
    struct GraphColumn
    {
        QList<GraphDependency> dependencies;
        
        /*!
         *  Constructor.
         */
        GraphColumn() : dependencies()
        {
        }
    };

    // Disable copying.
    FileDependencyGraphWidget(FileDependencyGraphWidget const& rhs);
    FileDependencyGraphWidget& operator=(FileDependencyGraphWidget const& rhs);

    /*!
     *  Reorganizes the graph based on the currently set filters.
     */
    void applyFilters();

    /*!
     *  Draws the dependency graph into the dependencies column.
     *
     *      @param [in] painter The drawing context.
     */
    void drawDependencyGraph(QPainter& painter);

    /*!
     *  Draws the temporary manual dependency arrow when the user is creating a manual connection.
     *
     *      @param [in] painter The drawing context.
     */
    void drawManualCreationArrow(QPainter& painter);

    /*!
     *  Draws a dependency arrow.
     *
     *      @param [in] painter        The painter context.
     *      @param [in] x              The x coordinate for the arrow.
     *      @param [in] fromY          The "from" y coordinate.
     *      @param [in] toY            The "to" y coordinate.
     *      @param [in] color          The arrow color.
     *      @param [in] bidirectional  If true, arrow heads will be drawn to both ends.
     */
    void drawArrow(QPainter& painter, int x, int fromY, int toY, QColor const& color, bool bidirectional = false);

    /*!
     *  Returns the center y coordinate for the row specified by the given model index.
     */
    int getRowY(QModelIndex const& index) const;

    /*!
     *  Checks whether the given column has space for the given dependency.
     */
    bool hasSpace(GraphColumn const& column, GraphDependency const& dependency) const;
    

    enum
    {
        DOT_RADIUS = 2,
        ARROW_WIDTH = 3,
        ARROW_HEIGHT = 5,
        POINTER_OFFSET = 2,
        GRAPH_MARGIN = 10,
        GRAPH_SPACING = 20,
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The data model.
    FileDependencyModel* model_;

    //! The graph columns.
    QList<GraphColumn> columns_;

    //! The current scroll value of the dependency graph column.
    unsigned int scrollIndex_;

    //! If true, the user is currently drawing a dependency.
    bool drawingDependency_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYGRAPHWIDGET_H
