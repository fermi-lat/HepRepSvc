#include "VertexFiller.h"
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
VertexFiller::VertexFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    
  
}


// This method build the types for the HepRep
void VertexFiller::buildTypes()
{
  m_builder->addType("TkrRecon","GammaVtxCol",
		     "Reconstructed Gamma Vertex collection","");
  
  m_builder->addType("GammaVtxCol","GammaVtx","Reconstructed Gamma Vertex","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","yellow","");
  m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
  m_builder->addAttDef("Q","Quality","Physics","");
  m_builder->addAttDef("Layer","Layer of conversion","Physics","");
  m_builder->addAttDef("Tower","Tower of conversion","Physics","");
}


// This method fill the instance tree Event with the actual TDS content
void VertexFiller::fillInstances (std::vector<std::string>& typesList)
{
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

}


bool VertexFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
