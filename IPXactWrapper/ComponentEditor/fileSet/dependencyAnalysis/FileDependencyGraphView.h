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

#ifndef FILEDEPENDENCYGRAPHVIEW_H
#define FILEDEPENDENCYGRAPHVIEW_H

#include <QTreeView>

class FileDependency;
class FileDependencyModel;
class FileDependencyItem;

//-----------------------------------------------------------------------------
//! Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------
class FileDependencyGraphView : public QTreeView
{
    Q_OBJECT

public:
    //-----------------------------------------------------------------------------
    //! Filter enumeration.
    //-----------------------------------------------------------------------------
    enum DependencyFilter
    {
        FILTER_GREEN        = 0x0001,
        FILTER_YELLOW       = 0x0002,
        FILTER_RED          = 0x0004,
        FILTER_TWO_WAY      = 0x0008,
        FILTER_ONE_WAY      = 0x0010,
        FILTER_MANUAL       = 0x0020,
        FILTER_AUTOMATIC    = 0x0040,
        FILTER_INTERNAL     = 0x0080,
        FILTER_EXTERNAL     = 0x0100,
        FILTER_DIFFERENCE   = 0x0200,

        FILTER_DEFAULT = FILTER_GREEN | FILTER_YELLOW | FILTER_RED |
                         FILTER_TWO_WAY | FILTER_ONE_WAY |
                         FILTER_MANUAL | FILTER_AUTOMATIC |
                         FILTER_INTERNAL/* | FILTER_EXTERNAL*/
    };

    //! Bit field type for the filters.
    typedef unsigned int DependencyFilters;

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyGraphView(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~FileDependencyGraphView();

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

signals:
    /*!
     *  Emitted when a file dependency selection has changed.
     */
    void selectionChanged(FileDependency* dependency);

    /*!
     *  Emitted when the maximum for the graph column scroll has changed.
     */
    void graphColumnScollMaximumChanged(int maximum);

    /*!
     *  Emitted when the dependency column (left edge) position changes.
     */
    void dependencyColumnPositionChanged(int pos);

public slots:
    /*!
     *  Sets the graph column scroll index.
     *
     *      @param [in] index The index to set.
     */
    void setGraphColumnScrollIndex(int index);

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

    /*!
     *  Handles auto-expand when rows are inserted.
     *
     *      @param [in] parent The parent model index.
     *      @param [in] start  The start index of the inserted rows.
     *      @param [in] end    The end index of the inserted rows.
     */
    virtual void rowsInserted(QModelIndex const& parent, int start, int end);

    virtual void reset();

private slots:
    /*!
     *  Called when a dependency has been added.
     */
    void onDependencyAdded(FileDependency* dependency, bool immediateRepaint = true);

    /*!
     *  Called when a dependency has been changed.
     */
    void onDependencyChanged(FileDependency* dependency);

    /*!
     *  Called when a dependency has been removed.
     */
    void onDependencyRemoved(FileDependency* dependency);

    /*!
     *  Called when the model has been reset.
     */
    void onModelReset();

    /*!
     *  Called when any header section has been resized.
     */
    void onSectionResized();

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
    FileDependencyGraphView(FileDependencyGraphView const& rhs);
    FileDependencyGraphView& operator=(FileDependencyGraphView const& rhs);

    /*!
     *  Reorganizes the graph based on the currently set filters.
     */
    void applyFilters();

    /*!
     *  Draws the dependency graph into the dependencies column.
     *
     *      @param [in] painter  The drawing context.
     *      @param [in] rect     The currently visible drawing area.
     */
    void drawDependencyGraph(QPainter& painter, QRect const& rect);

    /*!
     *  Retrieves the final drawing coordinates and the items for the given dependency.
     *
     *      @param [in]  dep    The dependency.
     *      @param [out] fromY  The resulted from y coordinate.
     *      @param [out] toY    The resulted to y coordinate.
     */
    bool getCoordinates(GraphDependency const& dep, int& fromY, int& toY) const;

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
     *  Retrieves the center y coordinate for the row specified by the given model index.
     *
     *      @param [in] index  The model index of the row.
     *      @param [in] y      The retrieved y coordinate.
     *
     *      @return True, if the row is visible. False if hidden.
     */
    bool getVisualRowY(QModelIndex const& index, int& y) const;

    /*!
     *  Checks whether the given column has space for the given dependency.
     */
    bool hasSpace(GraphColumn const& column, GraphDependency const& dependency) const;

    /*!
     *  Searches for a dependency at the given mouse coordinate.
     *
     *      @param [in] pt The mouse coordinate point to test for.
     *
     *      @return If found, the dependency at the given coordinate. Otherwise null.
     */
    FileDependency* findDependencyAt(QPoint const& pt) const;

    /*!
     *  Calculates the vertical coverage of the graph columns startColumn-endColumn inside the given query area.
     *
     *      @param [in]  startColumn  The start index of which columns to check for.
     *      @param [in]  endColumn    The end index of which columns to check for (inclusive).
     *      @param [in]  top          The top y coordinate of the query area.
     *      @param [in]  bottom       The bottom y coordinate of the query area.
     *      @param [out] covTop       The top y coordinate of the resulted coverage.
     *      @param [out] covBottom    The bottom y cooordinate of the resulted coverage.
     */
    void computeGraphCoverage(int startColumn, int endColumn, int top, int bottom,
                              int& covTop, int& covBottom) const;

    /*!
     *  Checks the dependency against the filters.
     *
     *      @param [in] dependency The dependency to check.
     *
     *      @return False, if the dependency should not be shown. Otherwise true.
     */
    bool filterDependency(FileDependency const* dependency) const;

    enum
    {
        DOT_RADIUS = 2,       //!< The radius of the "from" dot for the arrows.
        ARROW_WIDTH = 3,      //!< The half width of the arrow head.
        ARROW_HEIGHT = 6,     //!< The height of the arrow head.
        POINTER_OFFSET = 2,   //!< Pointer offset for drawing the arrow heads.
        SAFE_MARGIN = 5,      //!< Safe margin for placing the dependencies into the columns.
        GRAPH_MARGIN = 10,    //!< Left & right for the dependency graph in the Dependencies column.
        GRAPH_SPACING = 20,   //!< Spacing between consecutive graph columns.
        SELECTION_MARGIN = 4, //!< Mouse selection margin for dependencies.
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The data model.
    FileDependencyModel* model_;

    //! The graph columns.
    QList<GraphColumn> columns_;

    //! The number of graph columns that can be visible.
    int maxVisibleGraphColumns_;

    //! The current scroll value of the dependency graph column.
    int scrollIndex_;

    //! The currently selected dependency.
    FileDependency* selectedDependency_;

    //! If true, the user is currently drawing a dependency.
    bool drawingDependency_;

    //! Current filters used on the graph.
    DependencyFilters filters_;

    //! The start item of manual dependency creation
    FileDependencyItem* manualDependencyStartItem_;
    FileDependencyItem* manualDependencyEndItem_;

};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYGRAPHVIEW_H
