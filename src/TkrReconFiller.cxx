#include "TkrReconFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrVertex.h"
#include "Event/Recon/TkrRecon/TkrFitTrackBase.h"

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
    
  
}


// This method build the types for the HepRep
void TkrReconFiller::buildTypes()
{
  m_builder->addType("Recon","TkrRecon","TkrRecon Tree","");

  m_builder->addType("TkrRecon","TrackCol",
		     "Reconstructed Tracks collections","");

  m_builder->addType("TkrRecon","GammaVtxCol",
		     "Reconstructed Gamma Vertex collection","");

  m_builder->addType("TrackCol","Track","Reconstructed track","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","blue","");

  m_builder->addType("Track","TrackChiPlane",
		     "Chi Square plane segments","");


  m_builder->addType("GammaVtxCol","GammaVtx","Reconstructed Gamma Vertex","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","yellow","");
  m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
  m_builder->addAttDef("Q","Quality","Physics","");
  m_builder->addAttDef("Layer","Layer of conversion","Physics","");
  m_builder->addAttDef("Tower","Tower of conversion","Physics","");

}


// This method fill the instance tree Event with the actual TDS content
void TkrReconFiller::fillInstances (std::vector<std::string>& typesList)
{
  m_builder->addInstance("Recon","TkrRecon");

  if (hasType(typesList,"GammaVtx"))
    {
      Event::TkrVertexCol* pVertices = SmartDataPtr<Event::TkrVertexCol>(m_dpsvc,"/Event/TkrRecon/TkrVertexCol");
      
      //Now see if we can do the drawing
      if (pVertices)
	{
	  m_builder->addInstance("TkrRecon","GammaVtxCol");
	  Event::TkrVertexCol::const_iterator iter;
	  
	  for(iter = pVertices->begin(); iter != pVertices->end(); ++iter)
	    {
	      m_builder->addInstance("GammaVtxCol","GammaVtx");
	      const Event::TkrVertex& pVertex = **iter;
	      
	      Point endPoint = Point(pVertex.getPosition()) 
		- 1000.*pVertex.getDirection();


	      m_builder->addAttValue("Q", (float)(pVertex.getQuality()), "");
	      m_builder->addAttValue("E", (float)(pVertex.getEnergy()), "");
	      m_builder->addAttValue("Layer", (pVertex.getLayer()), "");
	      m_builder->addAttValue("Tower", (pVertex.getTower()), "");

	      double sx = pVertex.getPosition().x();
              double sy = pVertex.getPosition().y();
              double sz = pVertex.getPosition().z();	      
	      double ex = endPoint.x();
              double ey = endPoint.y();
              double ez = endPoint.z();	      
	      // draw reconstructed gamma
	      m_builder->addPoint(sx,sy,sz);
              m_builder->addPoint(ex,ey,ez);
	    }
	}
  
    }

  if (hasType(typesList,"Track"))
    { 
      Event::TkrFitTrackCol* pTracks = 
	SmartDataPtr<Event::TkrFitTrackCol>(m_dpsvc,EventModel::TkrRecon::TkrFitTrackCol);
      
      //Now see if we can do the drawing
      if (pTracks)
	{
	  m_builder->addInstance("TkrRecon","TrackCol");

	  int numTracks = pTracks->size();
	  
	  if (numTracks > 0) 
	    {
	      Event::TkrFitTrackCol::const_iterator it = pTracks->begin();

	      while(it != pTracks->end())
		{
		  m_builder->addInstance("TrackCol","Track");
		  const Event::TkrFitTrackBase& track = **it++;

		  Event::TkrFitHit::TYPE  fit      = Event::TkrFitHit::SMOOTH;
		  Event::TkrFitHit::TYPE  typ      = Event::TkrFitHit::SMOOTH;

		  Event::TkrFitPlaneConPtr hitIter = track.begin();
		  while(hitIter < track.end())
		    {
		      m_builder->addInstance("Track","TrackChiPlane");

		      Event::TkrFitPlane plane = *hitIter++;

		      Event::TkrFitPlane::AXIS prj = plane.getProjection();

		      double x0, y0, z0, xl, xr, yl, yr;
		      double delta= plane.getDeltaChiSq(typ)*10.; //Scale factor! We're in mm now!

		      if(prj == Event::TkrCluster::X){
			x0 = plane.getHit(typ).getPar().getXPosition();
			y0 = plane.getHit(fit).getPar().getYPosition(); 
			z0 = plane.getZPlane()+0.1;
			xl = x0-0.5*delta;
			xr = x0+0.5*delta;
			yl = y0;
			yr = y0;
		      } 
		      else {
			x0 = plane.getHit(fit).getPar().getXPosition();
			y0 = plane.getHit(typ).getPar().getYPosition(); 
			z0 = plane.getZPlane()+0.1;
			xl = x0;
			xr = x0;
			yl = y0-0.5*delta;
			yr = y0+0.5*delta;
		      }       
		      m_builder->addPoint(xl,yl,z0);
		      m_builder->addPoint(xr,yr,z0);
		    }
		}
	      
	      
	    }
	}
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
