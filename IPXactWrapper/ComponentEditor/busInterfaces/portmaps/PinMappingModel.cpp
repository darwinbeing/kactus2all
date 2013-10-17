//-----------------------------------------------------------------------------
// File: PinMappingModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.09.2013
//
// Description:
// Table model for bit field mapping.
//-----------------------------------------------------------------------------

#include "PinMappingModel.h"

#include <QStringList>
#include <QColor>
#include <QIcon>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionEndpoint.h>
#include <designwidget/HWComponentItem.h>
#include <designwidget/models/PortGenerationRow.h>
#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/component.h>
#include <models/generaldeclarations.h>
#include "models/port.h"
#include <models/portabstraction.h>

#include <LibraryManager/libraryinterface.h>
#include "LibraryManager/vlnv.h"
#include <IPXactWrapper/ComponentEditor/busInterfaces/portmaps/BitSelectionDialog.h>

namespace
{
    QString const HEADER_NAMES[] =
    {
        "Logical\nIndex",
        "Bit Field(s)"
    }; 

    QString const EDIT_ROW_NAME = "Drop port to add rows.";
}

Q_DECLARE_METATYPE(General::PortBounds)

    //-----------------------------------------------------------------------------
    // Function: PinMappingModel()
    //-----------------------------------------------------------------------------
    PinMappingModel::PinMappingModel(BusInterface* busif,
    QSharedPointer<Component> component,
    LibraryInterface* libHandler, 
    QObject* parent)
    : QAbstractTableModel(parent), 
    busif_(busif),
    mode_(General::INTERFACE_MODE_COUNT),
    logicalPort_(),
    portMaps_(busif->getPortMaps()),
    component_(component),
    handler_(libHandler),        
    rows_(),
    mappings_(),
    canEdit_(false)
{
    Q_ASSERT(busif);
    Q_ASSERT(component);
    Q_ASSERT(libHandler);
}

//-----------------------------------------------------------------------------
// Function: ~PinMappingModel()
//-----------------------------------------------------------------------------
PinMappingModel::~PinMappingModel()
{
}

//-----------------------------------------------------------------------------
// Function: rowCount()
//-----------------------------------------------------------------------------
int PinMappingModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return rows_.size();
}

//-----------------------------------------------------------------------------
// Function: columnCount()
//-----------------------------------------------------------------------------
int PinMappingModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (parent.isValid()) {
        return 0;
    }

    return COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: data()
//-----------------------------------------------------------------------------
QVariant PinMappingModel::data(const QModelIndex& index, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( !index.isValid() )
    {
        return QVariant();
    }

    if ( index.row() > rows_.size() || index.column() >= COLUMN_COUNT )
    {
        return QVariant();
    }

    if ( role == Qt::DisplayRole ) 
    {                  

        switch (index.column()) {
        case INDEX :
            {
                return index.row();
            }
        case PIN : 
            {                
                if (rows_.at(index.row()).isEmpty())
                {
                    return logicalPort_;
                }
                else
                {

                    QStringList ports;
                    foreach (General::PortBounds row, rows_.at(index.row()))
                    {
                        int left = component_->getPortLeftBound(row.portName_);
                        int right = component_->getPortRightBound(row.portName_);
                        int portSize = abs(left - right) + 1;
                        
                        if (portSize > 1)
                        {
                            ports.append(row.portName_ + "(" + QString::number(row.left_) + ")");
                        }
                        else
                        {
                            ports.append(row.portName_);
                        }                        
                    }
                    return ports.join(", ");
                }
            }
        default :
            {
                return QVariant();
            }
        }
    }

    else if ( role == Qt::ForegroundRole )
    {      
        if ((index.column() == INDEX && !canEdit_) ||
            (canEdit_ && index.row()  == rows_.size() - 1) ||          
            (index.column() == PIN && rows_.at(index.row()).isEmpty()))
        {
            return QColor("gray");
        }  
        return QColor("black");
    }

    else if ( role == Qt::BackgroundRole )
    {
        return QColor("white");
    }

    else if (role == Qt::TextAlignmentRole)
    {
        return (Qt::AlignLeft + Qt::AlignVCenter);     
    }

    else if ( role == Qt::EditRole )
    {
        if (index.column() == PIN)
        {
            return data(index, Qt::DisplayRole);
        }
    }

    else if ( role == Qt::ToolTipRole )
    {
        // TODO
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: headerData()
//-----------------------------------------------------------------------------
QVariant PinMappingModel::headerData(int section, Qt::Orientation orientation, 
    int role /*= Qt::DisplayRole*/ ) const
{
    if ( orientation != Qt::Horizontal )
    {
        return QVariant();  
    }

    if ( role == Qt::DisplayRole ) 
    {
        if ( section < COLUMN_COUNT )
        {
            return HEADER_NAMES[section];
        }               
    }

    // if unsupported role
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: setData()
//-----------------------------------------------------------------------------
bool PinMappingModel::setData(const QModelIndex& index, const QVariant& value, 
    int role /*Qt::EditRole*/ )
{
    if (!index.isValid())
    {
        return false;
    }
    // if row is invalid
    else if ( index.row() < 0 || rows_.size() < index.row() ){
        return false;
    }

    else if ( index.column() != PIN )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        switch (index.column()) 
        {

        case PIN :
            {                
                if(value.canConvert<General::PortBounds>())
                {
                    General::PortBounds data = value.value<General::PortBounds>();
                    int targetRow = index.row();
                    int lowerBound = qMin(data.left_, data.right_);
                    int higherBound = qMax(data.left_, data.right_);

                    for (int i = lowerBound; i <= higherBound; i++)
                    {
                        if (targetRow > rows_.size())
                        {
                            break;
                        }

                        General::PortBounds conn(data.portName_, i, i);
                        if (!hasDuplicates(conn, targetRow))
                        {
                            rows_[targetRow].append(conn);
                        }
                        targetRow++;
                    }
                    emit dataChanged(index,index);
                    return true;
                }
                // Clear cell.
                else if (value.toString().isEmpty() && !(canEdit_ && index.row() == rows_.size() - 1))
                {
                    rows_[index.row()].clear();       
                    emit dataChanged(index,index);
                    return true;
                }

                return false;

            }        
        default :
            {
                return false;
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: setHeaderData()
//-----------------------------------------------------------------------------
bool PinMappingModel::setHeaderData( int section, Qt::Orientation orientation, 
    const QVariant & value, int role /*= Qt::EditRole*/ )
{
    if( orientation != Qt::Horizontal )
    {
        return false;
    }

    if ( !value.isValid() )
    {
        return false;
    }

    if( role == Qt::EditRole && section < COLUMN_COUNT )
    {
        QAbstractTableModel::setHeaderData(section,orientation, value, role);
        emit headerDataChanged(orientation,section,section);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PinMappingModel::flags(const QModelIndex& index) const
{
    if ( !index.isValid() )
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == PIN)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }

    return Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool PinMappingModel::isValid() const
{
    return true;
}


//-----------------------------------------------------------------------------
// Function: PinMappingModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions PinMappingModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


//-----------------------------------------------------------------------------
// Function: PinMappingModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList PinMappingModel::mimeTypes() const
{
    QStringList types = QAbstractTableModel::mimeTypes();
    types << "text/plain";
    return types;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::dropMimeData()
//-----------------------------------------------------------------------------
bool PinMappingModel::dropMimeData(const QMimeData *data, 
    Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if ( action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if ( row != -1 || column != -1 || !parent.isValid() )
    {
        return false;
    }

    if (data->hasText())
    {
        QStringList portNames = data->text().split(";", QString::SkipEmptyParts);
        // Check direction compatibility.
        if (portNames.isEmpty() || !checkDirectionForPorts(portNames))
        {
            return false;
        }       
        QList<QVariant> dropData;

        int targetRow = parent.row(); 

        if (canEdit_)
        {
            removeEditRow();
        }

        foreach (QString portName, portNames)
        {
            if (!component_->hasPort(portName))
            {
                continue;
            }

            if (targetRow >= rowCount() && !canEdit_)
            {
                break;
            }

            int left = component_->getPort(portName)->getLeftBound();
            int right = component_->getPort(portName)->getRightBound();
            int portSize = abs(left - right) + 1;
            int rowsLeft = rowCount() - targetRow;

            if (portSize > rowsLeft)
            {
                if (canEdit_)
                {
                    addRows(portSize - rowsLeft);
                } 
                else
                {
                    BitSelectionDialog dialog(portSize, rowsLeft);
                    if (dialog.exec() != QDialog::Accepted)
                    {
                        return false;
                    }
                    left = dialog.getLeft();
                    right = dialog.getRight();
                }            
            } 
            General::PortBounds portData(portName, left, right);

            setData(index(targetRow, PIN), QVariant::fromValue(portData));
            targetRow += abs(left - right) + 1;
        }

        if (canEdit_)
        {
            addEditRow();
        }

        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::onSetLogicalSignal()
//-----------------------------------------------------------------------------
void PinMappingModel::onSetLogicalSignal(QString const& logicalName)
{
    if (!logicalPort_.isEmpty())
    {
        mappings_.insert(logicalPort_, rows_);
    }

    logicalPort_ = logicalName;
    // Reset the table.
    beginResetModel();
    rows_.clear();

    if (!logicalName.isEmpty())
    {
        int logicalPortSize = absDef_->getPortSize(logicalName, mode_);

        // Indexes can be added/removed, if absDef does not define size.
        canEdit_ = (logicalPortSize == -1);

        if (mappings_.contains(logicalName))
        {
            rows_ = mappings_.value(logicalName);
        } 
        else
        {
            // If logical width is not defined in the abs def, find the highest bound in port maps for width.
            if (logicalPortSize == -1)
            {
                foreach (QSharedPointer<General::PortMap> portMap, portMaps_)
                {   
                    if (portMap->logicalPort_ == logicalName)
                    {
                        int higherBound = qMax(portMap->logicalVector_->getLeft(), portMap->logicalVector_->getRight());
                        logicalPortSize = qMax(logicalPortSize, higherBound);
                    }
                }
                logicalPortSize++;
            }

            for (int index = 0; index < logicalPortSize; index++)
            {
                QList<General::PortBounds> pins;
                // Search for previous mappings.
                foreach(QSharedPointer<General::PortMap> portMap, portMaps_)
                {
                    if (portMap->logicalPort_ == logicalName)
                    {
                        int logLeft = portMap->logicalVector_->getLeft();
                        int logRight = portMap->logicalVector_->getRight();
                        int logLower = qMin(logLeft, logRight);
                        int logHigher = qMax(logLeft, logRight);
                        if (logLower <= index && index <= logHigher)
                        {
                            int physLeft = component_->getPortLeftBound(portMap->physicalPort_);               
                            int physRight = component_->getPortRightBound(portMap->physicalPort_);
                            int physLower = qMin(physLeft, physRight);
                            General::PortBounds toAdd(portMap->physicalPort_);     
                            if (abs(physLeft - physRight) + 1 > 1)
                            {
                                int physIndex = index - (logLower - physLower);
                                toAdd.left_ = physIndex;
                                toAdd.right_ = physIndex;
                            }                    
                            pins.append(toAdd);
                        }
                    }
                }                
                rows_.append(pins);
            }

            if (canEdit_)
            {
                addEditRow();
            }
        }
    }
    endResetModel();

    emit canModify(canEdit_);
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::onRemoveMapping()
//-----------------------------------------------------------------------------
void PinMappingModel::onRemoveMapping(QString const& logicalName)
{
    bool setPrevious = logicalPort_ == logicalName;
    if (setPrevious)
    {
        onSetLogicalSignal("");
    }

    if (mappings_.contains(logicalName))
    {
        mappings_.remove(logicalName);
    }

    if (setPrevious)
    {
        onSetLogicalSignal(logicalName);
    }

}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::setAbsType()
//-----------------------------------------------------------------------------
void PinMappingModel::setAbsType(const VLNV& absDefVlnv, General::InterfaceMode mode)
{
    if (!absDefVlnv.isValid())
    {
        return;
    }

    // ask library to parse the model for abstraction definition
    QSharedPointer<LibraryComponent> libComb;
    if (handler_->contains(absDefVlnv))
    { 
        libComb = handler_->getModel(absDefVlnv);
    }
    // if library did not contain the vlnv
    else
    {
        return;
    }
    // make sure the model is for abstraction definition
    if (handler_->getDocumentType(absDefVlnv) == VLNV::ABSTRACTIONDEFINITION)
    {
        absDef_ = libComb.staticCast<AbstractionDefinition>();
    }

    mode_ = mode;

    // Clear the table.
    beginResetModel();
    rows_.clear();
    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::getLogicalSignal()
//-----------------------------------------------------------------------------
QString PinMappingModel::getLogicalSignal() const
{
    return logicalPort_;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::getPortMaps()
//-----------------------------------------------------------------------------
QList< QSharedPointer<General::PortMap> > PinMappingModel::getPortMaps() const
{                    
    QList<QSharedPointer<General::PortMap> > mappings;

    // Omit editing row, if exists.
    int endIndex = rows_.size();
    if (canEdit_)
    {
        endIndex--;
    }

    for (int logIndex = 0; logIndex < endIndex; logIndex++)
    {
        foreach(General::PortBounds pin, rows_[logIndex])
        {           
            // Check if physical port is already mapped.
            bool alreadyMapped = false;            
            foreach (QSharedPointer<General::PortMap> portMap, mappings)
            {
                int logLowerBound = qMin(portMap->logicalVector_->getLeft(),portMap->logicalVector_->getRight());
                int logHigherBound = qMax(portMap->logicalVector_->getLeft(),portMap->logicalVector_->getRight());
                int phyLowerBound = qMin(portMap->physicalVector_->getLeft(),portMap->physicalVector_->getRight());
                int phyHigherBound = qMax(portMap->physicalVector_->getLeft(),portMap->physicalVector_->getRight());

                if (portMap->physicalPort_ == pin.portName_ &&
                    logLowerBound <= logIndex && logIndex <= logHigherBound &&
                    phyLowerBound <= pin.left_ && pin.left_ <= phyHigherBound )
                {
                    alreadyMapped = true;
                    break;
                }
            }

            if (!alreadyMapped) 
            {               
                // Create a new port map.      
                int logLeft = logIndex;
                int logRight = logIndex;
                int physLeft = pin.left_;
                int physRight = pin.left_;

                bool ascending = true;
                if (component_->getPortLeftBound(pin.portName_) < component_->getPortRightBound(pin.portName_))
                {
                    ascending = false;
                }

                // Check how many contiguous bits are connected  in ascending order.
                if (logIndex != rows_.size() - 1)
                {                    
                    bool stretch = true;
                    for (int index = logIndex + 1; index < rows_.size() && stretch; index++)
                    {
                        stretch = false;
                        foreach (General::PortBounds conn, rows_[index])
                        {
                            if (conn == pin )
                            { 
                                if (ascending && conn.left_ == physLeft + 1)                                
                                {
                                    physLeft++;
                                    logLeft++;
                                    stretch = true;
                                    break;
                                }
                                else if (!ascending && conn.left_ == physRight + 1)
                                {
                                    physRight++;
                                    logRight++;
                                    stretch = true;
                                    break;
                                }
                            }
                        }                   
                    } 
                }               

                QSharedPointer<General::PortMap> map(new General::PortMap());
                map->logicalPort_ = logicalPort_;
                map->logicalVector_->setLeft(logLeft);
                map->logicalVector_->setRight(logRight);
                map->physicalPort_ = pin.portName_;
                map->physicalVector_->setLeft(physLeft);
                map->physicalVector_->setRight(physRight);
                mappings.append(map);     
            }                               
        }
    }

    return mappings;
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::addRows()
//-----------------------------------------------------------------------------
void PinMappingModel::addRows(int count)
{
    int lastRow = rows_.size() + count - 1;

    beginInsertRows(QModelIndex(), rows_.size(), lastRow);
    for (int row = rows_.size(); row <= lastRow; row++)
    {
        rows_.append(QList<General::PortBounds>());
    }
    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::removeRows()
//-----------------------------------------------------------------------------
void PinMappingModel::removeRows(int count)
{
    int lastRemovedRow = rows_.size() - count;

    beginRemoveRows(QModelIndex(), lastRemovedRow, rows_.size() - 1);
    for (int row = rows_.size() - 1; row >= lastRemovedRow; row--)
    {
        rows_.removeLast();
    }
    endRemoveRows();
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::addEditRow()
//-----------------------------------------------------------------------------
void PinMappingModel::addEditRow()
{
    if (canEdit_){
        QList<General::PortBounds> pins;
        General::PortBounds toAdd(EDIT_ROW_NAME);              
        pins.append(toAdd);
        addRows(1);
        rows_.last().append(pins);
    }
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::removeEditRow()
//-----------------------------------------------------------------------------
void PinMappingModel::removeEditRow()
{
    if (canEdit_)
    {
        const QList<General::PortBounds> lastConnection = rows_.value(rows_.size() - 1);
        if (lastConnection.size() != 0 && 
            lastConnection.first().portName_ == EDIT_ROW_NAME)
        {
            removeRows(1);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PinMappingModel::hasDuplicates()
//-----------------------------------------------------------------------------
bool PinMappingModel::hasDuplicates(General::PortBounds connection, int row) const
{
    foreach (General::PortBounds pins, rows_.value(row))
    {
        if ( pins.left_ == connection.left_ && pins == connection)
        {
            return true;
        }
    }

    return false;
}



//-----------------------------------------------------------------------------
// Function: PinMappingModel::checkDirectionForPorts()
//-----------------------------------------------------------------------------
bool PinMappingModel::checkDirectionForPorts(QStringList const& ports)
{
    QStringList incompatiblePorts;
    foreach (QString portName, ports)
    {
        if (component_->getPortDirection(portName) != General::INOUT &&
            absDef_->getPortDirection(logicalPort_, mode_) != General::INOUT &&
            component_->getPortDirection(portName) != absDef_->getPortDirection(logicalPort_, mode_))
        {
            incompatiblePorts.append(portName);
        }
    }

    if (!incompatiblePorts.isEmpty())
    {
        emit errorMessage("Directions between logical port \"" + logicalPort_ + 
            "\" and physical port(s) " + incompatiblePorts.join(", ") + " did not match.");
        return false;
    }

    return true;
}

