#include "FitTrackFiller.h"
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

#include <sstream>
#include <iostream>
#include <algorithm>


// Constructor
FitTrackFiller::FitTrackFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    
  
}


// This method build the types for the HepRep
void FitTrackFiller::buildTypes()
{
    m_builder->addType("TkrRecon","TrackCol",
		               "Reconstructed Tracks collections","");

    m_builder->addType("TrackCol","Track","Reconstructed track","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","blue","");
    m_builder->addAttDef("TrackId","Track ID #","Physics","");
    m_builder->addAttDef("Layer","Layer at Track Start","Physics","");
    m_builder->addAttDef("Tower","Tower at Track Start","Physics","");
    m_builder->addAttDef("Energy","Fit Track Energy","Physics","MeV");
    m_builder->addAttDef("Quality","Fit Track Quality","Physics","");
    m_builder->addAttDef("# Hits","Number of Hits on Track","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Slopes","Track start slopes","Physics","");

    m_builder->addType("Track","TkrHitPlane", "Fit Track Hits", "");
    m_builder->addAttDef("Plane","Plane ID","Physics","");
    m_builder->addAttDef("ClusterID","Cluster ID","Physics","");
    m_builder->addAttDef("Energy","Energy at Plane","Physics","MeV");
    m_builder->addAttDef("Projection","Plane Projection","Physics","");
    m_builder->addAttDef("RadLen","Radiation Lengths to Plane","Phsyics","");
    m_builder->addAttDef("ActDist","Active Distance","Physics","mm");
    m_builder->addAttDef("Measured","Hit Position","Physics","");
    m_builder->addAttDef("Position","Fit Position","Physics","");
    m_builder->addAttDef("Slope","Fit Slope","Phyiscs","");
    m_builder->addAttDef("ChiSquare","Chi Square Contribution","Physics","");
}


// This method fill the instance tree Event with the actual TDS content
void FitTrackFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Track"))
    { 
        Event::TkrFitTrackCol* pTracks = 
	    SmartDataPtr<Event::TkrFitTrackCol>(m_dpsvc,EventModel::TkrRecon::TkrFitTrackCol);
      
        //Now see if we can do the drawing
        if (pTracks)
	    {
	        m_builder->addInstance("TkrRecon","TrackCol");

	        int numTracks = pTracks->size();
	  
	        if (numTracks > 0) 
	        {
                int trackId = 0;
	            Event::TkrFitTrackCol::const_iterator it = pTracks->begin();

	            while(it != pTracks->end())
		        {
		            m_builder->addInstance("TrackCol","Track");
		            const Event::TkrFitTrackBase& track = **it++;

        		    Event::TkrFitHit::TYPE  fit      = Event::TkrFitHit::SMOOTH;
		            Event::TkrFitHit::TYPE  typ      = Event::TkrFitHit::SMOOTH;

                    m_builder->addAttValue("TrackId", trackId++, "");
                    m_builder->addAttValue("Layer",track.getLayer(), "");
                    m_builder->addAttValue("Tower",track.getTower(), "");
	                m_builder->addAttValue("Quality", (float)(track.getQuality()), "");
	                m_builder->addAttValue("Energy", (float)(track.getEnergy()), "");
                    m_builder->addAttValue("# Hits", (int)(track.size()), "");
                        
                    //Build string for track start position
                    std::stringstream trkPosition;
                    trkPosition.setf(std::ios::fixed);
                    trkPosition.precision(3);
                    trkPosition << " (" << track.getTrackPar().getXPosition()
                                << ","  << track.getTrackPar().getYPosition() 
                                << ","  << track.getTrackParZ() << ")";
                    m_builder->addAttValue("Start Position",trkPosition.str(),"");
                        
                    //Build string for track start slopes
                    std::stringstream trkSlopes;
                    trkSlopes.setf(std::ios::fixed);
                    trkSlopes.precision(3);
                    trkSlopes << "  mx=" << track.getTrackPar().getXSlope()
                              << ", my="  << track.getTrackPar().getYSlope(); 
                    m_builder->addAttValue("Start Slopes",trkSlopes.str(),"");

                    //first loop through to draw the track
		            for(Event::TkrFitPlaneConPtr hitIter = track.begin(); hitIter < track.end(); hitIter++)
		            {
		                Event::TkrFitPlane plane = *hitIter;

                        double x0 = plane.getHit(typ).getPar().getXPosition();
                        double y0 = plane.getHit(typ).getPar().getYPosition();
                        double z0 = plane.getZPlane();

                        m_builder->addPoint(x0,y0,z0);
                    }

                    //Second loop through to draw the hits
		            for(Event::TkrFitPlaneConPtr hitIter = track.begin(); hitIter < track.end(); hitIter++)
		            {
		                m_builder->addInstance("Track","TkrHitPlane");

		                Event::TkrFitPlane plane = *hitIter;

		                Event::TkrFitPlane::AXIS prj = plane.getProjection();

		                double x0, y0, z0, xl, xr, yl, yr;
		                double delta= plane.getDeltaChiSq(typ)*10.; //Scale factor! We're in mm now!

		                if(prj == Event::TkrCluster::X)
                        {
			                x0 = plane.getHit(typ).getPar().getXPosition();
			                y0 = plane.getHit(fit).getPar().getYPosition(); 
			                z0 = plane.getZPlane()+0.1;
			                xl = x0-0.5*delta;
			                xr = x0+0.5*delta;
			                yl = y0;
			                yr = y0;
		                } 
		                else 
                        {
			                x0 = plane.getHit(fit).getPar().getXPosition();
			                y0 = plane.getHit(typ).getPar().getYPosition(); 
                   	        z0 = plane.getZPlane()+0.1;
			                xl = x0;
                            xr = x0;
                            yl = y0-0.5*delta;
                            yr = y0+0.5*delta;
                        }       
                        m_builder->addPoint(xl,yl,z0);
                        m_builder->addPoint(xr,yr,z0);
                        m_builder->addAttValue("Plane",plane.getIDPlane(),"");
                        m_builder->addAttValue("ClusterID",(int)(plane.getIDHit()),"");
                        m_builder->addAttValue("Energy",(float)(plane.getEnergy()),"");
                        m_builder->addAttValue("Projection",(int)(prj),"");
                        m_builder->addAttValue("RadLen",(float)(plane.getRadLen()),"");
                        m_builder->addAttValue("ActDist",(float)(plane.getActiveDist()),"");
                        m_builder->addAttValue("ChiSquare",(float)(plane.getDeltaChiSq(fit)),"");

                        //Build string for measured hit position
                        std::stringstream hitPosition;
                        hitPosition.setf(std::ios::fixed);
                        hitPosition.precision(3);
                        hitPosition << " (" << plane.getHit(Event::TkrFitHit::MEAS).getPar().getXPosition()
                                    << ","  << plane.getHit(Event::TkrFitHit::MEAS).getPar().getYPosition() 
                                    << ","  << z0 << ")";
                        m_builder->addAttValue("Measured",hitPosition.str(),"");

                        //Build string for position
                        std::stringstream fitPosition;
                        fitPosition.setf(std::ios::fixed);
                        fitPosition.precision(3);
                        fitPosition << " (" << x0 << "," << y0 << "," << z0 << ")";
                        m_builder->addAttValue("Position",fitPosition.str(),"");

                        //Build string for fit slopes
                        std::stringstream fitSlope;
                        fitSlope.setf(std::ios::fixed);
                        fitSlope.precision(5);
                        fitSlope << "  mx=" << plane.getHit(fit).getPar().getXSlope()
                                 << ", my="  << plane.getHit(fit).getPar().getYSlope(); 
                        m_builder->addAttValue("Slope",fitSlope.str(),"");
                    }
                }
	        }
	    }
    }
}


bool FitTrackFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}
