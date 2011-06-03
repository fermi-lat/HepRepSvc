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

#include "TkrUtil/ITkrTrackVecTool.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    const float _dH = 0.25; // size of TkrTrackHit box
    const float _hE = 0.1;  //halfheight of caps on hit "sigmas"
}

// Constructor
TrackFiller::TrackFiller(HepRepInitSvc* hrisvc,
                         IGlastDetSvc* gsvc,
                         IDataProviderSvc* dpsvc,
                         IParticlePropertySvc* ppsvc):
ClusterUtil(hrisvc, gsvc), m_dpsvc(dpsvc),m_ppsvc(ppsvc) {}

// This method build the types for the HepRep
void TrackFiller::buildTypes()
{
    // Build types for the standard and the CR tracks
    // call the same method to fill in the substructure
    m_builder->addType("TkrRecon","Tracks", "Reconstructed Tracks collection","");
    m_builder->addType("Tracks","Track","Reconstructed track","");
    buildTrack(m_builder);

    m_builder->addType("TkrRecon","CRTracks", "Reconstructed CRTracks collection","");
    m_builder->addType("CRTracks","Track","Reconstructed CRTrack","");
    buildTrack(m_builder);

}

void TrackFiller::buildTrack(IBuilder* builder) {
    builder->addAttValue("DrawAs","Line","");
    builder->addAttValue("Color","blue","");
    builder->addAttDef("TrackId","Track ID #","Physics","");
    builder->addAttDef("Start Volume","Track Volume Information","Physics","");
    builder->addAttDef("Status Low", "Track Low Status Bits","Physics","");
    builder->addAttDef("Status High","Track High Status Bits","Physics","");
    builder->addAttDef("Energy","Fit Track Energy","Physics","MeV");
    builder->addAttDef("Quality","Fit Track Quality","Physics","");
    builder->addAttDef("# Hits","Number of Hits on Track","Physics","");
    builder->addAttDef("Chi-Square(filter)","Fit Track Filtered Chi-Square",
        "Physics","");
    builder->addAttDef("Chi-Square(smooth)","Fit Track Smoothed Chi-Square",
        "Physics","");
    builder->addAttDef("# Deg of Free","Number of degrees of freedom in Chi-Square",
        "Physics","");
    builder->addAttDef("RMS resid","RMS Residual of track hits","Physics","");
    builder->addAttDef("Tkr-Cal RadLens","Fit Track Smoothed Chi-Square",
        "Physics","");
    builder->addAttDef("Kalman Theta MS","Fit Track Smoothed Chi-Square",
        "Physics","");
    builder->addAttDef("Kalman Energy","Fit Track Smoothed Chi-Square",
        "Physics","");
    builder->addAttDef("Start Position","Track start position","Physics","");
    builder->addAttDef("Start Direction","Track start direction","Physics","");

    builder->addType("Track","TkrTrackHit", "Track Hit", "");
    builder->addAttValue("DrawAs","Prism", "");
    builder->addAttDef("Sequence #", "Sequence of Hit on Track", "Physics", "");
    builder->addAttDef("Hit Volume","Volume containing this hit","Physics","");
    builder->addAttDef("Hit Status Low","Hit Low Status Bits","Physics","");
    builder->addAttDef("Hit Status High","Hit High Status Bits","Physics","");
    builder->addAttDef("ClusterID","Cluster ID","Physics","");
    builder->addAttDef("Energy","Energy at Plane","Physics","MeV");
    builder->addAttDef("Projection","Plane Projection","Physics","");
    builder->addAttDef("RadLen","Radiation Lengths to Plane","Phsyics","");
    builder->addAttDef("ActDist","Active Distance","Physics","mm");
    builder->addAttDef("Kink Angle","Kink Angle","Physics","");
    builder->addAttDef("Measured","Hit Position","Physics","");
    builder->addAttDef("Filter Position","Filter Fit Position","Physics","");
    builder->addAttDef("Filter Slope","Filter Fit Slope","Phyiscs","");
    builder->addAttDef("Smooth Position","Smooth Fit Position","Physics","");
    builder->addAttDef("Smooth Slope","Smooth Fit Slope","Phyiscs","");
    builder->addAttDef("ChiSquareFilter","Filter Chi Square Contribution",
        "Physics","");
    builder->addAttDef("ChiSquareSmooth","Smoother Chi Square Contribution",
        "Physics","");

    //builder->addType("TkrTrackHit","NoCluster","No Cluster","");
    builder->addType("TkrTrackHit","HitSigma","Hit Sigma", "");
    builder->addAttValue("DrawAs", "Line","");

    builder->addType("TkrTrackHit","TkrCluster","Reconstructed Cluster",""); 

    buildClusterTypes(builder);

}


// This method fill the instance tree Event with the actual TDS content
void TrackFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList,"Recon/TkrRecon/Tracks/Track")) return;

    //std::vector<Event::TkrTrackCol*> trackColVec;
    int numStTracks = 0;
    int numCRTracks = 0;
    std::string colName;

    int iCol;
    Event::TkrTrackCol* pTracks;

    // until we get the map of collections, we explicitly loop over
    // the 2 current track collections

    for(iCol=0; iCol<2; ++iCol) {
        if(iCol==0) {
            // First look up the normal track collection
            pTracks = SmartDataPtr<Event::TkrTrackCol>(m_dpsvc,
                EventModel::TkrRecon::TkrTrackCol);
            if(pTracks==0) continue;
            numStTracks = pTracks->size();
            if (numStTracks==0) continue;
            m_builder->addInstance("TkrRecon","Tracks");
            m_builder->setSubinstancesNumber("Tracks",numStTracks);
            colName = "Tracks";
        } else {
            // or the CR track collection
            pTracks = SmartDataPtr<Event::TkrTrackCol>(m_dpsvc,
                EventModel::TkrRecon::TkrCRTrackCol);
            if(pTracks==0) continue;
            numCRTracks = pTracks->size();
            if(numCRTracks==0) continue;
            m_builder->addInstance("TkrRecon","CRTracks");
            m_builder->setSubinstancesNumber("CRTracks",numCRTracks);
            colName = "CRTracks";
        }

        Event::TkrTrackCol::iterator it = pTracks->begin();

        int trackId  = 0;
        double trackWid = 2.0;

        while(it != pTracks->end()) {
            m_builder->addInstance(colName,"Track");
            Event::TkrTrack& track = **it++;
            bool isCR = (track.getStatusBits()&Event::TkrTrack::COSMICRAY)!=0;

            m_builder->addAttValue("Color",    (isCR ? "255,100,27" : "blue"), "");
            m_builder->addAttValue("LineStyle",(isCR ? "Dashed" : "Solid"), "");

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
            m_builder->addAttValue("Status",getBits(statBits, 15, 0),"");

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

                Vector planeDir = plane.getDirection(fit);
                Event::TkrTrackHit& nextPlane = **(hitIter+1);
                Point nextPos = nextPlane.getPoint(fit);
                double deltaZ = planePos.z() - nextPos.z();
                double dist = fabs(deltaZ/planeDir.z());
                Point nextPoint = planePos + dist*planeDir;
                m_builder->addPoint(nextPoint.x(),nextPoint.y(),nextPoint.z());
            }

            //Second loop through to draw the hits
            hitIter = track.begin();
            lastHit  = --track.end();
            int hit = 0;

            m_builder->setSubinstancesNumber("Track", track.size());

            for(hitIter=track.begin(); hitIter!=track.end(); ++hitIter, ++hit)
            {
                m_builder->addInstance("Track","TkrTrackHit");
                m_builder->addAttValue("LineWidth", (float)trackWid, "");

                Event::TkrTrackHit& plane = **hitIter;

                double x0, y0, z0;
                idents::TkrId tkrId = plane.getTkrId();
                x0 = plane.getTrackParams(typ).getxPosition();
                y0 = plane.getTrackParams(fit).getyPosition(); 
                z0 = plane.getZPlane();

                // this is the prism that we pick
                drawPrism(x0, y0, z0, _dH, _dH, _dH);

                m_builder->addAttValue("Sequence #", hit, "");
                m_builder->addAttValue("Hit Volume", 
                    getTkrIdString(plane.getTkrId()), "");

                //Build string for the measuring view
                std::stringstream hitView("No valid hit found");

                bool hitOnFit = ((plane.getStatusBits() & Event::TkrTrackHit::HITONFIT)!=0);
                if(hitOnFit) 
                {
                    if (plane.getTkrId().getView() == idents::TkrId::eMeasureX) { 
                        hitView << "This is an X measuring plane";
                    } else {                                                     
                        hitView << "This is a Y measuring plane";
                    }
                }

                m_builder->addAttValue("Projection",hitView.str(),"");
                //m_builder->addAttValue("ClusterID",clusterId,"");
                m_builder->addAttValue("Energy",(float)(plane.getEnergy()),"");
                m_builder->addAttValue("RadLen",(float)(plane.getRadLen()),"");
                m_builder->addAttValue("ActDist",(float)(plane.getActiveDist()),"");
                m_builder->addAttValue("Kink Angle", (float)(plane.getKinkAngle()),"");
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
                    //if(!pCluster->isSet(Event::TkrCluster::maskUSEDANY)) continue;

                    m_builder->addInstance("TkrTrackHit","TkrCluster");
                    m_builder->addAttValue("Sequence",    hit, "");

                    buildClusterInstance(m_builder, pCluster);
                }

                bool doSigma = true;
                if(doSigma) {

                    // now the "sigma"
                    m_builder->addInstance("TkrTrackHit", "HitSigma");
                    if (tkrId.hasView()) {
                        // try something like sqrt, but min and max it.
                        // with this choice, .01 < chisq < 1000.
                        double delta1=  std::max(0.01, plane.getChiSquareSmooth());
                        delta1 = sqrt(std::min(delta1, 1000.));
                        double delta2 = 0.0;

                        if( plane.getTkrId().getView() == idents::TkrId::eMeasureY) {
                            std::swap(delta1, delta2);
                        }

                        double xl, xr, yl, yr;
                        xl = x0 - delta1;
                        xr = x0 + delta1;
                        yl = y0 - delta2;
                        yr = y0 + delta2;

                        // this is all hidden inside the wafer...
                        //   so for experts only!
                        m_builder->addPoint(xl, yl, z0);
                        m_builder->addPoint(xl, yl, z0-_hE);
                        m_builder->addPoint(xr, yr, z0+_hE);
                        m_builder->addPoint(xr, yr, z0);
                    }
                }
            }
        }
    }

    return;
}
std::string TrackFiller::getTkrIdString(const idents::TkrId& tkrId)
{
    std::stringstream tkrIdStream;
    //tkrIdStream.setf(std::ios::fixed);

    if(!tkrId.hasTowerX() || !tkrId.hasTowerY() || !tkrId.hasTray() || !tkrId.hasBotTop()) {
        return "TkrId not valid";
    }

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
