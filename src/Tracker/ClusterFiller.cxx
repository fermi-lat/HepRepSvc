#include "ClusterFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
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

namespace {
    float _markerSize = 25.0;
    float _markerWidth = 2.0;
    bool  _scalingMarker  = true;
    bool _drawDigisIfNoClusters = true;

    std::string getClusterColor(Event::TkrCluster* pCluster, bool isAcc) {
        if (isAcc) { return "red"; }
        else if (pCluster->isSet(Event::TkrCluster::maskGHOST)) { 
            return "255,100,27";  // orange
        }
        else if (pCluster->isSet(Event::TkrCluster::maskSAMETRACK)) {
            return "yellow"; 
        } 
        else { return "green"; }
    }
}

// Constructor
ClusterFiller::ClusterFiller(HepRepInitSvc* hrisvc,
                             IGlastDetSvc* gsvc,
                             ITkrGeometrySvc* tgsvc,
                             IDataProviderSvc* dpsvc,
                             IParticlePropertySvc* ppsvc):
m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc), m_tgsvc(tgsvc)
{
    int    ladderNStrips;
    double ladderGap;

    gsvc->getNumericConstByName("SiWaferActiveSide", &m_siWaferActiveSide);
    gsvc->getNumericConstByName("stripPerWafer",     &ladderNStrips);
    gsvc->getNumericConstByName("SiThick",           &m_siThickness);
    gsvc->getNumericConstByName("SiWaferSide",       &m_siWaferSide);
    gsvc->getNumericConstByName("ssdGap",            &m_ssdGap);
    gsvc->getNumericConstByName("ladderGap",         &ladderGap);
    gsvc->getNumericConstByName("nWaferAcross",      &m_nWaferAcross);
    gsvc->getNumericConstByName("xNum",              &m_numXTowers);
    gsvc->getNumericConstByName("yNum",              &m_numYTowers);
    gsvc->getNumericConstByName("towerPitch",        &m_towerPitch);

    double deadWidth = m_siWaferSide - m_siWaferActiveSide;
    m_stripLength = m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*m_ssdGap - deadWidth;
    m_activeWidth = m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*ladderGap - deadWidth;
    m_siStripPitch = m_siWaferActiveSide / ladderNStrips;
}

// This method build the types for the HepRep
void ClusterFiller::buildTypes()
{  
    m_builder->addType("TkrRecon","TkrClusterCol",
        "Reconstructed Cluster collections","");
    m_builder->addAttDef("# Total Clusters", "number of clusters in Col", "Physics", "");
    m_builder->addAttDef("# Clusters not Used", "number of clusters not used on tracks", "Physics", "");

    m_builder->addType("TkrClusterCol","TkrCluster","Reconstructed Cluster","");
    m_builder->addType("TkrCluster", "Strip", "Strip", "");
    m_builder->addType("Strip", "ActiveStrip", "Active Part of Strip", "");
    m_builder->addAttValue("DrawAs","Prism","");
    //m_builder->addAttValue("Color","green","");
    m_builder->addAttDef("Sequence", "Position in TkrClusterCol", "Physics", "");
    m_builder->addAttDef("Tower","Cluster Tower #","Physics","");
    m_builder->addAttDef("Plane","Cluster Plane #","Physics","");
    m_builder->addAttDef("View","Cluster View","Physics","");
    m_builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    m_builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    m_builder->addAttDef("Status", "Cluster Low Status Bits","Physics","");
    m_builder->addAttDef("Position","Cluster Global Position","Physics","");
    m_builder->addAttDef("RawToT","Cluster Time over Threshold","Physics","");
    m_builder->addAttDef("Mips","ToT converted to Mips","Physics","");

    m_builder->addType("TkrCluster","TkrClusterToT"," ","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","red","");

    m_builder->addType("TkrCluster", "ClusterMarker", " ", "");
    //m_builder->addAttValue("Color", "green", "");
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
            //std::cout << "hit/flg/twr/pln/v/1st/lst " << hit << ": " 
            //    << pCluster->hitFlagged() << ", " 
            //    << pCluster->tower() << " " << pCluster->getPlane() << " " 
            //    << pCluster->getTkrId().getView() << "; " 
            //    << pCluster->firstStrip() << " " << pCluster->lastStrip() << std::endl;
        }

        m_builder->setSubinstancesNumber("TkrClusterCol",nNotUsed);

        m_builder->addAttValue("# Total Clusters", nHits, "");
        m_builder->addAttValue("# Clusters not used", nNotUsed, "");

        //Loop over all cluster hits in the TkrClusterCol vector

        double markerOffset = m_siStripPitch;
        double markerSize   = _markerSize*m_siStripPitch;

        double halfLength   = 0.5 * (m_stripLength);

        for(hit=0; hit<nHits; ++hit) {
            {
                Event::TkrCluster* pCluster = (*pClusters)[hit];
                if(pCluster->hitFlagged()) continue;
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

                unsigned int status = pCluster->getStatusWord();
                m_builder->addAttValue("Status",getBits(status, 15, 0),"");
                m_builder->addAttValue("Position",
                    getPositionString(pCluster->position()),"");

                int rawToT = pCluster->getRawToT();
                m_builder->addAttValue("RawToT",         (float) rawToT,"");
                m_builder->addAttValue("Mips",           (float)(pCluster->getMips()),"");

                //Draw the width of the cluster

                //Now draw the hit strips
                // check for ToT==255
                bool isAcc = (rawToT==255);
                std::string clusterColor = getClusterColor(pCluster,isAcc);

                m_builder->addInstance("TkrCluster", "Strip");
                m_builder->setSubinstancesNumber("Strip",m_nWaferAcross);
                double waferPitch = m_siWaferSide + m_ssdGap;
                double offset = -0.5*(m_nWaferAcross-1)*waferPitch;
                for (int wafer=0;wafer<m_nWaferAcross; ++wafer) {
                    m_builder->addInstance("Strip", "ActiveStrip");
                    if(clusterColor!="green") m_builder->addAttValue("LineStyle","Dashed","");
                    m_builder->addAttValue("Color", clusterColor, "");
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
                    drawPrism(x, y, z, dx, dy, dz);
                }

                // quick and dirty wide cluster display.
                bool isWide = (pCluster->size()>4)&&m_hrisvc->getClusterFiller_showWide();
                if(_scalingMarker) {
                    // make the cross in the measured view
                    m_builder->addInstance("TkrCluster", "ClusterMarker");
                    m_builder->setSubinstancesNumber("ClusterMarker",2);
                    m_builder->addInstance("ClusterMarker", "MarkerArm");
                    if(isAcc) m_builder->addAttValue("LineStyle","Dashed","");
                    m_builder->addAttValue("Color", clusterColor, "");
                    m_builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
                    m_builder->addPoint(xToT-deltaX, yToT-deltaY, z-markerSize);
                    if(isWide) m_builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);

                    m_builder->addInstance("ClusterMarker", "MarkerArm");
                    if(isAcc) m_builder->addAttValue("LineStyle","Dashed","");
                    m_builder->addAttValue("Color", clusterColor, "");
                    m_builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);
                    m_builder->addPoint(xToT+deltaX, yToT+deltaY, z-markerSize);
                    if(isWide) m_builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
                } else {
                    // Here's the code for the real marker
                    m_builder->addInstance("TkrCluster", "ClusterMarker");

                    if(isAcc) m_builder->addAttValue("LineStyle","Dashed","");
                    m_builder->addAttValue("Color", clusterColor, "");
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

            if(_drawDigisIfNoClusters) { 
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
            const Event::intVector& stripNumber = trunc.getStripNumber();
            const Event::floatVector& localX = trunc.getLocalX();
            Event::SortId id = iter->first;
            int tower = id.getTower();
            int tray  = id.getTray();
            int face  = id.getFace();
            int view  = id.getView();

            /* ====>
            std::cout 
            << "Tower/Tray/face " <<tower << "/" << tray << "/" << face 
            << ", status " << status 
            << ", #Strips " << numStrips[0] << "/" << numStrips[1]
            << ", # " << stripNumber[0]  << "/" << stripNumber[1] 
            << "/" << stripNumber[2]
            << ", X " << localX[0]  << "/" << localX[1]
            << "/" << localX[2]
            << std::endl;
            */


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
