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
    m_builder->addAttDef("Hit Status","Hit Status Bits","Physics","");
    m_builder->addAttDef("Energy","Energy reconstructed","Physics","MeV");
    m_builder->addAttDef("ChiSquare","ChiSquare of fit", "Physics", "");
    m_builder->addAttDef("Quality","Quality","Physics","");
    m_builder->addAttDef("ArcLen1","Arclen of track 1", "Physics", "mm");
    m_builder->addAttDef("ArcLen2","Arclen of track 2", "Physics", "mm");
    m_builder->addAttDef("DOCA", "Distance of Closest Approach", "Physics", "mm");
}


// This method fill the instance tree Event with the actual TDS content
void VertexFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Recon/TkrRecon/GammaVtxCol/GammaVtx"))
    {
        Event::TkrVertexCol* pVertices = SmartDataPtr<Event::TkrVertexCol>(m_dpsvc,"/Event/TkrRecon/TkrVertexCol");
      
        //Now see if we can do the drawing
        if (pVertices)
	    {
            std::string color("yellow");
            double         lineWidth = 2;
	        m_builder->addInstance("TkrRecon","GammaVtxCol");
	        Event::TkrVertexCol::const_iterator iter;
	  
	        for(iter = pVertices->begin(); iter != pVertices->end(); ++iter)
	        {
	            m_builder->addInstance("GammaVtxCol","GammaVtx");
	            const Event::TkrVertex& pVertex = **iter;
	      
	            Point endPoint = Point(pVertex.getPosition()) 
		                       - 10000.*pVertex.getDirection();


                m_builder->addAttValue("Color",color,"");
                m_builder->addAttValue("LineWidth", (float)lineWidth, "");
	            m_builder->addAttValue("Energy",    (float)(pVertex.getEnergy()), "");
                m_builder->addAttValue("ChiSquare", (float)(pVertex.getChiSquare()), "");
	            m_builder->addAttValue("Quality",   (float)(pVertex.getQuality()), "");
                m_builder->addAttValue("ArcLen1",   (float)(pVertex.getTkr1ArcLen()), "");
                m_builder->addAttValue("ArcLen2",   (float)(pVertex.getTkr2ArcLen()), "");
                m_builder->addAttValue("DOCA",      (float)(pVertex.getDOCA()), "");
                    
                //Build string for status bits
                std::stringstream outString;
                unsigned int      statBits = pVertex.getStatusBits();
                outString.setf(std::ios::hex);
                outString << std::hex << statBits;
                m_builder->addAttValue("Hit Status",outString.str(),"");

	            double sx = pVertex.getPosition().x();
                double sy = pVertex.getPosition().y();
                double sz = pVertex.getPosition().z();	      
	            double ex = endPoint.x();
                double ey = endPoint.y();
                double ez = endPoint.z();	      
	            // draw reconstructed gamma
	            m_builder->addPoint(sx,sy,sz);
                m_builder->addPoint(ex,ey,ez);

                color     = "gray";
                lineWidth = 1;
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
