/* 
 *
 *  Created on: 23.1.2011
 *      Author: Antti Kamppi
 */

#include "componenttreeitem.h"

#include "itemeditor.h"

#include <models/model.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/function.h>
#include <models/choice.h>
#include <models/modelparameter.h>
#include <models/parameter.h>
#include <models/memorymap.h>
#include <models/addressspace.h>
#include <models/remapstate.h>
#include <models/view.h>
#include <models/port.h>
#include <models/businterface.h>
#include <models/channel.h>
#include <models/cpu.h>
#include <models/otherclockdriver.h>
#include <models/componentgenerator.h>
#include <models/memorymapitem.h>
#include <models/addressblock.h>
#include <models/bank.h>
#include <models/subspacemap.h>
#include <models/generaldeclarations.h>
#include <models/register.h>
#include <models/ComProperty.h>
#include <models/ComInterface.h>
#include <models/ApiInterface.h>
#include <models/SWView.h>

#include <LibraryManager/libraryinterface.h>

#include <QSharedPointer>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QStringList>

#include <QDebug>

#include <typeinfo>

ComponentTreeItem::ComponentTreeItem(TreeItemType type, 
									 void* elementPointer,
									 QSharedPointer<Component> component,
									 LibraryInterface* handler,
									 ComponentTreeItem* parentItem):
QObject(parentItem), 
type_(type),
text_(), 
dataPointer_(elementPointer),
component_(component),
parent_(parentItem), 
childItems_(), 
handler_(handler),
editor_(NULL) {

    bool hwComp = component->getComponentImplementation() == KactusAttribute::KTS_HW;
    bool swComp = component->getComponentImplementation() == KactusAttribute::KTS_SW;

	// the type defines how the construction goes
	switch (type) {

		// if this is the root item
	case ComponentTreeItem::COMPONENT: {

		text_ = tr("Component");

		// create the child items for the root item

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::GENERAL, component.data(), component, handler, this));

        childItems_.append(new ComponentTreeItem(
		    ComponentTreeItem::FILESETS, 0, component, handler, this));

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::CHOICES, 0, component, this));

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::MODELPARAMETERS, 0, component, handler, this));

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::PARAMETERS, 0, component, handler, this));

		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::MEMORYMAPS, 0, component, handler, this));

        if (hwComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::ADDRESSSPACES, 0, component, handler, this));
        }

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::REMAPSTATES, 0, component, this));

        if (hwComp)
        {
		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::VIEWS, 0, component, handler, this));

		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::PORTS, 0, component, handler, this));

            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::BUSINTERFACES, 0, component, handler, this));

            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::CHANNELS, 0, component, handler, this));
        
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::CPUS, 0, component, handler, this));

		    childItems_.append(new ComponentTreeItem(
			    ComponentTreeItem::OTHERCLOCKDRIVERS, 0, component, handler, this));

            // HACK! We're using the vlnv as the data pointer to ensure uniqueness. Any better ideas?
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::SOFTWARE, component->getVlnv(), component, handler, this));
        }

        childItems_.append(new ComponentTreeItem(ComponentTreeItem::SWVIEWS, 0, component, handler, this));
        childItems_.append(new ComponentTreeItem(ComponentTreeItem::SWPROPERTIES, 0, component, handler, this));
        childItems_.append(new ComponentTreeItem(ComponentTreeItem::COMINTERFACES, 0, component, handler, this));

        if (swComp)
        {
            childItems_.append(new ComponentTreeItem(ComponentTreeItem::APIINTERFACES, 0, component, handler, this));
        }

// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::COMPONENTGENERATORS, 0, component, this));
		break;
									   }
	case ComponentTreeItem::GENERAL: {
		text_ = tr("General");
		break;
									 }
	case ComponentTreeItem::FILESETS: {
		text_ = tr("File sets");

		// get file sets from the component
		QList<QSharedPointer<FileSet> > list = component_->getFileSets();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::FILESET, list.at(i).data(),
				component, handler, this));
		}

		break;
									  }
	case ComponentTreeItem::FILESET: {

		// get pointer to the matching file set
		FileSet* fileSet = static_cast<FileSet*>(dataPointer_);
		
		// set the text that is shown in the editor
		text_ = fileSet->getName();

		// File set always has three children: Default file builders, Files and Functions
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::DEFAULTFILEBUILDERS, &fileSet->getDefaultFileBuilders(), 
			component, handler, this));
		childItems_.append(new ComponentTreeItem(
			ComponentTreeItem::FILES, &fileSet->getFiles(), component, handler, this));
// 		childItems_.append(new ComponentTreeItem(
// 			ComponentTreeItem::FUNCTIONS, 0, component, this));

		// get files from the fileSet and append them to this item
		QList<QSharedPointer<File> > fileList = fileSet->getFiles();
		for (int i = 0; i < fileList.size(); ++i) {

			// add child to "Files" child item
			childItems_.value(1)->addChild(new ComponentTreeItem(
				ComponentTreeItem::FILE, fileList.at(i).data(), component, handler, childItems_.at(1)));
		}

		// get the functions defined for the file set
// 		QList<QSharedPointer<Function> > functionList = fileSet->getFunctions();
// 		for (int i = 0; i < functionList.size(); ++i) {
// 			
// 			childItems_.value(2)->addChild(new ComponentTreeItem(
// 				ComponentTreeItem::FUNCTION, functionList.at(i).data(), component, childItems_.at(2)));
// 		}

		break;
									 }
	case ComponentTreeItem::FILES: {
		text_ = tr("Files");
		break;
								   }
	case ComponentTreeItem::FILE: {
		File* file = static_cast<File*>(dataPointer_);
		
		// set the text shown in the editor
		//QFileInfo fileInfo(file->getName());
		text_ = file->getName();
		break;
								  }
	case ComponentTreeItem::FUNCTIONS: {
		text_ = tr("Functions");
		break;
									   }
	case ComponentTreeItem::FUNCTION: {
		Function* functionP = static_cast<Function*>(dataPointer_);
		
		// if entry point has been defined
		if (!functionP->getEntryPoint().isEmpty()) {
			text_ = functionP->getEntryPoint();
		}
		// if no entry point is defined then use the only mandatory field in function
		else {
			text_ = functionP->getFileRef();
		}
									  }
	case ComponentTreeItem::DEFAULTFILEBUILDERS: {
		text_ = tr("Default file builders");
		break;
												 }
	case ComponentTreeItem::CHOICES: {
		text_ = tr("Choices");

		// get choices from the component
		QList<QSharedPointer<Choice> > list = component_->getChoices();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHOICE, list.at(i).data(), component, handler, this));
		}
		break;
									 }
	case ComponentTreeItem::CHOICE: {
		Choice* choice = static_cast<Choice*>(dataPointer_);

		// set the text shown in editor
		text_ = choice->getName();
		break;
									}

	case ComponentTreeItem::MODELPARAMETERS: {
        text_ = tr("Model parameters");
		QMap<QString, QSharedPointer<ModelParameter> >* modelParams = 
			component_->getModel()->getModelParametersPointer();
		dataPointer_ = modelParams;
		break;
											 }
	case ComponentTreeItem::PARAMETERS: {
		text_ = tr("Parameters");

		QList<QSharedPointer<Parameter> >* params = &component_->getParameters();
		dataPointer_ = params;
		break;
										}
	case ComponentTreeItem::MEMORYMAPS: {
		text_ = tr("Memory maps");

		QList<QSharedPointer<MemoryMap> >* list = component_->getMemoryMapsPointer();
		foreach (QSharedPointer<MemoryMap> memMap, *list) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::MEMORYMAP, memMap.data(), component_, handler_, this));
		}
		dataPointer_ = list;
		break;
										}
	case ComponentTreeItem::MEMORYMAP: {
		MemoryMap* memMap = static_cast<MemoryMap*>(dataPointer_);
		Q_ASSERT_X(memMap, "ComponentTreeItem constructor in case MEMORYMAP",
			"static_cast failed to give valid MemoryMap-pointer");

		text_ = memMap->getName();

		// the address blocks are created as children
		QList<QSharedPointer<MemoryMapItem> >* list = memMap->getItemsPointer();
		foreach (QSharedPointer<MemoryMapItem> item, *list) {
			QSharedPointer<AddressBlock> addrBlock = item.dynamicCast<AddressBlock>();

			if (addrBlock) {
				childItems_.append(new ComponentTreeItem(
					ComponentTreeItem::ADDRESSBLOCK, addrBlock.data(), component_, handler_, this));
			}
		}
		break;
									   }
	case ComponentTreeItem::ADDRESSBLOCK: {
		AddressBlock* addrBlock = static_cast<AddressBlock*>(dataPointer_);
		Q_ASSERT_X(addrBlock, "ComponentTreeItem constructor in case ADDRESSBLOCK",
			"static_cast failed to give valid AddressBlock-pointer");

		text_ = addrBlock->getName();

		// The registers are created as children
		QList<QSharedPointer<RegisterModel> >* list = addrBlock->getRegisterPointer();
		foreach (QSharedPointer<RegisterModel> registerM, *list) {

			QSharedPointer<Register> reg = registerM.dynamicCast<Register>();

			if (reg) {
				childItems_.append(new ComponentTreeItem(
					ComponentTreeItem::REGISTER, reg.data(), component_, handler_, this));
			}
		}
		break;
										  }
	case ComponentTreeItem::REGISTER: {
		Register* reg = static_cast<Register*>(dataPointer_);
		Q_ASSERT(reg);

		text_ = reg->getName();
		break;
									  }
	case ComponentTreeItem::ADDRESSSPACES: {
		text_ = tr("Address spaces");

		// get addressSpaces from the component and add them to the model
		QList<QSharedPointer<AddressSpace> >* list = &component_->getAddressSpaces();
		for (int i = 0; i < list->size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::ADDRESSSPACE, list->at(i).data(), component, handler, this));
		}
		dataPointer_ = list;
		break;
										   }
	case ComponentTreeItem::ADDRESSSPACE: {
		AddressSpace* addrSpace = static_cast<AddressSpace*>(dataPointer_);
		Q_ASSERT_X(addrSpace, "ComponentTreeItem constructor in case ADDRESSSPACE",
			"static_cast failed to give valid addressSpace-pointer");

		text_ = addrSpace->getName();
		break;
										  }
	case ComponentTreeItem::REMAPSTATES: {
		text_ = tr("Remap states");

		// get remapStates from the component and add them to the model
		QList<QSharedPointer<RemapState> > list = component_->getRemapStates();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::REMAPSTATE, list.at(i).data(), component, handler, this));
		}
		break;
										 }
	case ComponentTreeItem::REMAPSTATE: {
		RemapState* remapState = static_cast<RemapState*>(dataPointer_);
		Q_ASSERT_X(remapState, "ComponentTreeItem constructor in case REMAPSTATE",
			"static_cast failed to give valid RemapState-pointer");

		text_ = remapState->getName();
		break;
										}
	case ComponentTreeItem::VIEWS: {
		text_ = tr("Views");

		// get views from the component through model and set them to the model
		QList<QSharedPointer<View> > list = component_->getViews();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::VIEW, list.at(i).data(), component, handler, this));
		}
		break;
								   }
	case ComponentTreeItem::VIEW: {
		View* view = static_cast<View*>(dataPointer_);
		Q_ASSERT_X(view, "ComponentTreeItem constructor in case VIEW",
			"static_cast failed to give valid View-pointer");

		text_ = view->getName();
		break;
								  }

    case ComponentTreeItem::SWVIEWS: {
        text_ = tr("SW views");

        // get views from the component through model and set them to the model
        QList<QSharedPointer<SWView> > list = component_->getSWViews();
        for (int i = 0; i < list.size(); ++i) {
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::SWVIEW, list.at(i).data(), component, handler, this));
        }
        break;
                                   }
    case ComponentTreeItem::SWVIEW: {
        SWView* view = static_cast<SWView*>(dataPointer_);
        Q_ASSERT_X(view, "ComponentTreeItem constructor in case SWVIEW",
            "static_cast failed to give valid SWView-pointer");

        text_ = view->getName();
        break;
                                  }

	case ComponentTreeItem::PORTS: {
        text_ = tr("Ports");
		QMap<QString, QSharedPointer<Port> >* ports =
			component_->getModel()->getPortsPointer();
		dataPointer_ = ports;
		break;
								   }
	case ComponentTreeItem::BUSINTERFACES: {
        text_ = tr("Bus interfaces");
        
		// add each bus interface
		QMap<QString, QSharedPointer<BusInterface> > const& busifs = component_->getBusInterfaces();

		foreach (QSharedPointer<BusInterface> busif, busifs) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::BUSINTERFACE, busif.data(),
				component, handler, this));
		}
		break;
                                           }

    case ComponentTreeItem::BUSINTERFACE: {
		BusInterface* busInterface = static_cast<BusInterface*>(dataPointer_);
		Q_ASSERT_X(busInterface, "ComponentTreeItem constructor in case BUSINTERFACE",
			"static_cast failed to give valid BusInterface-pointer");

		text_ = busInterface->getName();
		break;
										  }
	case ComponentTreeItem::CHANNELS: {
		text_ = tr("Channels");

		// get the channels from the component and add them to the model
		QList<QSharedPointer<Channel> > list = component_->getChannels();
		for (int i = 0; i < list.size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHANNEL, list.at(i).data(), component, handler, this));
		}
		break;
									  }
	case ComponentTreeItem::CHANNEL: {
		Channel* channel = static_cast<Channel*>(dataPointer_);
		Q_ASSERT_X(channel, "ComponentTreeItem constructor in case CHANNEL",
			"static_cast failed to give valid Channel-pointer");

		text_ = channel->getName();
		break;
									 }
	case ComponentTreeItem::CPUS: {
		text_ = tr("Cpus");

		// get the cpus from the component and add them to the model
		QList<QSharedPointer<Cpu> >* list = &component_->getCpus();
		dataPointer_ = list;
		for (int i = 0; i < list->size(); ++i) {
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CPU, list->at(i).data(), component, handler, this));
		}
		break;
								  }
	case ComponentTreeItem::CPU: {
		Cpu* cpu = static_cast<Cpu*>(dataPointer_);
		Q_ASSERT_X(cpu, "ComponentTreeItem constructor in case CPU",
			"static_cast failed to give valid Cpu-pointer");

		text_ = cpu->getName();

		break;
								 }
	case ComponentTreeItem::OTHERCLOCKDRIVERS: {
		text_ = tr("Other clock drivers");

		QList<QSharedPointer<OtherClockDriver> >* drivers = 
			&component_->getOtherClockDrivers();
		dataPointer_ = drivers;
		break;
											   }
	case ComponentTreeItem::COMPONENTGENERATORS: {
		text_ = tr("Component generators");
		break;
												 }

    case ComponentTreeItem::SOFTWARE: {
        text_ = tr("Software mappings");
        //dataPointer_ = 0;
        break;
                                      }

    case ComponentTreeItem::SWPROPERTIES: {
        text_ = tr("SW properties");

        QList<QSharedPointer<ComProperty> >* params = &component_->getSWProperties();
        dataPointer_ = params;
        break;
                                          }

    case ComponentTreeItem::COMINTERFACES: {
        text_ = tr("COM interfaces");
        
        // Add each COM interface.
        QMap<QString, QSharedPointer<ComInterface> > const& interfaces = component_->getComInterfaces();

        foreach (QSharedPointer<ComInterface> comIf, interfaces)
        {
            childItems_.append(new ComponentTreeItem(ComponentTreeItem::COMINTERFACE, comIf.data(),
                                                     component, handler, this));
        }
        break;
                                           }

    case ComponentTreeItem::COMINTERFACE: {
        ComInterface* comIf = static_cast<ComInterface*>(dataPointer_);
        Q_ASSERT_X(comIf, "ComponentTreeItem constructor in case COMINTERFACE",
                   "static_cast failed to give valid ApiInterface-pointer");

        text_ = comIf->getName();
        break;
                                          }

    case ComponentTreeItem::APIINTERFACES: {
        text_ = tr("API interfaces");

        // Add each API interface.
        QMap<QString, QSharedPointer<ApiInterface> > const& interfaces = component_->getApiInterfaces();

        foreach (QSharedPointer<ApiInterface> apiIf, interfaces)
        {
            childItems_.append(new ComponentTreeItem(ComponentTreeItem::APIINTERFACE, apiIf.data(),
                                                     component, handler, this));
        }
        break;
                                           }

    case ComponentTreeItem::APIINTERFACE: {
        ApiInterface* apiIf = static_cast<ApiInterface*>(dataPointer_);
        Q_ASSERT_X(apiIf, "ComponentTreeItem constructor in case APIINTERFACE",
            "static_cast failed to give valid ApiInterface-pointer");

        text_ = apiIf->getName();
        break;
                                          }

	default: {

		// TODO remove this
		qDebug() << "Unsupported type in ComponentTreeItem constructor";
		break;
			 }
	}

}

ComponentTreeItem::~ComponentTreeItem() {
}

ComponentTreeItem* ComponentTreeItem::child(int index) {

	// if index is invalid
	if (childItems_.size() <= index || index < 0) {
		return 0;
	}

	return childItems_.value(index);
}

ComponentTreeItem* ComponentTreeItem::parent() {
	return parent_;
}

int ComponentTreeItem::row() {

	// if parent exists (this in not root item)
	if (parent_) {
		return parent_->getIndexOf(this);
	}

	// if this was root item
	return -1;
}

void* ComponentTreeItem::data() {
	return dataPointer_;
}

QString ComponentTreeItem::text() const {
	return text_;
}

bool ComponentTreeItem::hasChildren() {
	return !childItems_.isEmpty();
}

int ComponentTreeItem::getIndexOf(ComponentTreeItem *child) {
	return childItems_.indexOf(child);
}

int ComponentTreeItem::rowCount() const {
	return childItems_.size();
}

QFont ComponentTreeItem::getFont() const {

	QFont font(QApplication::font());

	switch (type_)
    {
    case ComponentTreeItem::GENERAL: 
    case ComponentTreeItem::FILESETS: 
    case ComponentTreeItem::CHOICES: 
    case ComponentTreeItem::MODELPARAMETERS:
    case ComponentTreeItem::PARAMETERS: 
    case ComponentTreeItem::MEMORYMAPS: 
    case ComponentTreeItem::ADDRESSSPACES: 
    case ComponentTreeItem::REMAPSTATES: 
    case ComponentTreeItem::VIEWS: 
    case ComponentTreeItem::SWVIEWS: 
    case ComponentTreeItem::PORTS: 
    case ComponentTreeItem::BUSINTERFACES:
    case ComponentTreeItem::CHANNELS: 
    case ComponentTreeItem::CPUS: 
    case ComponentTreeItem::OTHERCLOCKDRIVERS: 
    case ComponentTreeItem::COMPONENTGENERATORS:
    case ComponentTreeItem::SOFTWARE:
    case ComponentTreeItem::SWPROPERTIES:
    case ComponentTreeItem::COMINTERFACES:
    case ComponentTreeItem::APIINTERFACES: {
	    font.setPointSize(font.pointSize() + 2);
	    font.setBold(true);
	    return font;
                                           }
    default: {
	    return font;
             }
	}
}

void ComponentTreeItem::addChild( ComponentTreeItem* child ) {
	childItems_.append(child);
}

ComponentTreeItem::TreeItemType ComponentTreeItem::type() const {
	return type_;
}

QList<ComponentTreeItem*> ComponentTreeItem::getChildItems() {
	return childItems_;
}

void* ComponentTreeItem::getDataPointer() const {
	return dataPointer_;
}

void ComponentTreeItem::setText( const QString newText ) {
	text_ = newText;
}

void ComponentTreeItem::removeChild( ComponentTreeItem* childPointer ) {
	childItems_.removeAll(childPointer);
}

void ComponentTreeItem::getDataPointers( QList<void*>& pointers ) {
	// if the dataPointer is not null then append it to the list.
	if (dataPointer_) {
		pointers.append(dataPointer_);
	}

	// ask all children to append their dataPointers
	foreach (ComponentTreeItem* item, childItems_) {
		item->getDataPointers(pointers);
	}
}

bool ComponentTreeItem::createChild() {

	// the type of parent defines how a child is created
	switch (type_) {
		case ComponentTreeItem::ADDRESSSPACES: {
			AddressSpace* addrSpace = component_->createAddressSpace();
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::ADDRESSSPACE, addrSpace, component_, handler_, this));
			return true;
											   }
		case ComponentTreeItem::CPUS: {
			Cpu* cpu = component_->createCpu();
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CPU, cpu, component_, handler_, this));
			return true;
									  }

		case ComponentTreeItem::FILESETS: {
			FileSet* fileSet = component_->createFileSet();
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::FILESET, fileSet, component_, handler_, this));
			return true;
										  }
		case ComponentTreeItem::FILES: {
			// get pointer to the file set where the file is added to
			FileSet* fileSet = static_cast<FileSet*>(parent_->getDataPointer());

			QString componentPath = handler_->getPath(*component_->getVlnv());

			QStringList fileList = QFileDialog::getOpenFileNames(
				0, tr("Select files to add"), componentPath);

			foreach (QString filePath, fileList) {

				QString relPath = General::getRelativePath(componentPath, filePath);

				File* file = fileSet->createFile(relPath);
				childItems_.append(new ComponentTreeItem(
					ComponentTreeItem::FILE, file, component_, handler_, this));
			}

			return true;
									   }
		case ComponentTreeItem::CHANNELS: {

			// create a new empty channel
			Channel* chan = component_->createChannel();
			
			// create a child item that represents the channel
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::CHANNEL, chan, component_, handler_, this));

			return true;
										  }
		case ComponentTreeItem::BUSINTERFACES: {

			// create a new empty bus interface
			BusInterface* busif = component_->createBusInterface();

			// create a child item that represents the bus interface
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::BUSINTERFACE, busif, component_, handler_, this));

			return true;
											   }
        case ComponentTreeItem::COMINTERFACES: {

            // Create a new empty COM interface.
            ComInterface* comIf = component_->createComInterface();

            // create a child item that represents the COM interface
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::COMINTERFACE, comIf, component_, handler_, this));

            return true;
                                               }
        case ComponentTreeItem::APIINTERFACES: {

            // Create a new empty API interface.
            ApiInterface* apiIf = component_->createApiInterface();

            // create a child item that represents the API interface
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::APIINTERFACE, apiIf, component_, handler_, this));

            return true;
                                               }
		case ComponentTreeItem::VIEWS: {

			// create a new empty view
			View* view = component_->createView();

			// create a child item that represents the view
			childItems_.append(new ComponentTreeItem(
				ComponentTreeItem::VIEW, view, component_, handler_, this));

			return true;
									   }

        case ComponentTreeItem::SWVIEWS: {

            // create a new empty view
            SWView* view = component_->createSWView();

            // create a child item that represents the view
            childItems_.append(new ComponentTreeItem(
                ComponentTreeItem::SWVIEW, view, component_, handler_, this));

            return true;
                                       }
		// if the item type is such that the child can't be created
		default: {
			return false;
				 }
	}
}

bool ComponentTreeItem::canHaveChildren() const {
	switch (type_) {
		case ComponentTreeItem::ADDRESSSPACE:
		case ComponentTreeItem::CPU:
		case ComponentTreeItem::GENERAL:
		case ComponentTreeItem::FILE:
		case ComponentTreeItem::FILESET:
		case ComponentTreeItem::MODELPARAMETERS:
		case ComponentTreeItem::PORTS:
		case ComponentTreeItem::PARAMETERS:
		case ComponentTreeItem::OTHERCLOCKDRIVERS:
		case ComponentTreeItem::BUSINTERFACE:
		case ComponentTreeItem::VIEW:
        case ComponentTreeItem::SWVIEW:
		case ComponentTreeItem::DEFAULTFILEBUILDERS:
        case ComponentTreeItem::SOFTWARE:
        case ComponentTreeItem::SWPROPERTIES:
        case ComponentTreeItem::COMINTERFACE:
        case ComponentTreeItem::APIINTERFACE: {
			return false;
											  }
		default: {
			return true;
				 }
	}
}



bool ComponentTreeItem::canBeRemoved() const {
	switch (type_) {
		case ComponentTreeItem::GENERAL:
		case ComponentTreeItem::FILESETS:
		case ComponentTreeItem::CHOICES:
		case ComponentTreeItem::MODELPARAMETERS:
		case ComponentTreeItem::PARAMETERS:
		case ComponentTreeItem::MEMORYMAPS:
		case ComponentTreeItem::ADDRESSSPACES:
		case ComponentTreeItem::REMAPSTATES:
		case ComponentTreeItem::VIEWS:
        case ComponentTreeItem::SWVIEWS:
		case ComponentTreeItem::PORTS:
		case ComponentTreeItem::BUSINTERFACES:
		case ComponentTreeItem::CHANNELS:
		case ComponentTreeItem::CPUS:
		case ComponentTreeItem::OTHERCLOCKDRIVERS:
		case ComponentTreeItem::DEFAULTFILEBUILDERS:
		case ComponentTreeItem::FILES:
		case ComponentTreeItem::FUNCTIONS:
		case ComponentTreeItem::COMPONENTGENERATORS:
        case ComponentTreeItem::SOFTWARE:
        case ComponentTreeItem::SWPROPERTIES:
        case ComponentTreeItem::COMINTERFACES:
        case ComponentTreeItem::APIINTERFACES: {
			return false;
                                               }
		default: {
			return true;
				 }
	}
}

void ComponentTreeItem::moveChild(int sourceIndex, int targetIndex ) {

	// if there is no child in source index.
	if (sourceIndex < 0 || sourceIndex >= childItems_.size())
		return;
	else if (targetIndex < 0 || targetIndex > childItems_.size())
		return;

	ComponentTreeItem* itemToMove = childItems_.takeAt(sourceIndex);

	// if item is moved down then the target must be decremented because on item is
	// removed before adding to the list
	if (targetIndex > sourceIndex)
		--targetIndex;

	childItems_.insert(targetIndex, itemToMove);
	
	switch (type_) {
		// if one of these types then tell the parent item to sort the IP-Xact model
		case ComponentTreeItem::FILES: {
			QStringList fileNames;
			foreach (ComponentTreeItem* child, childItems_) {
				fileNames.append(child->text());
			}
			parent_->sortModel(fileNames);
			return;
									   }
		default:
			return;
	}
}

void ComponentTreeItem::sortModel(const QStringList& idList) {
	
	// currently only file sets have ability to be sorted.
	if (type_ != ComponentTreeItem::FILESET)
		return;

	FileSet* fileSet = static_cast<FileSet*>(dataPointer_);
	fileSet->sortFiles(idList);
}

void ComponentTreeItem::registerEditor( ItemEditor* editor ) {
	editor_ = editor;
}

bool ComponentTreeItem::isModelValid() const {

	switch (type_) 
	{
	case ComponentTreeItem::COMPONENT: {
		Component* comp = static_cast<Component*>(dataPointer_);
		Q_ASSERT(comp);
		return comp->isValid();
									   }
	case ComponentTreeItem::GENERAL: {
		// general is always true
		return true;
									 }
	case ComponentTreeItem::FILESETS: {

		// if at least one file set is invalid then display this as invalid
		QList<QSharedPointer<FileSet> > list = component_->getFileSets();
		foreach (QSharedPointer<FileSet> fileSet, list) {
			if (!fileSet->isValid(true)) {
				return false;
			}
		}

		// all file sets were valid
		return true;
									  }
	case ComponentTreeItem::FILESET: {
		// get pointer to the matching file set
		FileSet* fileSet = static_cast<FileSet*>(dataPointer_);

		return fileSet->isValid(true);
									 }
	case ComponentTreeItem::FILES: {

		// if at least one file is invalid
		const QList<QSharedPointer<File> >* files = 
			static_cast<const QList<QSharedPointer<File> >*>(dataPointer_);
		foreach (QSharedPointer<File> file, *files) {
			if (!file->isValid(true)) {
				return false;
			}
		}

		// if all files were valid
		return true;
								   }
	case ComponentTreeItem::FILE: {
		File* file = static_cast<File*>(dataPointer_);

		return file->isValid(true);
								  }
	case ComponentTreeItem::FUNCTIONS: {
		return false;
									   }
	case ComponentTreeItem::FUNCTION: {
		return false;
									  }
	case ComponentTreeItem::DEFAULTFILEBUILDERS: {

		// if at least one default file builder is invalid
		QList<QSharedPointer<FileBuilder> >* builders = 
			static_cast<QList<QSharedPointer<FileBuilder> >*>(dataPointer_);
		foreach (QSharedPointer<FileBuilder> fileBuilder, *builders) {
			if (!fileBuilder->isValid()) {
				return false;
			}
		}

		// all builders were valid
		return true;
												 }
	case ComponentTreeItem::CHOICES: {
		return false;
									 }
	case ComponentTreeItem::CHOICE: {
		return false;
									}
	case ComponentTreeItem::MODELPARAMETERS: {

		// if at least one model parameter is invalid
		QMap<QString, QSharedPointer<ModelParameter> >* modelParams = 
			component_->getModel()->getModelParametersPointer();
		foreach (QSharedPointer<ModelParameter> modelParam, *modelParams) {
			if (!modelParam->isValid()) {
				return false;
			}
		}

		// all model parameters were valid
		return true;
											 }
	case ComponentTreeItem::PARAMETERS: {

		// if at least one parameter is invalid
		QList<QSharedPointer<Parameter> > params = component_->getParameters();
		foreach (QSharedPointer<Parameter> param, params) {
			if (!param->isValid()) {
				return false;
			}
		}

		// all parameters were valid
		return true;
										}
	case ComponentTreeItem::MEMORYMAPS: {
		QList<QSharedPointer<MemoryMap> > memMaps = component_->getMemoryMaps();
		foreach (QSharedPointer<MemoryMap> memMap, memMaps) {
			if (!memMap->isValid()) {
				return false;
			}
		}
		return true;
										}
	case ComponentTreeItem::MEMORYMAP: {
		MemoryMap* memMap = static_cast<MemoryMap*>(dataPointer_);
		return memMap->isValid();
									   }
	case ComponentTreeItem::ADDRESSBLOCK: {
		AddressBlock* addrBlock = static_cast<AddressBlock*>(dataPointer_);
		return addrBlock->isValid();
										  }
	case ComponentTreeItem::REGISTER: {
		Register* reg = static_cast<Register*>(dataPointer_);
		return reg->isValid();
									  }
	case ComponentTreeItem::ADDRESSSPACES: {
		
		// if at least one address space is invalid
		QList<QSharedPointer<AddressSpace> > addSpaces = component_->getAddressSpaces();
		foreach (QSharedPointer<AddressSpace> addrSpace, addSpaces) {
			if (!addrSpace->isValid()) {
				return false;
			}	
		}

		// all address spaces were valid
		return true;
										   }
	case ComponentTreeItem::ADDRESSSPACE: {
		AddressSpace* addrSpace = static_cast<AddressSpace*>(dataPointer_);
		return addrSpace->isValid();
										  }
	case ComponentTreeItem::REMAPSTATES: {
		return false;
										 }
	case ComponentTreeItem::REMAPSTATE: {
		return false;
										}
	case ComponentTreeItem::VIEWS: {

		// if at least one view is invalid
		QList<QSharedPointer<View> > list = component_->getViews();
		QStringList fileSetNames = component_->getFileSetNames();
		foreach (QSharedPointer<View> view, list) {
			if (!view->isValid(fileSetNames)) {
				return false;
			}
		}

		// if all views are valid
		return true;
								   }
	case ComponentTreeItem::VIEW: {
		View* view = static_cast<View*>(dataPointer_);
		return view->isValid(component_->getFileSetNames());
								  }

    case ComponentTreeItem::SWVIEWS: {

        // if at least one view is invalid
        QList<QSharedPointer<SWView> > list = component_->getSWViews();
        QStringList fileSetNames = component_->getFileSetNames();
        foreach (QSharedPointer<SWView> view, list) {
            if (!view->isValid()) {
                return false;
            }
        }

        // if all views are valid
        return true;
                                   }
    case ComponentTreeItem::SWVIEW: {
        SWView* view = static_cast<SWView*>(dataPointer_);
        return view->isValid();
                                  }

	case ComponentTreeItem::PORTS: {

		// if at least one port is invalid
		QMap<QString, QSharedPointer<Port> >* ports =
			component_->getModel()->getPortsPointer();
		bool hasViews = component_->hasViews();
		foreach (QSharedPointer<Port> port, *ports) {
			if (!port->isValid(hasViews)) {
				return false;
			}
		}

		// all ports were valid
		return true;
								   }
	case ComponentTreeItem::BUSINTERFACES: {

		// if at least one bus interface is invalid
		QMap<QString, QSharedPointer<BusInterface> > const& busifs = component_->getBusInterfaces();
		QList<General::PortBounds> const& portBounds = component_->getPortBounds();

		foreach (QSharedPointer<BusInterface> busif, busifs) {
			if (!busif->isValid(portBounds)) {
				return false;
			}
		}

		// if all bus interfaces were valid
		return true;
										   }
	case ComponentTreeItem::BUSINTERFACE: {
		BusInterface* busInterface = static_cast<BusInterface*>(dataPointer_);
		const QList<General::PortBounds> portBounds = component_->getPortBounds();
		return busInterface->isValid(portBounds);
										  }

    case ComponentTreeItem::SWPROPERTIES: {

        // if at least one property is invalid
        QList<QSharedPointer<ComProperty> > properties = component_->getSWProperties();
        foreach (QSharedPointer<ComProperty> prop, properties) {
            if (!prop->isValid()) {
                return false;
            }
        }

        // all parameters were valid
        return true;
                                        }


    case ComponentTreeItem::COMINTERFACES: {
        // Check for invalid COM interfaces.
        QMap<QString, QSharedPointer<ComInterface> > const& interfaces = component_->getComInterfaces();
        
        foreach (QSharedPointer<ComInterface> comIf, interfaces)
        {
            if (!comIf->isValid())
            {
                return false;
            }
        }

        // All COM interfaces were valid.
        return true;
                                           }
    case ComponentTreeItem::COMINTERFACE: {
        ComInterface* comIf = static_cast<ComInterface*>(dataPointer_);
        return comIf->isValid();
                                          }
    case ComponentTreeItem::APIINTERFACES: {
        // Check for invalid API interfaces.
        QMap<QString, QSharedPointer<ApiInterface> > const& interfaces = component_->getApiInterfaces();

        foreach (QSharedPointer<ApiInterface> apiIf, interfaces)
        {
            if (!apiIf->isValid())
            {
                return false;
            }
        }

        // All API interfaces were valid.
        return true;
                                           }
    case ComponentTreeItem::APIINTERFACE: {
        ApiInterface* apiIf = static_cast<ApiInterface*>(dataPointer_);
        return apiIf->isValid();
                                          }
	case ComponentTreeItem::CHANNELS: {

		// if at least one channel is invalid
		QList<QSharedPointer<Channel> > list = component_->getChannels();
		QStringList busifNames = component_->getBusInterfaceNames();
		foreach (QSharedPointer<Channel> channel, list) {
			if (!channel->isValid(busifNames)) {
				return false;
			}
		}

		// if all channels are valid
		return true;
									  }
	case ComponentTreeItem::CHANNEL: {
		QStringList busifNames = component_->getBusInterfaceNames();
		Channel* channel = static_cast<Channel*>(dataPointer_);
		return channel->isValid(busifNames);
									 }
	case ComponentTreeItem::CPUS: {
		QStringList addrSpaceNames = component_->getAddressSpaceNames();
		QList<QSharedPointer<Cpu> >* cpus = static_cast<QList<QSharedPointer<Cpu> >* >(dataPointer_);
		foreach (QSharedPointer<Cpu> cpu, *cpus) {
			if (!cpu->isValid(addrSpaceNames)) {
				return false;
			}
		}
		return true;
								  }
	case ComponentTreeItem::CPU: {
		QStringList addrSpaceNames = component_->getAddressSpaceNames();
		Cpu* cpu = static_cast<Cpu*>(dataPointer_);
		return cpu->isValid(addrSpaceNames);
								 }
	case ComponentTreeItem::OTHERCLOCKDRIVERS: {

		// if at least one driver is invalid
		QList<QSharedPointer<OtherClockDriver> > drivers = 
			component_->getOtherClockDrivers();
		foreach (QSharedPointer<OtherClockDriver> driver, drivers) {
			if (!driver->isValid()) {
				return false;
			}
		}

		// if all drivers were valid
		return true;
											   }
	case ComponentTreeItem::COMPONENTGENERATORS: {
		return false;
												 }
	case ComponentTreeItem::SOFTWARE: {
		return true;
									  }
	default: {
		return false;
			 }
	}

	return false;
}

bool ComponentTreeItem::isValid() const {
	if (text().isEmpty()) {
		return false;
	}
	else if (!isModelValid()) {
		return false;
	}
	else if (editor_ && !editor_->isValid()) {
		return false;
	}

	return true;
}
