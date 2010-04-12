#include "VertexFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

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
VertexFiller::VertexFiller(HepRepInitSvc* hrisvc,
                           IGlastDetSvc* gsvc,
                           IDataProviderSvc* dpsvc,
                           IParticlePropertySvc* ppsvc
):
              m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{  
}


// This method build the types for the HepRep
void VertexFiller::buildTypes()
{
    //m_control = HepRepControl::getPtr();
    m_builder->addType("TkrRecon","GammaVtxCol",
		     "Reconstructed Gamma Vertex collection","");
  
    m_builder->addType("GammaVtxCol","GammaVtx","Reconstructed Gamma Vertex","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","yellow","");

    // This will make all the vertex extrapolations dashed
    if(m_hrisvc->getVertexFiller_dashes()) {
        m_builder->addAttValue("LineStyle", "Dashed", "");
    }
    m_builder->addAttDef("Status Low","Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","High Status Bits","Physics","");
    m_builder->addAttDef("Start Position","Vtx start position","Physics","");
    m_builder->addAttDef("Start Direction","Vtx start direction","Physics","");
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
			m_builder->setSubinstancesNumber("GammaVtxCol",pVertices->size());
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
                m_builder->addAttValue("Start Position", 
                    getPositionString(pVertex.getPosition()), "");
                m_builder->addAttValue("Start Direction", 
                    getDirectionString(pVertex.getDirection()), "");
                    
                //Build strings for status bits
                unsigned int      statBits = pVertex.getStatusBits();
                m_builder->addAttValue("Status Low",getBits(statBits, 15, 0),"");
                m_builder->addAttValue("Status High",getBits(statBits, 31, 16),"");

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
                lineWidth = 3;
	        }
	    }
  
    }

}
