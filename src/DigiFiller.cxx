#include "DigiFiller.h"
#include "FilterFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "TkrUtil/ITkrGeometrySvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

DigiFiller::DigiFiller(HepRepInitSvc* hrisvc,
                         IGlastDetSvc* gsvc,
                         ITkrGeometrySvc* tgsvc,
                         IDataProviderSvc* dpsvc):
  m_hrisvc(hrisvc),m_gdsvc(gsvc),m_tgsvc(tgsvc), m_dpsvc(dpsvc)
{
  FilterFiller* filter = new FilterFiller(m_hrisvc,m_gdsvc, m_dpsvc);
  m_subFillers.push_back(filter);
}

void DigiFiller::setBuilder(IBuilder* b)
{
  m_builder = b;
  for(unsigned int i=0;i<m_subFillers.size();i++)
    m_subFillers[i]->setBuilder(b);
}

// This method build the types for the HepRep
void DigiFiller::buildTypes()
{
  m_builder->addType("","Digi","Digi Tree","");
  m_builder->addAttValue("Layer","Event","");
 
  for(unsigned int i=0;i<m_subFillers.size();++i)
  {
    m_subFillers[i]->buildTypes();
  }  
}


// This method fill the instance tree Event/MC with the actual TDS content
void DigiFiller::fillInstances (std::vector<std::string>& typesList)
{
  if (!hasType(typesList, "Digi"))  
    return;
  
  m_builder->addInstance("","Digi");
  for(unsigned int i=0;i<m_subFillers.size();++i)
    m_subFillers[i]->fillInstances(typesList);
}
