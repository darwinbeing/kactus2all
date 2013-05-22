

#include "vhdltoipxact.h"
#include "models/generaldeclarations.h"
#include "models/component.h"
#include "models/port.h"
#include "models/model.h"
#include "models/modelparameter.h"
#include "models/fileset.h"
#include "models/file.h"
#include <QSharedPointer>
#include <QTableWidget>
#include <QHeaderView>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QMenu>

VHDLtoIPXACT::VHDLtoIPXACT(QSharedPointer<Component> comp, const QString xmlFilePath,
             QWidget *parent)
    : QDialog(parent)
{

    xmlFile = xmlFilePath;
	comp_ = comp;
    VHDLOpen = false;
    saveState = true;
    VHDdata = new VHDLanalysis;
    entityEndRow = 0;

    //starting size
    this->resize(1000, 750);

    //grid
    gridLayout = new QGridLayout;
    this->setLayout(gridLayout);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(11, 11, 11, 11);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    //vhdl display
    VHDLdisplay = new textEdit(this);
    VHDLdisplay->setObjectName(QStringLiteral("VHDLdisplay"));
    VHDLdisplay->setMaximumSize(QSize(750, 16777215));
    VHDLdisplay->setMouseTracking(true);
    VHDLdisplay->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    VHDLdisplay->setReadOnly(true);
    VHDLdisplay->setTextInteractionFlags(Qt::TextBrowserInteraction);

    gridLayout->addWidget(VHDLdisplay, 1, 0, 1, 1);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(6);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

    //table for ports
    tableWidgetPorts = new QTableWidget(this);
    tableWidgetPorts->setColumnCount(9);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(3, __qtablewidgetitem3);
    QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(4, __qtablewidgetitem4);
    QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(5, __qtablewidgetitem5);
    QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(6, __qtablewidgetitem6);
    QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(7, __qtablewidgetitem7);
    QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
    tableWidgetPorts->setHorizontalHeaderItem(8, __qtablewidgetitem8);
    tableWidgetPorts->setRowCount(1);
    tableWidgetPorts->setObjectName(QStringLiteral("tableWidgetPorts"));
    tableWidgetPorts->setContextMenuPolicy(Qt::CustomContextMenu);
    tableWidgetPorts->setEditTriggers(QAbstractItemView::AnyKeyPressed|QAbstractItemView::DoubleClicked
                                      |QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
    tableWidgetPorts->setDragDropMode(QAbstractItemView::DropOnly);
    tableWidgetPorts->setDefaultDropAction(Qt::CopyAction);
    tableWidgetPorts->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidgetPorts->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidgetPorts->setRowCount(1);
    tableWidgetPorts->verticalHeader()->setVisible(false);

    verticalLayout->addWidget(tableWidgetPorts);

    label_3 = new QLabel("label_3");
    //label_3->setObjectName(QStringLiteral("label_3"));
    label_3->setTextInteractionFlags(Qt::NoTextInteraction);

    verticalLayout->addWidget(label_3);

    //param table
    tableWidgetModelParams = new QTableWidget(this);
    tableWidgetModelParams->setColumnCount(5);
    QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
    tableWidgetModelParams->setHorizontalHeaderItem(0, __qtablewidgetitem9);
    QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
    tableWidgetModelParams->setHorizontalHeaderItem(1, __qtablewidgetitem10);
    QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
    tableWidgetModelParams->setHorizontalHeaderItem(2, __qtablewidgetitem11);
    QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
    tableWidgetModelParams->setHorizontalHeaderItem(3, __qtablewidgetitem12);
    QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
    tableWidgetModelParams->setHorizontalHeaderItem(4, __qtablewidgetitem13);
    if (tableWidgetModelParams->rowCount() < 1)
        tableWidgetModelParams->setRowCount(1);
    tableWidgetModelParams->setObjectName(QStringLiteral("tableWidgetModelParams"));
    tableWidgetModelParams->setContextMenuPolicy(Qt::CustomContextMenu);
    tableWidgetModelParams->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    tableWidgetModelParams->setRowCount(1);
    tableWidgetModelParams->verticalHeader()->setVisible(false);

    verticalLayout->addWidget(tableWidgetModelParams);

    label_4 = new QLabel(this);
    label_4->setObjectName(QStringLiteral("label_4"));
    label_4->setTextInteractionFlags(Qt::NoTextInteraction);

    verticalLayout->addWidget(label_4);

    tableWidgetFileSets = new QTableWidget(this);
    tableWidgetFileSets->setObjectName(QStringLiteral("tableWidgetFileSets"));
    tableWidgetFileSets->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    verticalLayout->addWidget(tableWidgetFileSets);


    gridLayout->addLayout(verticalLayout, 1, 1, 1, 1);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
    pushButtonOpenFile = new QPushButton(this);
    pushButtonOpenFile->setObjectName(QStringLiteral("pushButtonOpenFile"));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pushButtonOpenFile->sizePolicy().hasHeightForWidth());
    pushButtonOpenFile->setSizePolicy(sizePolicy);

    horizontalLayout->addWidget(pushButtonOpenFile);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    pushButtonDependency = new QPushButton(this);
    pushButtonDependency->setObjectName(QStringLiteral("pushButtonDependency"));

    horizontalLayout->addWidget(pushButtonDependency);

    pushButtonSaveFile = new QPushButton(this);
    pushButtonSaveFile->setObjectName(QStringLiteral("pushButtonSaveFile"));

    horizontalLayout->addWidget(pushButtonSaveFile);


    gridLayout->addLayout(horizontalLayout, 2, 0, 1, 2);

    label = new QLabel(this);
    label->setObjectName(QStringLiteral("label"));
    label->setTextFormat(Qt::AutoText);
    label->setTextInteractionFlags(Qt::NoTextInteraction);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    label_2 = new QLabel(this);
    label_2->setObjectName(QStringLiteral("label_2"));
    label_2->setTextInteractionFlags(Qt::NoTextInteraction);

    gridLayout->addWidget(label_2, 0, 1, 1, 1);

    this->setWindowTitle(QApplication::translate("dataConverter", "VHDL to IP-XACT Converter", 0));
    QTableWidgetItem *___qtablewidgetitem = tableWidgetPorts->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QApplication::translate("dataConverter", "Name", 0));
    QTableWidgetItem *___qtablewidgetitem1 = tableWidgetPorts->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QApplication::translate("dataConverter", "Direction", 0));
    QTableWidgetItem *___qtablewidgetitem2 = tableWidgetPorts->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QApplication::translate("dataConverter", "Width", 0));
    QTableWidgetItem *___qtablewidgetitem3 = tableWidgetPorts->horizontalHeaderItem(3);
    ___qtablewidgetitem3->setText(QApplication::translate("dataConverter", "Left Bound", 0));
    QTableWidgetItem *___qtablewidgetitem4 = tableWidgetPorts->horizontalHeaderItem(4);
    ___qtablewidgetitem4->setText(QApplication::translate("dataConverter", "Right Bound", 0));
    QTableWidgetItem *___qtablewidgetitem5 = tableWidgetPorts->horizontalHeaderItem(5);
    ___qtablewidgetitem5->setText(QApplication::translate("dataConverter", "Type", 0));
    QTableWidgetItem *___qtablewidgetitem6 = tableWidgetPorts->horizontalHeaderItem(6);
    ___qtablewidgetitem6->setText(QApplication::translate("dataConverter", "Type Definition", 0));
    QTableWidgetItem *___qtablewidgetitem7 = tableWidgetPorts->horizontalHeaderItem(7);
    ___qtablewidgetitem7->setText(QApplication::translate("dataConverter", "Default Value", 0));
    QTableWidgetItem *___qtablewidgetitem8 = tableWidgetPorts->horizontalHeaderItem(8);
    ___qtablewidgetitem8->setText(QApplication::translate("dataConverter", "Description", 0));
    label_3->setText(QApplication::translate("dataConverter", "IP-XACT Model Parameters", 0));
    QTableWidgetItem *___qtablewidgetitem9 = tableWidgetModelParams->horizontalHeaderItem(0);
    ___qtablewidgetitem9->setText(QApplication::translate("dataConverter", "Name", 0));
    QTableWidgetItem *___qtablewidgetitem10 = tableWidgetModelParams->horizontalHeaderItem(1);
    ___qtablewidgetitem10->setText(QApplication::translate("dataConverter", "Data Type", 0));
    QTableWidgetItem *___qtablewidgetitem11 = tableWidgetModelParams->horizontalHeaderItem(2);
    ___qtablewidgetitem11->setText(QApplication::translate("dataConverter", "Usage Type", 0));
    QTableWidgetItem *___qtablewidgetitem12 = tableWidgetModelParams->horizontalHeaderItem(3);
    ___qtablewidgetitem12->setText(QApplication::translate("dataConverter", "Value", 0));
    QTableWidgetItem *___qtablewidgetitem13 = tableWidgetModelParams->horizontalHeaderItem(4);
    ___qtablewidgetitem13->setText(QApplication::translate("dataConverter", "Description", 0));
    label_4->setText(QApplication::translate("dataConverter", "IP-XACT File Sets", 0));
    pushButtonOpenFile->setText(QApplication::translate("dataConverter", "Open VHDL File", 0));
    pushButtonDependency->setText(QApplication::translate("dataConverter", "Add Dependent Files", 0));
    pushButtonSaveFile->setText(QApplication::translate("dataConverter", "Create IP-XACT", 0));
    label->setText(QApplication::translate("dataConverter", "Source VHDL", 0));
    label_2->setText(QApplication::translate("dataConverter", "IP-XACT Ports", 0));


    connect( VHDLdisplay, SIGNAL( selectedWord( const QString & )), this,
             SLOT( updateTable( const QString &) ) );


    QMetaObject::connectSlotsByName(this);
}

VHDLtoIPXACT::~VHDLtoIPXACT()
{
    
}

//dialog to make sure user wants to close the tool
void VHDLtoIPXACT::closeEvent(QCloseEvent* event) {

    if (saveState == false){

    QMessageBox::StandardButton msg;
    msg = QMessageBox::question(this, "Warning", "IP-XACT has not been saved. Close anyway?"
                                , QMessageBox::Yes | QMessageBox::Cancel);
    if (msg == QMessageBox::Yes) {
        VHDLdisplay->clear();
        //VHDLdisplay->connect(VHDLdisplay, SIGNAL(doubleClicked(int, int)), SLOT(TextEditDoubleClicked(int,int)));
        event->accept();
        return;
    }
    else if( msg == QMessageBox::Cancel ){
        event->ignore();
        return;
    }
    }

}

void VHDLtoIPXACT::updateTable( const QString &worduc )
{
    int params = tableWidgetModelParams->rowCount();
    int ports = tableWidgetPorts->rowCount();

    QTextCursor cursors = VHDLdisplay->textCursor();
    VHDLdisplay->setReadOnly( false);

    cursors.select(QTextCursor::LineUnderCursor);
    if (entityEndRow < cursors.blockNumber() ) return;
    VHDdata->EntityConverter( *tableWidgetModelParams, *tableWidgetPorts, worduc, true);

    if (params < tableWidgetModelParams->rowCount() ) {
        cursors.beginEditBlock();
        QTextCharFormat plainFormat(cursors.charFormat());
        plainFormat.setForeground(Qt::cyan);
        cursors.insertText(cursors.selectedText(),plainFormat);
        plainFormat.setForeground(Qt::black);
        cursors.endEditBlock();
    }
    else if (params > tableWidgetModelParams->rowCount() || ports > tableWidgetPorts->rowCount() ){
        cursors.beginEditBlock();
        QTextCharFormat plainFormat(cursors.charFormat());
        plainFormat.setForeground(Qt::red);
        cursors.insertText(cursors.selectedText(),plainFormat);
        plainFormat.setForeground(Qt::black);
        cursors.endEditBlock();
    }
    else if (ports < tableWidgetPorts->rowCount() ){
        cursors.beginEditBlock();
        QTextCharFormat plainFormat(cursors.charFormat());
        plainFormat.setForeground(Qt::blue);
        cursors.insertText(cursors.selectedText(),plainFormat);
        plainFormat.setForeground(Qt::black);
        cursors.endEditBlock();
    }
    VHDLdisplay->setReadOnly( true );

}

//open vhdl file for analysis and clear previous file.
void VHDLtoIPXACT::on_pushButtonOpenFile_released()
{
    if ( VHDLOpen == false ) {
        VHDdata = new VHDLanalysis;
        QString filename = QFileDialog::getOpenFileName(this,tr("Open file"), "", tr("VHDL files (*.vhd *.vhdl)"));
        QFile data(filename);
        if(data.open(QIODevice::ReadOnly | QIODevice::Text)){
            vhdlFile = filename;
            QTextStream in(&data);
            VHDLdisplay->clear();
            QString line;
            label->setText("Source VHDL - " + filename);
            int params = tableWidgetModelParams->rowCount();
            int ports = tableWidgetPorts->rowCount();
            int linereturn = 0;

            while (!in.atEnd()){
                line = in.readLine();
                if ( linereturn != 4) {
                    linereturn = VHDdata->EntityConverter( *tableWidgetModelParams, *tableWidgetPorts, line, false);
                    if ( linereturn == 4 ) entityEndRow = VHDLdisplay->textCursor().blockNumber();
                }
                if (tableWidgetPorts->rowCount() != ports  ) {
                    VHDLdisplay->setTextColor(Qt::blue);
                    VHDLdisplay->insertPlainText(line+"\n");
                    ports = tableWidgetPorts->rowCount();
                    VHDLdisplay->setTextColor(Qt::black);
                }
                else if ( tableWidgetModelParams->rowCount() != params ){
                    VHDLdisplay->setTextColor(Qt::cyan);
                    VHDLdisplay->insertPlainText(line+"\n");
                    params = tableWidgetModelParams->rowCount();
                    VHDLdisplay->setTextColor(Qt::black);
                }
                else {
                    VHDLdisplay->setTextColor(Qt::black);
                    VHDLdisplay->insertPlainText(line+"\n");
                }

            }
        }
        else {
            QMessageBox msgBox;
            msgBox.setInformativeText("Failed to open file");
            msgBox.exec();
        }
        VHDLOpen = true;
        pushButtonOpenFile->setText("Close VHDL File");
        data.close();
        saveState = false;


    }
    else if( VHDLOpen == true) {
        QMessageBox::StandardButton msg;
        msg = QMessageBox::question(this, "Warning", "Closing the file discards all information on the tables. Do you want to proceed?"
                                    , QMessageBox::Yes | QMessageBox::Cancel);
        if (msg == QMessageBox::Yes) {
            VHDLdisplay->clear();
            //Tyhjennä taulut ja tietorakenne
            VHDLOpen = false;
            pushButtonOpenFile->setText("Open VHDL File");
            label->setText("Source VHDL");
            saveState = true;
            tableWidgetModelParams->clearContents();
            while (  tableWidgetModelParams->rowCount() > 1 ){
                tableWidgetModelParams->removeRow(0);
            }
            tableWidgetPorts->clearContents();
            while (  tableWidgetPorts->rowCount() > 1 ){
                tableWidgetPorts->removeRow(0);
            }
            delete VHDdata;
            VHDLOpen = false;
        }
        else {
            //Do nothing
        }

    }
}

//file dependencies call
void VHDLtoIPXACT::on_pushButtonDependency_clicked()
{
    QMessageBox msgBox;
    msgBox.setInformativeText("Not yet implemented!");
    msgBox.exec();
}


void VHDLtoIPXACT::on_pushButtonSaveFile_released()
{
    accept();
    return;

    //todo: change filename to xmlFile
//    VLNV testi;
//    testi.setName("name");

    QMessageBox msgBox;
    /*
    if ( !comp_->getVlnv()->isValid() ) {
        msgBox.setInformativeText("VLNV data not valid!");
        msgBox.exec();
        return;
    } */
     /*comp_->getVlnv()->getName()+ "." + comp_->getVlnv()->getVersion() + ".xml"*/
    QString nametag =  "testi.xml";
    /*
    //start file writing
    //QString filename = QFileDialog::getSaveFileName(this,tr("Open file"), "", tr("XML files (*.xml)"));
    QFile xmlData(xmlFile + "\\" + nametag);
    if (xmlData.open(QIODevice::WriteOnly)) {

		//test code for kactus komponent use
        
        //create model
        Model* model = new Model;


        //create port list
		
        QList<QSharedPointer<Port>> ports;
		
		//create modelParam list
		
        QList<QSharedPointer<ModelParameter>> modelParameters;
		
		
        //loop ports into model
        General::Direction direction;
        bool ok = true;
		
        for ( int i = 0 ; i < tableWidgetPorts->rowCount() ; ++i){
            if ( !tableWidgetPorts->itemAt(i,0)->text().isEmpty() ) {

                if ( !tableWidgetPorts->itemAt(i,1)->text().compare("in",Qt::CaseInsensitive )) {
					direction = General::INOUT ;
                }
                else if ( !tableWidgetPorts->itemAt(i,1)->text().compare("out",Qt::CaseInsensitive )) {
					direction = General::INOUT ;
                }
                else if ( !tableWidgetPorts->itemAt(i,1)->text().compare("inout",Qt::CaseInsensitive )) {
                    direction = General::INOUT ;
                }
                else {
                    direction = General::DIRECTION_INVALID;
                }

                QSharedPointer<Port> port = QSharedPointer<Port>(new Port);
                port->setName(tableWidgetPorts->itemAt(i,0)->text());
                port->setDirection(direction);
                port->setLeftBound(tableWidgetPorts->itemAt(i,3)->text().toInt(&ok));
                port->setRightBound(tableWidgetPorts->itemAt(i,4)->text().toInt(&ok));
                port->setTypeName(tableWidgetPorts->itemAt(i,5)->text());
                port->setTypeDefinition(tableWidgetPorts->itemAt(i,5)->text(), tableWidgetPorts->itemAt(i,6)->text() );
                port->setDefaultValue(tableWidgetPorts->itemAt(i,7)->text());
                port->setDescription(tableWidgetPorts->itemAt(i,8)->text());

                ports.append( port );

				
            }
        }

        msgBox.setInformativeText("params");
        msgBox.exec();
		
        //loop parameters into model
        for ( int i = 0 ; i < tableWidgetModelParams->rowCount() ; ++i){
            if ( !tableWidgetModelParams->itemAt(i,0)->text().isEmpty() ) {

                QSharedPointer<ModelParameter> param = QSharedPointer<ModelParameter>(new ModelParameter);

                param->setDataType(tableWidgetModelParams->itemAt(i,1)->text());
                param->setName(tableWidgetModelParams->itemAt(i,0)->text());
                param->setDescription(tableWidgetModelParams->itemAt(i,4)->text());
                param->setValue(tableWidgetModelParams->itemAt(i,3)->text());
                param->setUsageType(tableWidgetModelParams->itemAt(i,2)->text());
                modelParameters.append( param );

            }
        }
		

        model->setModelParameters( modelParameters ); //on model
        model->setPorts( ports ); //on model


        //add model into component

        msgBox.setInformativeText("model");
        msgBox.exec();

        //comp_->setModel( model );

        //add fileset

        msgBox.setInformativeText("fileset");
        msgBox.exec();
        */
        /*
        QSharedPointer<FileSet> fileSet(new FileSet(xmlFile));
        const QString filePath = General::getRelativePath( xmlFile, vhdlFile);

        QSharedPointer<File> file(new File());
        file->addFileType("vhdlsource");
        file->setName(filePath);

        msgBox.setInformativeText("fileset2");
        msgBox.exec();

        fileSet->addFile(file);
        comp_->addFileSet(  fileSet );
        */
        /*
        QStringList errorList;
        if ( !comp_->isValid(errorList)) {
            msgBox.setInformativeText("Failure to verify data fields!");
            msgBox.exec();
            return;
        } 
        msgBox.setInformativeText("write");
        msgBox.exec();
        comp_->write(xmlData);


        saveState = true;

        xmlData.close();
        */
}

void VHDLtoIPXACT::removeRow()
{
    tableWidgetPorts->removeRow(tableWidgetPorts->currentRow());
}
void VHDLtoIPXACT::addRow()
{
    if (tableWidgetPorts->currentRow() < 1 )
    {
        tableWidgetPorts->insertRow(0);
    }
    else
    {
        tableWidgetPorts->insertRow(tableWidgetPorts->currentRow());
    }

}
void VHDLtoIPXACT::removeRowG()
{
    tableWidgetModelParams->removeRow(tableWidgetModelParams->currentRow());
}
void VHDLtoIPXACT::addRowG()
{
    if (tableWidgetModelParams->currentRow() < 1 )
    {
        tableWidgetModelParams->insertRow(0);
    }
    else
    {
        tableWidgetModelParams->insertRow(tableWidgetModelParams->currentRow());
    }

}

void VHDLtoIPXACT::on_tableWidgetPorts_customContextMenuRequested()
{
    QMenu *context= new QMenu(this);
    QAction* add = new QAction("Add row",this);
    QAction* del = new QAction("Delete row",this);
    context->addAction(add);
    context->addAction(del);
    context->popup( QCursor::pos() );
    connect(del, SIGNAL(triggered()), this, SLOT(removeRow()));
    connect(add, SIGNAL(triggered()), this, SLOT(addRow()));

}

void VHDLtoIPXACT::on_tableWidgetModelParams_customContextMenuRequested()
{
    QMenu *context= new QMenu(this);
    QAction* add = new QAction("Add row",this);
    QAction* del = new QAction("Delete row",this);
    context->addAction(add);
    context->addAction(del);
    context->popup( QCursor::pos() );
    connect(del, SIGNAL(triggered()), this, SLOT(removeRowG()));
    connect(add, SIGNAL(triggered()), this, SLOT(addRowG()));
}

