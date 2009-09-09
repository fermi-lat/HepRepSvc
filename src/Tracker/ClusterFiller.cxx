#include "ClusterFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
//#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "TkrUtil/ITkrGeometrySvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrVertex.h"
#include "Event/Recon/TkrRecon/TkrTruncationInfo.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

// Constructor
ClusterFiller::ClusterFiller(HepRepInitSvc* hrisvc,
                             IGlastDetSvc* gsvc,
                             ITkrGeometrySvc* tgsvc,
                             IDataProviderSvc* dpsvc,
                             IParticlePropertySvc* ppsvc) :
ClusterUtil(hrisvc, gsvc), m_tgsvc(tgsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{    

    m_drawDigisIfNoClusters = true;
}

// This method build the types for the HepRep
void ClusterFiller::buildTypes()
{  
    m_builder->addType("TkrRecon","TkrClusterCol",
        "Reconstructed Cluster collections","");
    m_builder->addAttDef("# Total Clusters", "number of clusters in Col", "Physics", "");
    m_builder->addAttDef("# Clusters not Used", "number of clusters not used on tracks", "Physics", "");

    m_builder->addType("TkrClusterCol","TkrCluster","Reconstructed Cluster","");

    buildClusterTypes(m_builder);

    // add digi stuff
    m_builder->addType("TkrRecon","TkrDigiCol",
        "Digis (plotted if no clusters","");
    m_builder->addAttDef("# Digis", "number of digis in Col", "Physics", "");
    m_builder->addAttDef("Total Strips", "total number of hit strips", "Physics", "");

    m_builder->addType("TkrDigiCol","TkrXDigiCol","Digi","");
    m_builder->addAttDef("Total X Digis", "# X Digis", "Physics", "");
    m_builder->addAttDef("Total X Strips", "x-measuring strips", "Physics", "");
    m_builder->addType("TkrXDigiCol","TkrDigi","Digi","");

    m_builder->addAttDef("DigiSequence", "Position in DigiCol", "Physics", "");
    m_builder->addAttDef("DigiTower","Digi Tower #","Physics","");
    m_builder->addAttDef("DigiPlane","Digi Plane #","Physics","");
    m_builder->addAttDef("DigiView","Digi View","Physics","");
    m_builder->addAttDef("# strips", "# strips", "Physics", "");
    m_builder->addAttDef("StripList", "list of hit strips", "Physics", "");

    //m_builder->addType("TkrDigi", "DigiStrip", "Strip", "");
    //m_builder->addAttValue("DrawAs","Prism","");
    //m_builder->addAttValue("Color","yellow","");

    m_builder->addType("TkrDigiCol","TkrYDigiCol","Digi","");
    m_builder->addAttDef("Total Y Digis", "# Y Digis", "Physics", "");
    m_builder->addAttDef("Total Y Strips", "y-measuring strips", "Physics", "");
    m_builder->addType("TkrYDigiCol","TkrDigi","Digi","");

    m_builder->addAttDef("DigiSequence", "Position in DigiCol", "Physics", "");
    m_builder->addAttDef("DigiTower","Digi Tower #","Physics","");
    m_builder->addAttDef("DigiPlane","Digi Plane #","Physics","");
    m_builder->addAttDef("DigiView","Digi View","Physics","");
    m_builder->addAttDef("# strips", "# strips", "Physics", "");
    m_builder->addAttDef("StripList", "list of hit strips", "Physics", "");

    m_builder->addType("TkrDigi", "DigiStrip", "Strip", "");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","yellow","");
    m_builder->addAttValue("LineStyle","Dashed","");

    //add in truncation ranges... these block out the truncated portions of the plane
    m_builder->addType("TkrRecon", "TruncationMap", "map of TruncationRanges", "");
    m_builder->addType("TruncationMap", "PlaneInfo", "Plane Info", "");
    m_builder->addAttValue("DrawAs", "Prism","");
    m_builder->addAttValue("Color", "gray", "");
}

// This method fill the instance tree Event with the actual TDS content
void ClusterFiller::fillInstances (std::vector<std::string>& typesList)
{  

    Event::TkrClusterCol* pClusters = 
        SmartDataPtr<Event::TkrClusterCol>(m_dpsvc,EventModel::TkrRecon::TkrClusterCol);

    bool doClusters = hasType(typesList,"Recon/TkrRecon/TkrClusterCol/TkrCluster");

    bool doDigis = 
        hasType(typesList,"Recon/TkrRecon/TkrDigiCol/TkrXDigiCol/TkrDigi")
        && hasType(typesList,"Recon/TkrRecon/TkrDigiCol/TkrYDigiCol/TkrDigi");

    Event::TkrDigiCol* pDigis = 
        SmartDataPtr<Event::TkrDigiCol>(m_dpsvc,EventModel::Digi::TkrDigiCol);

    if (pClusters && !pClusters->empty() && doClusters) {
        int    nHits      = pClusters->size();

        m_builder->addInstance("TkrRecon","TkrClusterCol");

        int    nNotUsed = 0;
        int hit;
        for(hit=0; hit<nHits; ++hit) {
            Event::TkrCluster* pCluster = (*pClusters)[hit];
            if(!pCluster->hitFlagged()) nNotUsed++;
        }

        m_builder->setSubinstancesNumber("TkrClusterCol",nNotUsed);

        m_builder->addAttValue("# Total Clusters", nHits, "");
        m_builder->addAttValue("# Clusters not used", nNotUsed, "");

        //Loop over all cluster hits in the TkrClusterCol vector

        m_hasTypeToT =  hasType(typesList,
            "Recon/TkrRecon/TkrClusterCol/TkrCluster/TkrClusterToT");


        for(hit=0; hit<nHits; ++hit) {
            {
                Event::TkrCluster* pCluster = (*pClusters)[hit];
                if(pCluster->hitFlagged()) continue;
                m_builder->addInstance("TkrClusterCol","TkrCluster");
                m_builder->addAttValue("Sequence",    hit, "");

                buildClusterInstance(m_builder, pCluster);
            }
        }
    } else if (pDigis && !pDigis->empty() && doDigis){
        // if there aren't any clusters, use the digis instead.
        // These will be drawn in yellow


        int nDigis = pDigis->size();
        m_builder->addInstance("TkrRecon","TkrDigiCol");
        m_builder->addAttValue("# Digis", nDigis, "" );

        double dx = 0.5*m_siStripPitch;
        double dy = 0.5*m_stripLength;
        double dz   = m_siThickness;

        //Loop over all digis in the TkrDigiCol vector

        int totHits = 0;
        int totXHits = 0;
        int totYHits = 0;
        int xDigis = 0;
        int yDigis = 0;
        int nHits;
        int iDigi;
        for (iDigi=0;iDigi<nDigis;++iDigi) {
            Event::TkrDigi* pDigi = (*pDigis)[iDigi];
            nHits = pDigi->getNumHits();
            totHits += nHits;
            if(pDigi->getView()==0) {
                totXHits += nHits;
                if (nHits>0) xDigis++;
            } else {
                totYHits += nHits;
                if(nHits>0) yDigis++;
            }
        }
        m_builder->addAttValue("Total Strips", totHits, "");

        if(totXHits>0) {
            m_builder->addInstance("TkrDigiCol", "TkrXDigiCol");
            m_builder->setSubinstancesNumber("TkrXDigiCol", xDigis);
            m_builder->addAttValue("Total X Digis", xDigis, "");
            m_builder->addAttValue("Total X Strips", totXHits, "");
        }
        if(totYHits>0) {
            m_builder->addInstance("TkrDigiCol", "TkrYDigiCol");       
            m_builder->setSubinstancesNumber("TkrYDigiCol", yDigis);
            m_builder->addAttValue("Total Y Digis", yDigis, "");
            m_builder->addAttValue("Total Y Strips", totYHits, "");
        }

        for (iDigi=0;iDigi<nDigis;++iDigi) {
            Event::TkrDigi* pDigi = (*pDigis)[iDigi];

            int view  = pDigi->getView();
            if(view==0) {m_builder->addInstance("TkrXDigiCol","TkrDigi");}
            else        {m_builder->addInstance("TkrYDigiCol","TkrDigi");}
            m_builder->addAttValue("DigiSequence",    iDigi, "");
            int tower = (pDigi->getTower()).id();
            int layer = pDigi->getBilayer();

            m_builder->addAttValue("DigiTower", tower, "");
            m_builder->addAttValue("DigiLayer", layer, "");
            m_builder->addAttValue("DigiView",  view, "");
            m_builder->addAttValue("ToT[0]",    pDigi->getToT(0),"");
            m_builder->addAttValue("ToT[1]",    pDigi->getToT(1),"");
            int nHits = pDigi->getNumHits();
            m_builder->addAttValue("# strips",  nHits, "");

            int iHit;
            std::stringstream stripList;
            for(iHit=0;iHit<nHits;++iHit) {
                int strip = pDigi->getHit(iHit);
                stripList << strip ;
                if (iHit<nHits-1) stripList << ", ";
            }
            m_builder->addAttValue("StripList", stripList.str(), "");

            if(m_drawDigisIfNoClusters) { 
                m_builder->setSubinstancesNumber("TkrDigi",nHits);
                for(iHit=0;iHit<nHits;++iHit) {
                    int strip = pDigi->getHit(iHit);
                    m_builder->addInstance("TkrDigi", "DigiStrip");

                    // Draw the strip
                    HepPoint3D digiPos = 
                        m_tgsvc->getStripPosition(tower, layer, view, strip);

                    double x  = digiPos.x();
                    double y  = digiPos.y();
                    double z  = digiPos.z();
                    double dx1 = dx;
                    double dy1 = dy;

                    if (view == idents::TkrId::eMeasureY) {
                        std::swap(dx1, dy1);
                    }
                    drawPrism(x, y, z, dx1, dy1, dz);
                }
            }
        }
    }

    // display truncated regions, if any
    SmartDataPtr<Event::TkrTruncationInfo> truncInfo( m_dpsvc, EventModel::TkrRecon::TkrTruncationInfo );
    if (truncInfo!=0 && truncInfo->isTruncated()) {
        Event::TkrTruncationInfo::TkrTruncationMap*  truncMap = truncInfo->getTruncationMap();
        Event::TkrTruncationInfo::TkrTruncationMap::const_iterator iter = truncMap->begin();
        m_builder->addInstance("TkrRecon", "TruncationMap");
        int numTrunc = 0;
        for(; iter!=truncMap->end(); ++iter) {
            Event::TkrTruncatedPlane trunc = iter->second;
            const int status   = trunc.getStatus();
            if (status!=0) numTrunc++;
        }
        m_builder->setSubinstancesNumber("TruncationMap",numTrunc);

        for(; iter!=truncMap->end(); ++iter) {
            Event::TkrTruncatedPlane trunc = iter->second;
            const int status   = trunc.getStatus();
            if (status==0) continue;
            const Event::intVector& numStrips = trunc.getStripCount();
            //const Event::intVector& stripNumber = trunc.getStripNumber();
            const Event::floatVector& localX = trunc.getLocalX();
            Event::SortId id = iter->first;
            int tower = id.getTower();
            //int tray  = id.getTray();
            //int face  = id.getFace();
            int view  = id.getView();

            // need x,y limits and z-coordinate

            double z = trunc.getPlaneZ();
            double dz = 0.5*m_siThickness;
            idents::TowerId towerId(tower);
            int xTower = towerId.ix();
            int yTower = towerId.iy();
            double offsetX = m_towerPitch*(xTower-0.5*(m_numXTowers-1));
            double offsetY = m_towerPitch*(yTower-0.5*(m_numYTowers-1));

            double xLow  = localX[3];
            double xHigh = localX[3];
            if((status&Event::TkrTruncatedPlane::END0SET)>0) xLow  = localX[0];
            if((status&Event::TkrTruncatedPlane::RC1SET)>0) xHigh = localX[1];
            if((status&Event::TkrTruncatedPlane::CC1SET)>0 && numStrips[1]==0) xHigh = localX[2];

            double x, y, dx, dy;

            if(xHigh>xLow) {
                double x  = 0.5*(xLow+xHigh);
                double dx = 0.5*(xHigh-xLow);
                double y  = 0.0;
                double dy = 0.5*m_stripLength;
                if(view==idents::TkrId::eMeasureY){
                    std::swap(x,y);
                    std::swap(dx, dy);
                }
                x += offsetX;
                y += offsetY;

                m_builder->addInstance("TruncationMap", "PlaneInfo");
                drawPrism(x, y, z, dx, dy, dz);
            }
            if((status&Event::TkrTruncatedPlane::CC1SET)>0 && numStrips[1]>0){
                xLow  = localX[2];
                xHigh = 0.5*m_activeWidth;
                x  = 0.5*(xLow+xHigh);
                dx = 0.5*(xHigh-xLow);
                y  = 0.0;
                dy = 0.5*m_stripLength;
                if(view==idents::TkrId::eMeasureY){
                    std::swap(x,y);
                    std::swap(dx, dy);
                }
                x += offsetX;
                y += offsetY;

                m_builder->addInstance("TruncationMap", "PlaneInfo");
                drawPrism(x, y, z, dx, dy, dz);
            }
        }
    }
}
