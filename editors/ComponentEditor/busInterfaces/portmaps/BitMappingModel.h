//-----------------------------------------------------------------------------
// File: BitMappingModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.09.2013
//
// Description:
// Table model for bit field mapping.
//-----------------------------------------------------------------------------

#ifndef BITMAPPINGMODEL_H
#define BITMAPPINGMODEL_H

#include <QList>
#include <QAbstractTableModel>
#include <QMimeData>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/abstractiondefinition.h>

class BusInterface;
class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! BitMappingModel class.
//-----------------------------------------------------------------------------
class BitMappingModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    //! Column numbering.
    enum Columns
    {        
        INDEX,
        BIT,
        COLUMN_COUNT
    };

    //! The constructor.
    BitMappingModel(BusInterface* busif,
        QSharedPointer<Component> component,
        LibraryInterface* libHandler, 
        QObject* parent = 0);

    //! The destructor.
    virtual ~BitMappingModel();
	
	/*! Get the number of rows in the model.
	 *
	 * @param [in] parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * @return Number of rows currently in the model.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! Get the number of columns in the model
	 *
	 * @param [in] parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of columns currently in the model.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! Get the data for the specified item for specified role.
	 *
	 * @param [in] index Identifies the item that's data is wanted.
	 * @param [in] role Specifies what kind of data is wanted.
	 *
	 * @return QVariant containing the data.
	 */
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! Get the data for the headers
	 *
	 * @param [in] section The column that's header is wanted.
	 * @param [in] orientation Only Qt::Horizontal is supported.
	 * @param [in] role Specified the type of data that is wanted.
	 *
	 * @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! Set the data for specified item.
	 *
	 * @param [in] index Specifies the item that's data is modified
	 * @param [in] value The value to be set.
	 * @param [in] role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * @return True if data was successfully set in non-locked column.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! Set the header data for specified section.
	 *
	 * @param [in] section Specifies the item that's data is modified
	 * @param [in] orientation The header's orientation. Only Qt::Horizontal is supported.
	 * @param [in] value The value to be set.
	 * @param [in] role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * @return True if the header was successfully set.
	 */
    bool setHeaderData( int section, Qt::Orientation orientation, 
        const QVariant & value, int role = Qt::EditRole );

	/*! Get information on how specified item can be handled.
	 *
	 * @param [in] index Specifies the item that's flags are wanted.
	 *
	 * @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Check if the model is in valid state or not.
	 *
	 * @return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! Set the abstraction definition that is used in this port map.
	 *
	 * \param vlnv Identifies the abstraction definition.
	 *
	*/
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

    /*!
     *  Gets the name of the logical signal being mapped.
     *
     *
     *      @return Name of the logical signal.
     */
    QString getLogicalSignal() const;

    /*!
     *  Generates the port maps according to the visible mapping.
     *
     *      @return The generated port maps.
     */
    QList< QSharedPointer<PortMap> > getPortMaps() const;

    /*!
     *  Returns the model's supported drop actions.
     *     
     *      @return The supported drop actions.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns the supported mime types in a drop action.
     *
     *      @return The supported mime types.
     */
    virtual QStringList mimeTypes() const;

    /*!
     *  Handler for mime data in a drop action.
     *
     *      @param [in] data    The mimedata associated with the drop.
     *      @param [in] action  The performed drop action.
     *      @param [in] row     Target row in the model.
     *      @param [in] column  Target column in the model.
     *      @param [in] parent  Target parent of the model.
     *
     *      @return True, if the data could be handled with the model, otherwise false.
     */
    virtual bool dropMimeData(const QMimeData *data,
        Qt::DropAction action, int row, int column, const QModelIndex &parent);

    /*!
     *  Maps the given ports to first unmapped index(es).
     *
     *      @param [in] portNames   The names of the ports to map.
     */
    virtual void mapToEnd(QStringList portNames);

signals:

    /*!
     *  Emitted when the contents of the model change.
     */
    void contentChanged();

    /*!
     *  Emitted when new logical port has been set.
     *
     */
    void logicalPortSet(QString const&);    

    /*!
     *  Emitted when an error message must be shown to user.
     *
     */
    void errorMessage(QString const& msg);

public slots:

	/*! Sets the logical signal for mapping.
     *     
     * @param [in] logicalName      The name of the logical signal to set.
     */
    void onSetLogicalSignal(QString const& logicalName);

    /*!
     *  Removes all port maps of a logical port.
     *
     *      @param [in] logicalName   The port whose mappings to remove.
     */
    void onRemoveMapping(QString const& logicalName);

private:

    //! No copying.
	BitMappingModel(const BitMappingModel& other);

	//! No assignment.
	BitMappingModel& operator=(const BitMappingModel& other);

    /*!
     *  Adds empty rows to the end of the model.
     *
     *      @param [in] count   Number of rows to add.
     */
    void addRows(int count);

    /*!
     *  Removes rows from the end of the model.
     *
     *      @param [in] count   Number of rows to remove.
     */
    void removeRows(int count);

    /*!
     *  Adds an edit row to the end of the model. Dropping port on the edit row adds more rows.
     */
    void addEditRow();

    /*!
     *  Removes the edit row from the end of the model, if edit row exists.
     */
    void removeEditRow();

    /*!
     *  Checks if the physical port connection is already made with the logical port.
     *
     *      @param [in] connection   The physical port connection.
     *      @param [in] row          The logical index (row in the model).
     *
     *      @return True, if connection already exists, otherwise false.
     */
    bool hasDuplicates(General::PortBounds connection, int row) const;

    /*!
     *  Checks if the physical ports have a compatible direction with the logical port.
     *
     *      @param [in] ports   The ports to check.
     *
     *      @return True, if all the directions are compatible, otherwise false.
     */
    bool checkDirectionForPorts(QStringList const& ports);

    /*!
     *  Adds a given bit mapping to a logical row.
     *
     *      @param [in] mapping     The mapping to add.
     *      @param [in] row         The logical row to add to.
     *
     *      @return The last mapped row.
     */
    int addMapping( General::PortBounds mapping, int row );

    /*!
     *  Creates the initial table for selected logical signal based on existing port maps.               
     */
    void createInitialMappings();

    /*!
     *  Saves the current mapping in the table.     
     */
    void saveCurrentMappings();

    /*!
     *  Restores a previously saved mapping for selected logical signal.
     */
    void restoreMappings();

    /*!
     *  Checks if a given physical port is already mapped to a logical index in the given set of port maps.
     *
     *      @param [in] mappings        The set of port maps to compare.
     *      @param [in] pin             The physical mapping to find.
     *      @param [in] logicalIndex    The logical index to check.
     *
     *      @return True, if a mapping exists, otherwise false.
     */
    bool isMapped( QList<QSharedPointer<PortMap> > mappings, General::PortBounds const& pin, 
        int logicalIndex) const;


    /*!
     *  Creates a port map expanding the maximum amount of contiguous physical and logical bits.
     *
     *      @param [in] pin                 The physical mapping to create the map for.
     *      @param [in] firstLogicalIndex   The first logical index to map.
     *
     *      @return The created port map.
     */
    QSharedPointer<PortMap> createPortMapForPin( General::PortBounds const& pin, 
        int firstLogicalIndex ) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Table rows that correspond to the bit indexes in the logical signal.
    QList< QList<General::PortBounds> > rows_;

    QMap< QString, QList< QList<General::PortBounds> > > mappings_;

    //! Pointer to the bus interface being edited.
    BusInterface* busif_;

    //! Bus interface mode.
    General::InterfaceMode mode_;

    //! Name of the logical port being mapped.
    QString logicalPort_;

    //! Pointer to the data structure within the model containing the port maps.
    QList<QSharedPointer<PortMap> >& portMaps_;

    //! Pointer to the component being edited.
    QSharedPointer<Component> component_;

    //! Pointer to the instance that manages the library.
    LibraryInterface* handler_;

    //! Pointer to the abstraction definition that is used.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! Boolean for adding and removing rows.
    bool canEdit_;    

};

#endif // BITMAPPINGMODEL_H
