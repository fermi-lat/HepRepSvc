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

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

namespace {
    float _markerSize = 25.0;
    float _markerWidth = 2.0;
    bool  _scalingMarker  = true;
}

// Constructor
ClusterFiller::ClusterFiller(IGlastDetSvc* gsvc,
                             IDataProviderSvc* dpsvc,
                             IParticlePropertySvc* ppsvc):
m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    int    ladderNStrips;

    gsvc->getNumericConstByName("SiWaferActiveSide", &m_siWaferActiveSide);
    gsvc->getNumericConstByName("stripPerWafer",     &ladderNStrips);
    gsvc->getNumericConstByName("SiThick",           &m_siThickness);
    gsvc->getNumericConstByName("SiWaferSide",       &m_siWaferSide);
    gsvc->getNumericConstByName("ssdGap",            &m_ssdGap);
    gsvc->getNumericConstByName("nWaferAcross",     &m_nWaferAcross);

    m_stripLength = m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*m_ssdGap;

    m_siStripPitch = m_siWaferActiveSide / ladderNStrips;
}


// This method build the types for the HepRep
void ClusterFiller::buildTypes()
{  
    m_builder->addType("TkrRecon","TkrClusterCol",
        "Reconstructed Cluster collections","");
    m_builder->addAttDef("# Clusters", "number of clusters in Col", "Physics", "");

    m_builder->addType("TkrClusterCol","TkrCluster","Reconstructed Cluster","");
    m_builder->addType("TkrCluster", "Strip", "Strip", "");
    m_builder->addType("Strip", "ActiveStrip", "Active Part of Strip", "");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","green","");
    m_builder->addAttDef("Sequence", "Position in TkrClusterCol", "Physics", "");
    m_builder->addAttDef("Tower","Cluster Tower #","Physics","");
    m_builder->addAttDef("Plane","Cluster Plane #","Physics","");
    m_builder->addAttDef("View","Cluster View","Physics","");
    m_builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    m_builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    m_builder->addAttDef("Position","Cluster Global Position","Physics","");
    m_builder->addAttDef("RawToT","Cluster Time over Threshold","Physics","");
    m_builder->addAttDef("Mips","ToT converted to Mips","Physics","");

    m_builder->addType("TkrCluster","TkrClusterToT"," ","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","red","");

    m_builder->addType("TkrCluster", "ClusterMarker", " ", "");
    m_builder->addAttValue("Color", "green", "");
    if(_scalingMarker) {
        // Tracy hates the fixed-size marker, try again!
        m_builder->addType("ClusterMarker", "MarkerArm", " ", "");
        m_builder->addAttValue("DrawAs", "Line", "");
        m_builder->addAttValue("LineWidth", _markerWidth, "");
    } else {
        //Here's the real marker 
        m_builder->addAttValue("DrawAs", "Point", "");
        m_builder->addAttValue("MarkerName", "Cross", "");
        m_builder->addAttValue("MarkerSize", "1", "");
    }
}

// This method fill the instance tree Event with the actual TDS content
void ClusterFiller::fillInstances (std::vector<std::string>& typesList)
{  
    if (!hasType(typesList,"Recon/TkrRecon/TkrClusterCol")) return;

    Event::TkrClusterCol* pClusters = 
        SmartDataPtr<Event::TkrClusterCol>(m_dpsvc,EventModel::TkrRecon::TkrClusterCol);

    if (!pClusters) return;

    int    nHits      = pClusters->size();
    m_builder->addInstance("TkrRecon","TkrClusterCol");
    m_builder->addAttValue("# Clusters", nHits, "");

    //Loop over all cluster hits in the SiClusters vector
    if (!hasType(typesList,"Recon/TkrRecon/TkrClusterCol/TkrCluster")) return;

    double markerOffset = m_siStripPitch;
    double markerSize   = _markerSize*m_siStripPitch;

    double halfLength   = 0.5 * (m_stripLength);

    int hit;
    for(hit=0; hit<nHits; ++hit) {
        {
            Event::TkrCluster* pCluster = (*pClusters)[hit];
            Point              clusPos  = pCluster->position();

            double halfWidth = 0.5 * pCluster->size() * m_siStripPitch;
            double x    = clusPos.x();
            double y    = clusPos.y();
            double z    = clusPos.z();
            double dz   = 0.5 * m_siThickness;

            double dx   = halfWidth;
            double dy   = halfLength;

            double xToT = x;
            double yToT = y - dy - markerOffset;
            double deltaY = 0.0;
            double deltaX = markerSize;

            int    view   = pCluster->getTkrId().getView();
            if (view == idents::TkrId::eMeasureY) {
                dy   = 0.5 * pCluster->size() * m_siStripPitch;
                dx   = 0.5 * m_stripLength;
                xToT = x - dx - markerOffset;
                yToT = y;
                deltaY = markerSize;
                deltaX = 0.0;
            }

            double ToT  = pCluster->getRawToT() / 64.;       // So, max ToT = 4 mm

            m_builder->addInstance("TkrClusterCol","TkrCluster");
            m_builder->addAttValue("Sequence",    hit, "");

            m_builder->addAttValue("Tower",       pCluster->tower(),"");
            m_builder->addAttValue("Plane",       (int)pCluster->getPlane(),"");
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

            m_builder->addAttValue("RawToT",         (float)(pCluster->getRawToT()),"");
            m_builder->addAttValue("Mips",           (float)(pCluster->getMips()),"");

            //Draw the width of the cluster

            //Now draw the hit strips
            m_builder->addInstance("TkrCluster", "Strip");
            double waferPitch = m_siWaferSide + m_ssdGap;
            double offset = -0.5*(m_nWaferAcross-1)*waferPitch;
            for (int wafer=0;wafer<4; ++wafer) {
                m_builder->addInstance("Strip", "ActiveStrip");
                double delta = offset + wafer*waferPitch;
                x  = clusPos.x();
                y  = clusPos.y() + delta;
                dx = halfWidth;
                dy = 0.5*m_siWaferActiveSide;
                if (view == idents::TkrId::eMeasureY) {
                    x = clusPos.x() + delta;
                    y = clusPos.y();
                    std::swap(dx, dy);
                }
                m_builder->addPoint(x+dx,y+dy,z+dz);
                m_builder->addPoint(x-dx,y+dy,z+dz);
                m_builder->addPoint(x-dx,y-dy,z+dz);
                m_builder->addPoint(x+dx,y-dy,z+dz);
                m_builder->addPoint(x+dx,y+dy,z-dz);
                m_builder->addPoint(x-dx,y+dy,z-dz);
                m_builder->addPoint(x-dx,y-dy,z-dz);
                m_builder->addPoint(x+dx,y-dy,z-dz);
            }

            if(_scalingMarker) {
                // make the cross in the measured view
                m_builder->addInstance("TkrCluster", "ClusterMarker");
                m_builder->addInstance("ClusterMarker", "MarkerArm");
                m_builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
                m_builder->addPoint(xToT-deltaX, yToT-deltaY, z-markerSize);  
                m_builder->addInstance("ClusterMarker", "MarkerArm");
                m_builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);
                m_builder->addPoint(xToT+deltaX, yToT+deltaY, z-markerSize);
            } else {
                // Here's the code for the real marker
                m_builder->addInstance("TkrCluster", "ClusterMarker");
                m_builder->addPoint(xToT, yToT, z);       
            }

            //Time over threshold add here
            if (hasType(typesList,
                "Recon/TkrRecon/TkrClusterCol/TkrCluster/TkrClusterToT"))
            {
                m_builder->addInstance("TkrCluster","TkrClusterToT");
                m_builder->addPoint(xToT,yToT,z);
                m_builder->addPoint(xToT,yToT,z+ToT);
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
