#include "ReconFiller.h"
#include "AcdReconFiller.h"
#include "CalReconFiller.h"
#include "TkrReconFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

ReconFiller::ReconFiller(IGlastDetSvc* gsvc,
                         IDataProviderSvc* dpsvc,
                         IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
  AcdReconFiller* acdRecon = new AcdReconFiller(m_gdsvc, m_dpsvc, m_ppsvc);
  CalReconFiller* calRecon = new CalReconFiller(m_gdsvc, m_dpsvc, m_ppsvc);
  TkrReconFiller* tkrRecon = new TkrReconFiller(m_gdsvc, m_dpsvc, m_ppsvc);
  m_subFillers.push_back(acdRecon);
  m_subFillers.push_back(calRecon);
  m_subFillers.push_back(tkrRecon);
}

void ReconFiller::setBuilder(IBuilder* b)
{
  m_builder = b;
  for(unsigned int i=0;i<m_subFillers.size();i++)
    m_subFillers[i]->setBuilder(b);
}

// This method build the types for the HepRep
void ReconFiller::buildTypes()
{
  m_builder->addType("","Recon","Recon Tree","");
  m_builder->addAttValue("Layer","Event","");
 
  for(unsigned int i=0;i<m_subFillers.size();i++)
  {
    m_subFillers[i]->buildTypes();
  }  
}


// This method fill the instance tree Event/MC with the actual TDS content
void ReconFiller::fillInstances (std::vector<std::string>& typesList)
{
  if (!hasType(typesList, "Recon"))  
    return;
  
  m_builder->addInstance("","Recon");
  for(unsigned int i=0;i<m_subFillers.size();i++)
    m_subFillers[i]->fillInstances(typesList);
}

bool ReconFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}

