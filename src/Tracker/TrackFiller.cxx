#include "TrackFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrTrack.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <sstream>
#include <iostream>
#include <algorithm>

// Constructor
TrackFiller::TrackFiller(IGlastDetSvc* gsvc,
                         IDataProviderSvc* dpsvc,
                         IParticlePropertySvc* ppsvc):
m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{


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
    m_builder->addAttDef("Chi-Square(filter)","Fit Track Filtered Chi-Square","Physics","");
    m_builder->addAttDef("Chi-Square(smooth)","Fit Track Smoothed Chi-Square","Physics","");
    m_builder->addAttDef("# Deg of Free","Number of degrees of freedom in Chi-Square","Physics","");
    m_builder->addAttDef("RMS resid","RMS Residual of track hits","Physics","");
    m_builder->addAttDef("Tkr-Cal RadLens","Fit Track Smoothed Chi-Square","Physics","");
    m_builder->addAttDef("Kalman Theta MS","Fit Track Smoothed Chi-Square","Physics","");
    m_builder->addAttDef("Kalman Energy","Fit Track Smoothed Chi-Square","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");

    m_builder->addType("Track","TkrTrackHit", "Track Hits", "");
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
    m_builder->addAttDef("ChiSquareFilter","Filter Chi Square Contribution","Physics","");
    m_builder->addAttDef("ChiSquareSmooth","Smoother Chi Square Contribution","Physics","");
}


// This method fill the instance tree Event with the actual TDS content
void TrackFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Recon/TkrRecon/Tracks/Track"))
    { 
        Event::TkrTrackCol* pTracks = 
            SmartDataPtr<Event::TkrTrackCol>(m_dpsvc,EventModel::TkrRecon::TkrTrackCol);

        //Now see if we can do the drawing
        if (pTracks)
        {
            m_builder->addInstance("TkrRecon","Tracks");

            int numTracks = pTracks->size();

            if (numTracks > 0) 
            {
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

                    m_builder->addAttValue("Start Volume",       getTkrIdString(track[0]->getTkrId()), "");
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
                    m_builder->addAttValue("Start Position",     getPositionString(track.getInitialPosition()), "");
                    m_builder->addAttValue("Start Direction",    getDirectionString(track.getInitialDirection()), "");

                    //Build strings for status bits
                    unsigned int statBits = track.getStatusBits();
                    m_builder->addAttValue("Status Low",getBits(statBits, 15, 0),"");

                    std::stringstream outString;
                    m_builder->addAttValue("Status High",getBits(statBits, 31, 16),"");
                    //first loop through to draw the track
                    for(Event::TkrTrackHitVecItr hitIter = track.begin(); hitIter < track.end(); hitIter++)
                    {
                        Event::TkrTrackHit& plane = **hitIter;
                        Point planePos = plane.getPoint(fit);

                        m_builder->addPoint(planePos.x(),planePos.y(),planePos.z());
                    }

                    //Second loop through to draw the hits
                    for(Event::TkrTrackHitVecItr hitIter = track.begin(); hitIter < track.end(); hitIter++)
                    {
                        m_builder->addInstance("Track","TkrTrackHit");
                        m_builder->addAttValue("LineWidth", (float)trackWid, "");

                        Event::TkrTrackHit& plane = **hitIter;

                        if (!(plane.getStatusBits() & Event::TkrTrackHit::HITONFIT)) continue;

                        double x0, y0, z0, xl, xr, yl, yr;
                        double delta= 10. * plane.getChiSquareSmooth(); //Scale factor! We're in mm now!

                        if (plane.getTkrId().getView() == idents::TkrId::eMeasureX)
                        {
                            x0 = plane.getTrackParams(typ).getxPosition();
                            y0 = plane.getTrackParams(fit).getyPosition(); 
                            z0 = plane.getZPlane()+0.1;
                            xl = x0-0.5*delta;
                            xr = x0+0.5*delta;
                            yl = y0;
                            yr = y0;
                        } 
                        else 
                        {
                            x0 = plane.getTrackParams(fit).getxPosition();
                            y0 = plane.getTrackParams(typ).getyPosition(); 
                            z0 = plane.getZPlane()+0.1;
                            xl = x0;
                            xr = x0;
                            yl = y0-0.5*delta;
                            yr = y0+0.5*delta;
                        }       
                        m_builder->addPoint(xl,yl,z0);
                        m_builder->addPoint(xr,yr,z0);
                        m_builder->addAttValue("Hit Volume", getTkrIdString(plane.getTkrId()), "");

                        //Build string for the measuring view
                        std::stringstream hitView("No valid hit found");
                        //int clusterId = -1;

                        if (plane.getStatusBits() & Event::TkrTrackHit::HITONFIT)
                        {
                            if (plane.getTkrId().getView() == idents::TkrId::eMeasureX) hitView << "This is an X measuring plane";
                            else                                                        hitView << "This is a Y measuring plane";
                            //clusterId = -1; //plane.getClusterPtr()->id();
                        }

                        m_builder->addAttValue("Projection",hitView.str(),"");
                        //m_builder->addAttValue("ClusterID",clusterId,"");
                        m_builder->addAttValue("Energy",(float)(plane.getEnergy()),"");
                        m_builder->addAttValue("RadLen",(float)(plane.getRadLen()),"");
                        m_builder->addAttValue("ActDist",(float)(plane.getActiveDist()),"");
                        m_builder->addAttValue("ChiSquareFilter",(float)(plane.getChiSquareFilter()),"");
                        m_builder->addAttValue("ChiSquareSmooth",(float)(plane.getChiSquareSmooth()),"");
                        m_builder->addAttValue("Measured",getPositionString(plane.getPoint(Event::TkrTrackHit::MEASURED)),"");
                        m_builder->addAttValue("Filter Position", getPositionString(plane.getPoint(Event::TkrTrackHit::FILTERED)),"");
                        m_builder->addAttValue("Filter Slope",    getSlopeString(plane.getTrackParams(Event::TkrTrackHit::FILTERED)),"");
                        m_builder->addAttValue("Smooth Position", getPositionString(plane.getPoint(Event::TkrTrackHit::SMOOTHED)),"");
                        m_builder->addAttValue("Smooth Slope",    getSlopeString(plane.getTrackParams(Event::TkrTrackHit::SMOOTHED)),"");

                        //Build string for status bits
                        unsigned int      statBits = plane.getStatusBits();
                        m_builder->addAttValue("Hit Status Low",getBits(statBits, 15, 0),"");
                        m_builder->addAttValue("Hit Status High",getBits(statBits, 31, 16),"");
                    }
                }
            }
        }
    }
}


bool TrackFiller::hasType(std::vector<std::string>& list, std::string type) 
{
    if (list.size() == 0) return 1;

    std::vector<std::string>::const_iterator i; 

    i = std::find(list.begin(),list.end(),type);
    if(i == list.end()) return 0;
    else return 1;

}

std::string TrackFiller::getTkrIdString(const idents::TkrId& tkrId)
{
    std::stringstream tkrIdStream;
    //tkrIdStream.setf(std::ios::fixed);

    int tower   = 4 * tkrId.getTowerX() + tkrId.getTowerY();
    int trayNum = tkrId.getTray();

    tkrIdStream << " Tower " << tower << ", tray " << trayNum;

    if (tkrId.getBotTop()==idents::TkrId::eTKRSiTop) {tkrIdStream << " top";}
    else                                             {tkrIdStream << " bot";}

    return tkrIdStream.str();
}

std::string TrackFiller::getTripleString(int precis, double x, double y, double z)
{
    std::stringstream triple;
    triple.setf(std::ios::fixed);
    triple.precision(precis);
    triple << " (" << x << "," << y << "," << z << ")";

    return triple.str();
}

std::string TrackFiller::getPositionString(const Point& position)
{
    int precis = 3;
    return getTripleString(precis, position.x(), position.y(), position.z());
}

std::string TrackFiller::getDirectionString(const Vector& direction)
{
    int precis = 5;
    return getTripleString(precis, direction.x(), direction.y(), direction.z());
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

std::string TrackFiller::getBits(unsigned int statBits, int highBit, int lowBit)
{                    
    std::stringstream outString;
    int bit;
    for (bit=highBit; bit>=lowBit; --bit) {
        outString << (statBits>>(bit)&1) ;
        if (bit%4==0) outString << " ";
    }
    return outString.str();
}
