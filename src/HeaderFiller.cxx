#include "HeaderFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/Event.h"
#include "Event/TopLevel/EventModel.h"
#include <sstream>
#include <algorithm>

// This method build the types for the HepRep
void HeaderFiller::buildTypes()
{
  m_builder->addType("","Header","Event Header Tree","");
  m_builder->addAttValue("Layer","Event","");
  
  m_builder->addAttValue("DrawAs","Text","");
  m_builder->addAttDef("EventID","The event identifier","Physics","");
}


// This method fill the instance tree Event/MC with the actual TDS content
void HeaderFiller::fillInstances (std::vector<std::string>& typesList)
{
  std::stringstream sName;
 
  // This is to retrive event and run number from the event
  SmartDataPtr<Event::EventHeader>
     evt(m_dpsvc, EventModel::EventHeader);
  
  if (evt)
   {
     m_builder->addInstance("","Header");

     unsigned int evtRun = evt->run();
     unsigned int evtEvent = evt->event();
     sName << evtRun << "-" << evtEvent; 

     double hpos = 0.05;
     double vpos = 0.1;
     
     m_builder->addAttValue("EventID",sName.str(),"");
     m_builder->addAttValue("Text","ID: " + sName.str(),"");
     m_builder->addAttValue("HPos",(float)hpos,"");
     m_builder->addAttValue("VPos",(float)vpos,"");     
   }  

}


bool HeaderFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}

