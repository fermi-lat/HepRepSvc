#include "TkrReconFiller.h"

#include "Tracker/ClusterFiller.h"
#include "Tracker/CandTrackFiller.h"
//#include "Tracker/FitTrackFiller.h"
#include "Tracker/TrackFiller.h"
#include "Tracker/VertexFiller.h"

#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>


// Constructor
TkrReconFiller::TkrReconFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    fillVector.clear();

    fillVector.push_back(new ClusterFiller(gsvc,dpsvc,ppsvc));
    fillVector.push_back(new CandTrackFiller(gsvc,dpsvc,ppsvc));
    fillVector.push_back(new TrackFiller(gsvc,dpsvc,ppsvc));
    fillVector.push_back(new VertexFiller(gsvc,dpsvc,ppsvc));
}

TkrReconFiller::~TkrReconFiller()
{
  for(std::vector<IFiller*>::const_iterator fillIter = fillVector.begin(); fillIter < fillVector.end(); fillIter++)
  {
      delete (*fillIter);
  }

  fillVector.clear();
}

// We need to set the builder for our sub fillers
void TkrReconFiller::setBuilder(IBuilder* b)
{
    //Set the builders to our list of sub fillers
    for(std::vector<IFiller*>::const_iterator fillIter = fillVector.begin(); fillIter < fillVector.end(); fillIter++)
    {
        (*fillIter)->setBuilder(b);
    }

    IFiller::setBuilder(b);
};

// This method build the types for the HepRep
void TkrReconFiller::buildTypes()
{
  m_builder->addType("Recon","TkrRecon","TkrRecon Tree","");

  for(std::vector<IFiller*>::const_iterator fillIter = fillVector.begin(); fillIter < fillVector.end(); fillIter++)
  {
      (*fillIter)->buildTypes();
  }
}


// This method fill the instance tree Event with the actual TDS content
void TkrReconFiller::fillInstances (std::vector<std::string>& typesList)
{
  if (!hasType(typesList, "Recon/TkrRecon"))
    return;
  
  m_builder->addInstance("Recon","TkrRecon");

  for(std::vector<IFiller*>::const_iterator fillIter = fillVector.begin(); fillIter < fillVector.end(); fillIter++)
  {
      (*fillIter)->fillInstances(typesList);
  }
}


bool TkrReconFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
