#include "ClusterFiller.h"
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
ClusterFiller::ClusterFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    double siWaferActiveSide;
    int    ladderNStrips;

    gsvc->getNumericConstByName("towerPitch",        &m_towerPitch);
    gsvc->getNumericConstByName("SiWaferActiveSide", &siWaferActiveSide);
    gsvc->getNumericConstByName("stripPerWafer",     &ladderNStrips);
    gsvc->getNumericConstByName("SiThick",           &m_siThickness);

    m_siStripPitch = siWaferActiveSide / ladderNStrips;
}


// This method build the types for the HepRep
void ClusterFiller::buildTypes()
{  
    m_builder->addType("TkrRecon","TkrClusterCol",
		     "Reconstructed Cluster collections","");

    m_builder->addType("TkrClusterCol","TkrCluster","Reconstructed Cluster","");
    m_builder->addAttValue("DrawAs","Prism","");
    //m_builder->addAttValue("DrawAs","Line","");
    //m_builder->addAttValue("MarkName","Cross","");
    m_builder->addAttValue("Color","green","");
    m_builder->addAttDef("ID","Cluster ID","Physics","");
    m_builder->addAttDef("Plane","Cluster Plane #","Physics","");
    m_builder->addAttDef("Tower","Cluster Tower #","Physics","");
    m_builder->addAttDef("View","Cluster View","Physics","");
    m_builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    m_builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    m_builder->addAttDef("Position","Cluster Global Position","Physics","");
    m_builder->addAttDef("ToT","Cluster Time over Threshold","Physics","");

    m_builder->addType("TkrCluster","TkrClusterToT"," ","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","green","");
}


// This method fill the instance tree Event with the actual TDS content
void ClusterFiller::fillInstances (std::vector<std::string>& typesList)
{  
    if (hasType(typesList,"Recon/TkrRecon/TkrClusterCol"))
    {
        Event::TkrClusterCol* pClusters = SmartDataPtr<Event::TkrClusterCol>(m_dpsvc,EventModel::TkrRecon::TkrClusterCol);

        if (pClusters)
        {
            int    nHits      = pClusters->nHits();

            m_builder->addInstance("TkrRecon","TkrClusterCol");


            //Loop over all cluster hits in the SiClusters vector
            if (hasType(typesList,"Recon/TkrRecon/TkrClusterCol/TkrCluster"))
              while(nHits--)
              {
                Event::TkrCluster* pCluster = pClusters->getHit(nHits);
                Point              clusPos  = pCluster->position();

                double x    = clusPos.x();
                double y    = clusPos.y();
                double z    = clusPos.z();
                double dx   = 0.5 * pCluster->size() * m_siStripPitch;
                double dy   = 0.5 * (m_towerPitch - 20.);  // One day we will fix this...
                double dz   = 0.5 * m_siThickness;
                double xToT = x;
                double yToT = y - dy;
                double ToT  = pCluster->ToT() / 64.;       // So, max ToT = 4 mm

                int    view   = pCluster->v();

                m_builder->addInstance("TkrClusterCol","TkrCluster");

                m_builder->addAttValue("ID",          pCluster->id(),"");
                m_builder->addAttValue("Plane",       pCluster->plane(),"");
                m_builder->addAttValue("Tower",       pCluster->tower(),"");
                m_builder->addAttValue("View",        view,"");
                m_builder->addAttValue("First Strip", pCluster->firstStrip(),"");
                m_builder->addAttValue("Last Strip",  pCluster->lastStrip(),"");

                //Build string for cluster position
                std::stringstream clusterPosition;
                clusterPosition.setf(std::ios::fixed);
                clusterPosition.precision(3);
                clusterPosition << " (" << pCluster->position().x()
                  << ","  << pCluster->position().y() 
                  << ","  << pCluster->position().z() << ")";
                m_builder->addAttValue("Position",clusterPosition.str(),"");

                m_builder->addAttValue("ToT",         (float)(pCluster->ToT()),"");

                //Draw the width of the cluster
                if (pCluster->v() == Event::TkrCluster::Y)
                {
                  dy   = 0.5 * pCluster->size() * m_siStripPitch;
                  dx   = 0.5 * m_towerPitch;
                  xToT = x - dx;
                  yToT = y;
                }

                //Now draw the hit strips
                m_builder->addPoint(x+dx,y+dy,z+dz);
                m_builder->addPoint(x-dx,y+dy,z+dz);
                m_builder->addPoint(x-dx,y-dy,z+dz);
                m_builder->addPoint(x+dx,y-dy,z+dz);
                m_builder->addPoint(x+dx,y+dy,z-dz);
                m_builder->addPoint(x-dx,y+dy,z-dz);
                m_builder->addPoint(x-dx,y-dy,z-dz);
                m_builder->addPoint(x+dx,y-dy,z-dz);

                //Time over threshold add here
                if (hasType(typesList,"Recon/TkrRecon/TkrClusterCol/TkrCluster/TkrClusterToT"))
                {
                  m_builder->addInstance("TkrCluster","TkrClusterToT");
                  m_builder->addPoint(xToT,yToT,z);
                  m_builder->addPoint(xToT,yToT,z+ToT);
                }
              }
        }        
    } 
}


bool ClusterFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
