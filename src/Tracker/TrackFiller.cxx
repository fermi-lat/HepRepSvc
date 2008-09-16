#include "TrackFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrTrack.h"

//#include "CLHEP/Geometry/Transform3D.h"
//#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    float _markerSize = 25.0;
    float _markerWidth = 2.0;
    bool  _scalingMarker  = true;
    bool _drawDigisIfNoClusters = true;

    std::string getClusterColor(Event::TkrCluster* pCluster, bool isAcc) {
        if (isAcc) { return "red"; }
        else if (pCluster->isSet(Event::TkrCluster::maskGHOST)) { return "255,100,27"; } // orange
        else if (pCluster->isSet(Event::TkrCluster::maskSAMETRACK)) {return "yellow"; } 
        else {return "green"; }
    }

}

// Constructor
TrackFiller::TrackFiller(HepRepInitSvc* hrisvc,
                         IGlastDetSvc* gsvc,
                         IDataProviderSvc* dpsvc,
                         IParticlePropertySvc* ppsvc):
m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
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
void TrackFiller::buildTypes()
{
    m_builder->addType("TkrRecon","Tracks", "Reconstructed Tracks collections","");

    m_builder->addType("Tracks","Track","Reconstructed track collection","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","blue","");
    m_builder->addAttDef("TrackId","Track ID #","Physics","");
    m_builder->addAttDef("Start Volume","Track Volume Information","Physics","");
    m_builder->addAttDef("Status Low", "Track Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Track High Status Bits","Physics","");
    m_builder->addAttDef("Energy","Fit Track Energy","Physics","MeV");
    m_builder->addAttDef("Quality","Fit Track Quality","Physics","");
    m_builder->addAttDef("# Hits","Number of Hits on Track","Physics","");
    m_builder->addAttDef("Chi-Square(filter)","Fit Track Filtered Chi-Square",
        "Physics","");
    m_builder->addAttDef("Chi-Square(smooth)","Fit Track Smoothed Chi-Square",
        "Physics","");
    m_builder->addAttDef("# Deg of Free","Number of degrees of freedom in Chi-Square",
        "Physics","");
    m_builder->addAttDef("RMS resid","RMS Residual of track hits","Physics","");
    m_builder->addAttDef("Tkr-Cal RadLens","Fit Track Smoothed Chi-Square",
        "Physics","");
    m_builder->addAttDef("Kalman Theta MS","Fit Track Smoothed Chi-Square",
        "Physics","");
    m_builder->addAttDef("Kalman Energy","Fit Track Smoothed Chi-Square",
        "Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");

    m_builder->addType("Track","TkrTrackHit", "Track Hits", "");
    m_builder->addAttDef("Sequence #", "Sequence of Hit on Track", "Physics", "");
    m_builder->addAttDef("Hit Volume","Volume containing this hit","Physics","");
    m_builder->addAttDef("Hit Status Low","Hit Low Status Bits","Physics","");
    m_builder->addAttDef("Hit Status High","Hit High Status Bits","Physics","");
    m_builder->addAttDef("ClusterID","Cluster ID","Physics","");
    m_builder->addAttDef("Energy","Energy at Plane","Physics","MeV");
    m_builder->addAttDef("Projection","Plane Projection","Physics","");
    m_builder->addAttDef("RadLen","Radiation Lengths to Plane","Phsyics","");
    m_builder->addAttDef("ActDist","Active Distance","Physics","mm");
    m_builder->addAttDef("Measured","Hit Position","Physics","");
    m_builder->addAttDef("Filter Position","Filter Fit Position","Physics","");
    m_builder->addAttDef("Filter Slope","Filter Fit Slope","Phyiscs","");
    m_builder->addAttDef("Smooth Position","Smooth Fit Position","Physics","");
    m_builder->addAttDef("Smooth Slope","Smooth Fit Slope","Phyiscs","");
    m_builder->addAttDef("ChiSquareFilter","Filter Chi Square Contribution",
        "Physics","");
    m_builder->addAttDef("ChiSquareSmooth","Smoother Chi Square Contribution",
        "Physics","");

    m_builder->addType("TkrTrackHit","NoCluster","No Cluster","");

    m_builder->addType("TkrTrackHit","TkrCluster","Reconstructed Cluster","");    
    m_builder->addType("TkrCluster", "Strip", "Strip", "");
    m_builder->addType("Strip", "ActiveStrip", "Active Part of Strip", "");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttDef("Tower","Cluster Tower #","Physics","");
    m_builder->addAttDef("Plane","Cluster Plane #","Physics","");
    m_builder->addAttDef("View","Cluster View","Physics","");
    m_builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    m_builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    m_builder->addAttDef("Position","Cluster Global Position","Physics","");
    m_builder->addAttDef("RawToT","Cluster Time over Threshold","Physics","");
    m_builder->addAttDef("Mips","ToT converted to Mips","Physics","");
    m_builder->addType("TkrCluster", "ClusterMarker", " ", "");

    m_builder->addType("TkrCluster","TkrClusterToT"," ","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","red","");

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
void TrackFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList,"Recon/TkrRecon/Tracks/Track")) return;

    Event::TkrTrackCol* pTracks = 
        SmartDataPtr<Event::TkrTrackCol>(m_dpsvc,
        EventModel::TkrRecon::TkrTrackCol);

    Event::TkrClusterCol* pClusters = 
        SmartDataPtr<Event::TkrClusterCol>(m_dpsvc,
        EventModel::TkrRecon::TkrClusterCol);

    //Now see if we can do the drawing
    if (!pTracks) return;

    m_builder->addInstance("TkrRecon","Tracks");

    int numTracks = pTracks->size();

    if (numTracks==0) return;

    int trackId  = 0;
    int trackWid = 2.0;
    //Event::TkrTrackCol::const_iterator it = pTracks->begin();
    Event::TkrTrackCol::iterator it = pTracks->begin();

    while(it != pTracks->end())
    {
        m_builder->addInstance("Tracks","Track");
        //const Event::TkrTrack& track = **it++;
        Event::TkrTrack& track = **it++;

        Event::TkrTrackHit::ParamType fit = Event::TkrTrackHit::SMOOTHED;
        Event::TkrTrackHit::ParamType typ = Event::TkrTrackHit::SMOOTHED;

        if (trackId > 1) trackWid = 1.0;

        m_builder->addAttValue("TrackId",            trackId++, "");
        m_builder->addAttValue("LineWidth",          (float)trackWid, "");
        idents::TkrId id = track[0]->getTkrId();
        std::string str = getTkrIdString(id);

        m_builder->addAttValue("Start Volume",       str, "");
        m_builder->addAttValue("Quality",            (float)(track.getQuality()),          "");
        m_builder->addAttValue("Energy",             (float)(track.getInitialEnergy()),    "");
        m_builder->addAttValue("# Hits",             (int)(track.size()),                  "");

        m_builder->addAttValue("Chi-square(filter)", (float)(track.getChiSquareFilter()),  "");
        m_builder->addAttValue("Chi-Square(smooth)", (float)(track.getChiSquareSmooth()),  "");
        m_builder->addAttValue("# Deg of Free",      (int)(track.getNDegreesOfFreedom()),  "");
        m_builder->addAttValue("RMS resid",          (float)(track.getScatter()),          "");
        m_builder->addAttValue("Tkr-Cal RadLens",    (float)(track.getTkrCalRadlen()),     "");
        m_builder->addAttValue("Kalman Theta MS",    (float)(track.getKalThetaMS()),       "");
        m_builder->addAttValue("Kalman Energy",      (float)(track.getKalEnergy()),        "");
        m_builder->addAttValue("Start Position",     
            getPositionString(track.getInitialPosition()), "");
        m_builder->addAttValue("Start Direction",    
            getDirectionString(track.getInitialDirection()), "");

        //Build strings for status bits
        unsigned int statBits = track.getStatusBits();
        m_builder->addAttValue("Status Low",getBits(statBits, 15, 0),"");

        std::stringstream outString;
        m_builder->addAttValue("Status High",getBits(statBits, 31, 16),"");
        //first loop through to draw the track
        Event::TkrTrackHitVecItr hitIter = track.begin();
        Event::TkrTrackHitVecItr lastHit  = --track.end();
        for(; hitIter!=lastHit; ++hitIter)
        {
            Event::TkrTrackHit& plane = **hitIter;
            Point planePos = plane.getPoint(fit);
            m_builder->addPoint(planePos.x(),planePos.y(),planePos.z());
            //if(hitIter!=track.begin()) {
            //    Point* startPoint = new Point(planePos);
            //    startPos.push_back(startPoint);
            //}

            Vector planeDir = plane.getDirection(fit);
            Event::TkrTrackHit& nextPlane = **(hitIter+1);
            Point nextPos = nextPlane.getPoint(fit);
            double deltaZ = planePos.z() - nextPos.z();
            double dist = fabs(deltaZ/planeDir.z());
            Point nextPoint = planePos + dist*planeDir;
            m_builder->addPoint(nextPoint.x(),nextPoint.y(),nextPoint.z());

            //nextPoint -= 0.6*planeDir/fabs(planeDir.z());
            //Point* endPoint = new Point(nextPoint);
            //endPos.push_back(endPoint);
            //int size = endPos.size();
            //m_builder->addPoint(nextPoint.x(),nextPoint.y(),nextPoint.z());
        }

        //Second loop through to draw the hits
        hitIter = track.begin();
        lastHit  = --track.end();
        int hit = 0;

        double markerOffset = m_siStripPitch;
        double markerSize   = _markerSize*m_siStripPitch;

        double halfLength   = 0.5 * (m_stripLength);

        for(hitIter=track.begin(); hitIter!=track.end(); ++hitIter, ++hit)
        {
            m_builder->addInstance("Track","TkrTrackHit");
            m_builder->addAttValue("LineWidth", (float)trackWid, "");

            Event::TkrTrackHit& plane = **hitIter;

            double x0, y0, z0, xl, xr, yl, yr;
            idents::TkrId tkrId = plane.getTkrId();
            x0 = plane.getTrackParams(typ).getxPosition();
            y0 = plane.getTrackParams(fit).getyPosition(); 
            z0 = plane.getZPlane()+0.1;
            if (tkrId.hasView()) {
                double delta= 10. * plane.getChiSquareSmooth(); //Scale factor! We're in mm now!
                if (plane.getTkrId().getView() == idents::TkrId::eMeasureX)
                {
                    xl = x0-0.5*delta;
                    xr = x0+0.5*delta;
                    yl = y0;
                    yr = y0;
                } 
                else 
                {
                    xl = x0;
                    xr = x0;
                    yl = y0-0.5*delta;
                    yr = y0+0.5*delta;
                }       
                m_builder->addPoint(xl,yl,z0);
                m_builder->addPoint(xr,yr,z0);
            } else {
                m_builder->addPoint(x0, y0, z0);
            }
            m_builder->addAttValue("Sequence #", hit, "");
            m_builder->addAttValue("Hit Volume", 
                getTkrIdString(plane.getTkrId()), "");

            //Build string for the measuring view
            std::stringstream hitView("No valid hit found");
            //int clusterId = -1;

            bool hitOnFit = ((plane.getStatusBits() & Event::TkrTrackHit::HITONFIT)!=0);
            if(hitOnFit) 
            {
                if (plane.getTkrId().getView() == idents::TkrId::eMeasureX) { 
                    hitView << "This is an X measuring plane";
                } else {                                                     
                    hitView << "This is a Y measuring plane";
                }
                //clusterId = -1; //plane.getClusterPtr()->id();
            }

            m_builder->addAttValue("Projection",hitView.str(),"");
            //m_builder->addAttValue("ClusterID",clusterId,"");
            m_builder->addAttValue("Energy",(float)(plane.getEnergy()),"");
            m_builder->addAttValue("RadLen",(float)(plane.getRadLen()),"");
            m_builder->addAttValue("ActDist",(float)(plane.getActiveDist()),"");
            if (hitOnFit) {
                m_builder->addAttValue("ChiSquareFilter",
                    (float)(plane.getChiSquareFilter()),"");
                m_builder->addAttValue("ChiSquareSmooth",
                    (float)(plane.getChiSquareSmooth()),"");
            }
            m_builder->addAttValue("Measured",
                getPositionString(plane.getPoint(Event::TkrTrackHit::MEASURED)),"");
            m_builder->addAttValue("Filter Position", 
                getPositionString(plane.getPoint(Event::TkrTrackHit::FILTERED)),"");
            m_builder->addAttValue("Filter Slope",    
                getSlopeString(plane.getTrackParams(Event::TkrTrackHit::FILTERED)),"");
            m_builder->addAttValue("Smooth Position", 
                getPositionString(plane.getPoint(Event::TkrTrackHit::SMOOTHED)),"");
            m_builder->addAttValue("Smooth Slope",    
                getSlopeString(plane.getTrackParams(Event::TkrTrackHit::SMOOTHED)),"");

            //Build string for status bits
            unsigned int      statBits = plane.getStatusBits();
            m_builder->addAttValue("Hit Status Low",getBits(statBits, 15, 0),"");
            m_builder->addAttValue("Hit Status High",getBits(statBits, 31, 16),"");

            Event::TkrClusterPtr pCluster = plane.getClusterPtr();

            if(pCluster) {
                double markerOffset = m_siStripPitch;
                double markerSize   = _markerSize*m_siStripPitch;

                double halfLength   = 0.5 * (m_stripLength);

                if(!pCluster->hitFlagged()) continue;
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

                m_builder->addInstance("TkrTrackHit","TkrCluster");
                m_builder->addAttValue("Sequence",    hit, "");

                m_builder->addAttValue("Tower",       pCluster->tower(),"");
                m_builder->addAttValue("Plane",       (int)pCluster->getPlane(),"");
                m_builder->addAttValue("View",        view,"");
                m_builder->addAttValue("First Strip", pCluster->firstStrip(),"");
                m_builder->addAttValue("Last Strip",  pCluster->lastStrip(),"");

                /*
                //Build string for cluster position
                std::stringstream clusterPosition;
                clusterPosition.setf(std::ios::fixed);
                clusterPosition.precision(3);
                clusterPosition << " (" << pCluster->position().x()
                << ","  << pCluster->position().y() 
                << ","  << pCluster->position().z() << ")";
                */
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
                double waferPitch = m_siWaferSide + m_ssdGap;
                double offset = -0.5*(m_nWaferAcross-1)*waferPitch;
                for (int wafer=0;wafer<m_nWaferAcross; ++wafer) {
                    m_builder->addInstance("Strip", "ActiveStrip");
                    if(isAcc) m_builder->addAttValue("LineStyle","Dashed","");
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
                    "Recon/TkrRecon/Tracks/Track/TkrTrackHit/TkrCluster/TkrClusterToT"))
                {
                    m_builder->addInstance("TkrCluster","TkrClusterToT");
                    m_builder->addPoint(xToT,yToT,z);
                    m_builder->addPoint(xToT,yToT,z+ToT);
                }
            }
        }    
    }
}

std::string TrackFiller::getTkrIdString(const idents::TkrId& tkrId)
{
    std::stringstream tkrIdStream;
    //tkrIdStream.setf(std::ios::fixed);

    if(!tkrId.hasTowerX() || !tkrId.hasTowerY() || !tkrId.hasTray() || !tkrId.hasBotTop()) {
        return "TkrId not valid";
    }

    //int tower   = 4 * tkrId.getTowerX() + tkrId.getTowerY();
    int xTower = tkrId.getTowerX();
    int yTower = tkrId.getTowerY();
    int tower = idents::TowerId(xTower, yTower).id();
    int trayNum = tkrId.getTray();

    tkrIdStream << " Tower " << tower << ", tray " << trayNum;

    if (tkrId.getBotTop()==idents::TkrId::eTKRSiTop) {tkrIdStream << " top";}
    else                                             {tkrIdStream << " bot";}

    return tkrIdStream.str();
}

std::string TrackFiller::getSlopeString(const Event::TkrTrackParams& params)
{
    std::stringstream trkSlopes;

    trkSlopes.setf(std::ios::fixed);
    trkSlopes.precision(5);
    trkSlopes << "  mx=" << params.getxSlope()
        << ", my=" << params.getySlope(); 

    return trkSlopes.str();
}
