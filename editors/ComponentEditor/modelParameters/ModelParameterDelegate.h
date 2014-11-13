//-----------------------------------------------------------------------------
// File: ModelParameterDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------

#ifndef MODELPARAMETERDELEGATE_H
#define MODELPARAMETERDELEGATE_H

#include <QStyledItemDelegate>

#include <editors/ComponentEditor/parameters/ParameterDelegate.h>

class Choice;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing model parameters.
//-----------------------------------------------------------------------------
class ModelParameterDelegate : public ParameterDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *     @param [in] choices  The choices available for model parameter value.
	 *     @param [in] parent   The parent object
	*/
	ModelParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject *parent = 0);

	//! The destructor
	virtual ~ModelParameterDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

protected:

    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;
    
    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int formatColumn() const;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const;

private:

	//! No copying
	ModelParameterDelegate(const ModelParameterDelegate& other);

	//! No assignment
	ModelParameterDelegate& operator=(const ModelParameterDelegate& other);


};

#endif // MODELPARAMETERDELEGATE_H
