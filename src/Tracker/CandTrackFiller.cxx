#include "CandTrackFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrPatCand.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <sstream>
#include <iostream>
#include <algorithm>


// Constructor
CandTrackFiller::CandTrackFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    
  
}


// This method build the types for the HepRep
void CandTrackFiller::buildTypes()
{
    m_builder->addType("TkrRecon","CandidateCol", "Candidate Track Collection","");

    m_builder->addType("CandidateCol","Candidate","Candidate track","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","green","");
    m_builder->addAttValue("LineStyle","Dashed","");
    m_builder->addAttDef("TrackId","Track ID #","Physics","");

    m_builder->addType("Candidate","CandHit", "Candidate Track Hits", "");
    m_builder->addAttDef("Plane","Plane ID","Physics","");
}


// This method fill the instance tree Event with the actual TDS content
void CandTrackFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList, "Recon/TkrRecon"))
      return;

    if (hasType(typesList,"Recon/TkrRecon/CandidateCol/Candidate"))
    { 
        Event::TkrPatCandCol* pTkrCandidates = SmartDataPtr<Event::TkrPatCandCol>(m_dpsvc,EventModel::TkrRecon::TkrPatCandCol);

        // If candidates exist, draw them!
        if (pTkrCandidates)
        {
	        m_builder->addInstance("TkrRecon","CandidateCol");

            // Loop over the candidate tracks
            for(Event::TkrPatCandColPtr patCandIter = pTkrCandidates->begin(); patCandIter != pTkrCandidates->end(); patCandIter++)
            {
                Event::TkrPatCand* patCand = *patCandIter;

                //Create new candidate track instance
                m_builder->addInstance("CandidateCol","Candidate");
                //m_builder->addAttValue("TrackId", trackId++, "");

                // Set the starting point for drawing the candidate track
                Point x0        = patCand->getPosition();
                Point x1        = x0;
                int   lastLayer = -1;

                m_builder->addPoint(x0.x(),x0.y(),x0.z());

                for(Event::CandHitVectorPtr hitIter = patCand->begin(); 
                                        hitIter != patCand->end(); hitIter++)
                {
                    // Pointer to candidate hit
                    Event::TkrPatCandHit* candHit = *hitIter;
                    // Point to move to
                    Point xCur     = candHit->Position();
                    int   curLayer = candHit->PlaneIndex();

                    if (curLayer == lastLayer)
                    {
                        // Set the point to draw to
                        if (candHit->View() == idents::TkrId::eMeasureX) x1.setX(xCur.x());
                        else                                             x1.setY(xCur.y());
                        x1.setZ(0.5*(x1.z()+xCur.z()));

                        // Draw a line between the two points
                        m_builder->addPoint(x1.x(),x1.y(),x1.z());

                        x0 = x1;
                    }
                    else
                    {
                        // Set the point to draw to
                        if (candHit->View() == idents::TkrId::eMeasureX) x1.setX(xCur.x());
                        else                                             x1.setY(xCur.y());
                        x1.setZ(xCur.z());

                        lastLayer = curLayer;
                    }
                }
            }
	    }
    }
}


bool CandTrackFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
